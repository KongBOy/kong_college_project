/*
這個程式的功能是：
利用 preprocess_6 找出的 五線譜端點
把 輸入譜的圖片 以五線譜為單位做切割(根據 每組五線譜端點 做切割
*/

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include <cstring>
#include <direct.h>
#include <iomanip>


#include "preprocess_7_cut_according_staff.h"

// #include "vertical_map_to_recognize.h"



#include "string_tools.h"

#define HORIZONTAL_DIR "horizontal_test/"

#define EXTEND_RATE 1.0
#define STAFF_LINE_COUNT 5
#define X_Y_COUNT 2



using namespace cv;
using namespace std;


void Perspective_trans(double x,double y,Mat warp_matrix,double & result_x,double& result_y){
    // 參考網站:
    // https://theailearner.com/tag/cv2-getperspectivetransform/
    // https://medium.com/analytics-vidhya/opencv-perspective-transformation-9edffefb2143
    float trans_proc_1[3];

    trans_proc_1[0] = x;
    trans_proc_1[1] = y;
    trans_proc_1[2] = 1;
    // cout << "_1[0] = " << trans_proc_1[0] << " , _1[1] = " << trans_proc_1[1] << " , _1[2] = " << trans_proc_1[2] << endl;

    float trans_proc_2[3];
    for(int j = 0 ; j < 3 ; j++){
        trans_proc_2[j]  = trans_proc_1[0]*warp_matrix.at<double>(j,0);
        trans_proc_2[j] += trans_proc_1[1]*warp_matrix.at<double>(j,1);
        trans_proc_2[j] += trans_proc_1[2]*warp_matrix.at<double>(j,2);
    }
    cout << "_2[0] = " << trans_proc_2[0] << " , _2[1] = " << trans_proc_2[1] << " , _2[2] = " << trans_proc_2[2] << endl;

    result_x = (trans_proc_2[0] / trans_proc_2[2]);
    result_y = (trans_proc_2[1] / trans_proc_2[2]);
    cout << endl;


    cout <<   "result_x = " << result_x
         <<" , result_y = " << result_y << endl;

}


//根據 每組staff 的 最左、右、上、下 切出proc_img
//根據 每組staff 的 第一條和第五條的線頭 assign srdTri
void Cut_staff(Mat src_bin,Mat src_bin_erase_line,
               int& staff_count, int*** left_point, int*** right_point,
               Mat final_img_roi_erase_line[],Mat final_img_roi[],
               double trans_start_point_x[],double trans_start_point_y[])
               // trans_start_point的意思是 五線譜最左上角的那一點
{
    // 初始化 每組五線譜的最左右端 的 max/min, 左端邊是 越左越max 偏右越min, 右端是 越右越max 偏左越min
    int* left_max  = new int[staff_count];
    int* left_min  = new int[staff_count];
    int* right_max = new int[staff_count];
    int* right_min = new int[staff_count];
    for(int i = 0 ; i < staff_count ; i++){
         left_max[i] =  10000; // 隨便很 大 的數
         left_min[i] = -10000; // 隨便很 小 的數
        right_max[i] = -10000; // 隨便很 小 的數
        right_min[i] =  10000; // 隨便很 大 的數
    }



    // 更新每組五線譜的最左右端 的 max/min, 左端邊是 越左越max 偏右越min, 右端是 越右越max 偏左越min
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        // 需要比較 第一 ~ 五條線的左 右喔！所以要for迴圈
        for(int line_num = 0 ; line_num < STAFF_LINE_COUNT ; line_num++){
            if( left_point[staff_num][line_num][0] <=  left_max[staff_num])  left_max[staff_num] =   left_point[staff_num][line_num][0];
            if(right_point[staff_num][line_num][0] >= right_max[staff_num]) right_max[staff_num] =  right_point[staff_num][line_num][0];
            if( left_point[staff_num][line_num][0] >=  left_min[staff_num])  left_min[staff_num] =   left_point[staff_num][line_num][0];
            if(right_point[staff_num][line_num][0] <= right_min[staff_num]) right_min[staff_num] =  right_point[staff_num][line_num][0];
        }
    }


    // 每組五線譜 先 看看頭有沒有明顯的錯誤, 沒有的話 就抓出五線譜上的 0左上  1左下  2右上  3右下 的頭, 做透射到 一個 固定大小的尺寸的五線譜囉
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        cout << "left_max [" << left_max [staff_num] << "] = " << left_max [staff_num] << endl;
        cout << "left_min [" << left_min [staff_num] << "] = " << left_min [staff_num] << endl;
        cout << "right_max[" << right_max[staff_num] << "] = " << right_max[staff_num] << endl;
        cout << "right_min[" << right_min[staff_num] << "] = " << right_min[staff_num] << endl;
        
        // 找頭偵錯, 如果兩端的 min/max 差距超過30 大概就是找頭出問題了
        if( left_min [staff_num] - left_max[staff_num]  > 30 || 
            right_max[staff_num] - right_min[staff_num] > 30 ||
            left_max [staff_num] < 0 ||
            right_max[staff_num] > src_bin.cols -1){
            cout << "wrong staff" << endl;
            staff_count--;
            continue;
        }

        // 標出來源的 0左上  1左下  2右上  3右下
        Point2f dst4P[4];  // distination 4 point
        Point2f src4P[4];  // sorce 4 point
        src4P[0] = Point2f(left_point [staff_num][0][0] , left_point [staff_num][0][1] );
        src4P[1] = Point2f(left_point [staff_num][4][0] , left_point [staff_num][4][1] );
        src4P[2] = Point2f(right_point[staff_num][0][0] , right_point[staff_num][0][1] );
        src4P[3] = Point2f(right_point[staff_num][4][0] , right_point[staff_num][4][1] );
        for(int i = 0 ; i < 4 ; i++) cout << "src4P[" << i << "].x = " << src4P[i].x << " src4P[" << i << "].y = " << src4P[i].y << endl;

        // 標出轉換後的  0左上  1左下  2右上  3右下, 反正就一直換圖嘗試, 目前覺得 width=1156, height=43 還不錯
        int staff_width  = 1156;
        int staff_height =   43;
        int padding_L   =    5;
        int padding_R   =    5;
        int padding_U   =    1.5 * staff_height;
        int padding_D   =    1.5 * staff_height;
        dst4P[0] = Point2f(       0     + padding_L, padding_U                );
        dst4P[1] = Point2f(       0     + padding_L, padding_U + staff_height );
        dst4P[2] = Point2f( staff_width + padding_L, padding_U                );
        dst4P[3] = Point2f( staff_width + padding_L, padding_U + staff_height );
        for(int i = 0 ; i < 4 ; i++) cout << "dst4P[" << i << "].x = " << dst4P[i].x << " dst4P[" << i << "].y = " << dst4P[i].y << endl;

        // 取得轉換矩陣
        Mat warp_matrix = getPerspectiveTransform(src4P, dst4P);
        cout << warp_matrix << ' ' << endl;
        for(int i = 0 ; i < 3 ; i++){
            for(int j = 0 ; j < 3 ; j++){
                cout << warp_matrix.at<double>(i,j) << ", ";
            }
            cout << endl;
        }
        
        // 建立放轉換後影像的畫布
        int final_img_w = staff_width  + padding_L + padding_R;
        int final_img_h = staff_height + padding_U + padding_D;
        Mat temp(final_img_h, final_img_w, CV_8UC1, Scalar(255));
        Mat final_img            = temp.clone();
        Mat final_img_erase_line = temp.clone();

        // 透射轉換
        warpPerspective(src_bin           , final_img           , warp_matrix, final_img           .size(), 0, 0, 255);
        warpPerspective(src_bin_erase_line, final_img_erase_line, warp_matrix, final_img_erase_line.size(), 0, 0, 255);

        // 渲染出去給下個階段用
        final_img_roi           [staff_num] = final_img           .clone();
        final_img_roi_erase_line[staff_num] = final_img_erase_line.clone();
        // 轉換後的 五線譜 起始x, 起始y (五線譜的左上角)
        trans_start_point_x[staff_num] = padding_L;
        trans_start_point_y[staff_num] = padding_U;



        // 以下都 debug 顯示用
        cout << "trans_start_point_x[staff_num] = " << trans_start_point_x[staff_num]
            <<", trans_start_point_y[staff_num] = " << trans_start_point_y[staff_num] << endl;

        Mat debug_ref_line = final_img_roi[staff_num].clone();
        cvtColor(final_img_roi[staff_num], debug_ref_line,CV_GRAY2BGR);
        Point LT_warped = Point(trans_start_point_x[staff_num]              , trans_start_point_y[staff_num]               );
        Point LD_warped = Point(trans_start_point_x[staff_num]              , trans_start_point_y[staff_num] + staff_height);
        Point RT_warped = Point(trans_start_point_x[staff_num] + staff_width, trans_start_point_y[staff_num]              );
        Point RD_warped = Point(trans_start_point_x[staff_num] + staff_width, trans_start_point_y[staff_num] + staff_height);
        line(debug_ref_line, LT_warped, LD_warped, Scalar(0,255,0), 2);  // L
        line(debug_ref_line, RT_warped, RD_warped, Scalar(0,255,0), 2);  // R
        line(debug_ref_line, LT_warped, RT_warped, Scalar(0,255,0), 2);  // T
        line(debug_ref_line, LD_warped, RD_warped, Scalar(0,255,0), 2);  // D
        // imshow("debug_ref_line",debug_ref_line);

        // 存debug圖
        string pre7_debug_dir = "debug_img/pre7_cut_staff";
        _mkdir(pre7_debug_dir.c_str()); // 建立 框內影像 存放的資料夾
        stringstream ss;
        ss << setw(2) <<setfill('0') <<staff_num;
        string str_staff_num = ss.str();
        ss.str("");
        ss.clear();
        string debug_path                = pre7_debug_dir + "/" + str_staff_num + ".bmp";
        string debug_path_erase_line     = pre7_debug_dir + "/" + str_staff_num + "_erase_line.bmp";
        string debug_path_reference_line = pre7_debug_dir + "/" + str_staff_num + "_reference_line.bmp";
        imwrite(debug_path                , final_img_roi           [staff_num]);
        imwrite(debug_path_erase_line     , final_img_roi_erase_line[staff_num]);
        imwrite(debug_path_reference_line , debug_ref_line);

        // waitKey(0);
        // imshow("warp_perspective_roi",final_img_roi[staff_num]);
        // imshow("warp_perspective_rl_roi",final_img_roi_erase_line[staff_num]);
        // waitKey(0);

        // Mat vertical_map(final_img_roi_erase_line[staff_num].rows,
        //                  final_img_roi_erase_line[staff_num].cols, CV_8UC1, Scalar(255));

        // Vertical_map_to_recognize(final_img_roi_erase_line,vertical_map,final_img_roi,trans[0].y);

        // ***************************************************************************************************
        // if(staff_num < 4 ) continue;

        /*
        int l_edge[200];
        int r_edge[200];
        int distance[200];
        int mountain_area[200];
        int e_count = 0;
        int* note_type;
        recognition_0_vertical_map_to_speed_up(final_img_roi_erase_line[staff_num],
                                               vertical_map,
                                               e_count,l_edge,r_edge,distance,mountain_area,note_type);
        */



        /*
        // 自己設資料結構 head
        int maybe_head_count = 0;
        float maybe_head[3][200];
        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                maybe_head[i][j] = 0;


        // 自己設資料結構 line
        short lines[3][200];///[0]頂點x [1]頂點y [2]長度
        bool lines_dir[2][200];///[0]左下 [1]右上

        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                lines[i][j] = 0;

        for(int i = 0 ; i < 2 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                lines_dir[i][j] = false;

        int lines_count = 0;

        int lines_time[200];
        for(int i = 0 ; i < 200 ; i++) lines_time[i] = 0;
        */
        /*
        // 自己設資料結構 note ~~~~~~~寫出去~~~~~~~~~

        int row_note[5][1000]; /// 0 = x , 1 = y , 2 = type , 3 = time_bar , 4 = 音高
        int row_note_count = 0;
        for(int i = 0 ; i < 5 ; i++)
            for(int j = 0 ; j < 1000 ; j++)
                row_note[i][j] = 0;
        */



        /// ● 0 全音符
        /// 1 全休止符
        /// ● 2 二分 音符
        /// 3 二分 休止符
        /// ● 4 四分 音符
        /// ● 5 四分 休止符
        /*
        Mat template_img_4 = imread("Resource/note/4/4.bmp",0);
        Mat template_img_4_rest = imread("Resource/note/4-rest/4-rest.bmp",0);
        Mat template_img_2 = imread("Resource/note/2/2.bmp",0);
        Mat template_img_0 = imread("Resource/note/0/0.bmp",0);
        Mat template_img_1 = imread("Resource/note/0-rest/0-rest.bmp",0);
        Mat template_img_3 = imread("Resource/note/2-rest/2-rest.bmp",0);
        Mat template_img_8 = imread("Resource/note/8-rest/8-rest-3.bmp",0);
        Mat template_img_6 = imread("Resource/note/6-rest/6-rest-2.bmp",0);

        recognition_0_all_head(1,template_img_1,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(3,template_img_3,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        */
        /*
        recognition_0_all_head(4,template_img_4,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(2,template_img_2,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(0,template_img_0,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(5,template_img_4_rest,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);


        recognition_0_all_head(8,template_img_8,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(6,template_img_6,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        */
        // recognition_0_all_head(7,template_img_8,final_img_roi_erase_line[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);



        /*
        bubbleSort_note(row_note_count,row_note,Y_INDEX);
        bubbleSort_note(row_note_count,row_note,X_INDEX);

        recognition_5_find_pitch(final_img_roi[staff_num],Mat(15,15,CV_8UC1),row_note_count,row_note,trans[0].y);



        // midi(row_note_count,row_note);

        cout << endl << endl;
        cout << "print row_note~~~" << endl;
        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            cout << "go_row_note = " << go_row_note
                <<" , type = " << row_note[2][go_row_note]
                <<" , x = " << row_note[0][go_row_note]
                <<" , y = " << row_note[1][go_row_note]
                <<" , time = " << row_note[3][go_row_note]
                <<" , pitch = " << row_note[4][go_row_note]
                <<endl;
        }
        cout << "row_note_count = " << row_note_count << endl;

        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            note[0][note_count+go_row_note] = row_note[0][go_row_note];
            note[1][note_count+go_row_note] = row_note[1][go_row_note];
            note[2][note_count+go_row_note] = row_note[2][go_row_note];
            note[3][note_count+go_row_note] = row_note[3][go_row_note];
            note[4][note_count+go_row_note] = row_note[4][go_row_note];
        }
        note_count += row_note_count;
        // play();
        */
    }

}
