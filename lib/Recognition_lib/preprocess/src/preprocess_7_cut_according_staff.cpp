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
    int* left_most  = new int[staff_count];
    int* right_most = new int[staff_count];
    int* up_most    = new int[staff_count];
    int* down_most  = new int[staff_count];
    for(int i = 0 ; i < staff_count ; i++){
         left_most[i] = 1000; // 隨便很 大 的數
        right_most[i] = -100; // 隨便很 小 的數
           up_most[i] = 1000; // 隨便很 大 的數
         down_most[i] = -100; // 隨便很 小 的數
    }



    // 更新最左右
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        // 需要比較 第一 ~ 五條線的左 右喔！所以要for迴圈
        for(int line_num = 0 ; line_num < STAFF_LINE_COUNT ; line_num++){
            if( left_point[staff_num][line_num][0] <=  left_most[staff_num])  left_most[staff_num] =   left_point[staff_num][line_num][0];
            if(right_point[staff_num][line_num][0] >= right_most[staff_num]) right_most[staff_num] =  right_point[staff_num][line_num][0];
        }
    }

    // 更新最上下
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        // 只需要比較 第一條線 的左右兩點 即可！不用for迴圈
        if(left_point[staff_num][0][1] < right_point[staff_num][0][1]) up_most[staff_num] = left_point [staff_num][0][1];
        else                                                           up_most[staff_num] = right_point[staff_num][0][1];
        // 只需要比較 第五條線 的左右兩點 即可！不用for迴圈
        if(left_point[staff_num][4][1] > right_point[staff_num][4][1]) down_most[staff_num] = left_point [staff_num][4][1];
        else                                                           down_most[staff_num] = right_point[staff_num][4][1];
    }


    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        cout << "left_most [" << left_most [staff_num] << "] = " << left_most [staff_num] << endl;
        cout << "right_most[" << right_most[staff_num] << "] = " << right_most[staff_num] << endl;
        cout << "up_most   [" << up_most   [staff_num] << "] = " << up_most   [staff_num] << endl;
        cout << "down_most [" << down_most [staff_num] << "] = " << down_most [staff_num] << endl;
        // waitKey(0);

        // int tran_base_left = left_most[staff_num];
        // int tran_base_up   = up_most  [staff_num];

        // 0左上  1左下  2右上  3右下
        Point2f dst4P[4];  // distination 4 point
        Point2f src4P[4];  // sorce 4 point，有扣掉 五線譜 左邊、上面 的空白處
        src4P[0] = Point2f(left_point [staff_num][0][0] - left_most[staff_num], left_point [staff_num][0][1] - up_most  [staff_num]);
        src4P[1] = Point2f(left_point [staff_num][4][0] - left_most[staff_num], left_point [staff_num][4][1] - up_most  [staff_num]);
        src4P[2] = Point2f(right_point[staff_num][0][0] - left_most[staff_num], right_point[staff_num][0][1] - up_most  [staff_num]);
        src4P[3] = Point2f(right_point[staff_num][4][0] - left_most[staff_num], right_point[staff_num][4][1] - up_most  [staff_num]);
        for(int i = 0 ; i < 4 ; i++) cout << "src4P[" << i << "].x = " << src4P[i].x << " src4P[" << i << "].y = " << src4P[i].y << endl;

        // 找頭偵錯, 因為 left_point, right_point 的初始值 我是設定 隨便很大的值(1000) 和 隨便很小的值(-1000), 如果找頭失敗 src4P 減完 或者 本身負很大 結果會是負的
        bool right_staff = true;
        for(int i = 0 ; i < 4 ; i++){
            if( src4P[i].x < 0 || (src4P[i].x > src_bin.cols -1) ||
                src4P[i].y < 0 || (src4P[i].y > src_bin.rows -1) ){
                right_staff = false;  // 代表這組有錯誤，跳過這組
                cout << "wrong staff" << endl;
                break;
            }
        }

        ///代表這組有錯誤，跳過這組
        if(right_staff == false){
            staff_count--;
            continue;
        }


        // 現在要找出 要拉到怎麼樣才算正規 的規格~~~
        // 水平x方面：就五線譜長度 ~~ 但每條線好像都會差一咪咪所以取平均
        // 垂直y方面：就staff五條線間隔的和
        // int ave_staff_line_leng = 0;
        // for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        //     for(int line_num = 0 ; line_num < STAFF_LINE_COUNT ; line_num++){
        //         ave_staff_line_leng += right_point[staff_num][line_num][0] - left_point[staff_num][line_num][0];
        //     }
        // }
        // ave_staff_line_leng /= (staff_count*STAFF_LINE_COUNT);
        // int stander_col = 925;   //
        // int stander_row = (stander_col*(src4P[3].y-src4P[0].y) / ave_staff_line_leng  );  //+1
        // int stander_row_expend = stander_row*3;

        // cout << "standard_col = " << stander_col << endl;
        // cout << "standard_row = " << stander_row << endl;
        // cout << "ave_staff_line_leng: " << ave_staff_line_leng << endl;
        // cout << "rate = " << (src4P[3].y - src4P[0].y) / (src4P[3].x - src4P[0].x) << endl;
        // cout << "停";
        // Mat stander(stander_row_expend,stander_col,CV_8UC1);
        
        // 反正就一直嘗試, 覺得 width=1156, height=43 目前還不錯
        int padding_lr = 5;
        dst4P[0] = Point2f(    0 + padding_lr, 43*0);
        dst4P[1] = Point2f(    0 + padding_lr, 43*1 );
        dst4P[2] = Point2f( 1156 + padding_lr, 43*0);
        dst4P[3] = Point2f( 1156 + padding_lr, 43*1);

        for(int i = 0 ; i < 4 ; i++)
            cout << "dst4P[" << i << "].x = " << dst4P[i].x << " dst4P[" << i << "].y = " << dst4P[i].y << endl;

        Mat warp_matrix = getPerspectiveTransform(src4P, dst4P);
        cout << warp_matrix << ' ' << endl;

        for(int i = 0 ; i < 3 ; i++){
            for(int j = 0 ; j < 3 ; j++){
                cout << warp_matrix.at<double>(i,j) << ", ";
            }
            cout << endl;
        }


        // 不能切得剛剛好，上下再多切一些因為有些太高音或太低音這樣子囉
        int extend =(down_most[staff_num] - up_most[staff_num])* EXTEND_RATE;  // 多幾組五線譜的寬度



        ///***************************************************************************************************
        ///***************************************************************************************************
        ///***************************************************************************************************





        ///***************************************************************************************************
        ///***************************************************************************************************
        ///***************************************************************************************************
        ///根據 最左、下、右、上 來切出proc_img
        ///        int staff_width  = src4P[0].x - src4P[2].x;
        ///        int staff_height = src4P[0].y - src4P[1].y;
        Mat src_bin_cropped            = src_bin           (Rect( left_most[staff_num] , up_most[staff_num]-extend, right_most[staff_num] - left_most[staff_num] , down_most[staff_num] - up_most[staff_num] +extend*2));
        Mat src_img_erase_line_cropped = src_bin_erase_line(Rect( left_most[staff_num] , up_most[staff_num]-extend, right_most[staff_num] - left_most[staff_num] , down_most[staff_num] - up_most[staff_num] +extend*2));
        imshow("src_bin_cropped"           ,src_bin_cropped);
        imshow("src_img_erase_line_cropped",src_img_erase_line_cropped);

        // 存debug圖
        string pre7_debug_dir = "debug_img/pre7_cut_staff";
        _mkdir(pre7_debug_dir.c_str()); // 建立 框內影像 存放的資料夾
        stringstream ss;
        ss << setw(2) <<setfill('0') <<staff_num;
        string str_staff_num = ss.str();
        ss.str("");
        ss.clear();
        string debug_path            = pre7_debug_dir + "/" + str_staff_num + ".bmp";
        // string debug_path_erase_line = pre7_debug_dir + "/" + str_staff_num + "_erase_line.bmp";
        imwrite(debug_path           , src_bin_cropped);
        // imwrite(debug_path_erase_line, src_img_erase_line_cropped);

        Mat final_img;
        Mat final_img_erase_line;
        // 小 ~ 大
        if(src_bin_cropped.cols < 1156 + padding_lr * 2 || src_bin_cropped.rows < 43*4){ 
            cout << "case1" << endl;
            int final_img_w;
            int final_img_h;
            if(src_bin_cropped.cols < 1156 + padding_lr * 2)  final_img_w = 1156 + padding_lr * 2;
            if(src_bin_cropped.rows < 43*4)  final_img_h = 43*4;
            Mat temp(final_img_h, final_img_w, CV_8UC1, Scalar(255));
            final_img            = temp.clone();
            final_img_erase_line = temp.clone();
        }
        // 大 ~ 小
        else{
            cout << "case2" << endl;
            final_img            = src_bin_cropped           .clone();
            final_img_erase_line = src_img_erase_line_cropped.clone();
        }
        
        ///不用怕存的空間不夠大~~~但是轉完後要roi，因為原本的圖太大了！會剩很多空白~~~
        
        cout << "final_img.width=" << final_img.cols << ", final_img.height=" <<final_img.rows << endl;
        warpPerspective(src_bin_cropped           , final_img           , warp_matrix, final_img           .size(), 0, 0, 255);
        warpPerspective(src_img_erase_line_cropped, final_img_erase_line, warp_matrix, final_img_erase_line.size(), 0, 0, 255);
        cout << "src_bin_cropped.width=" << src_bin_cropped.cols << ", src_bin_cropped.height=" <<src_bin_cropped.rows << endl;
        cout << "final_img.width=" << final_img.cols << ", final_img.height=" <<final_img.rows << endl;
        string debug_path_warped = pre7_debug_dir + "/" + str_staff_num + "_warped.bmp";
        imwrite(debug_path_warped, final_img);
        // waitKey(0);
        // warpPerspective(src_bin_cropped   , final_img   , warp_matrix, src_bin_cropped   .size(), 0, 0, 0);
        // warpPerspective(src_img_erase_line_cropped, final_img_erase_line, warp_matrix, src_img_erase_line_cropped.size(), 0, 0, 0);
        // warp to strength
        //imshow("warp_perspective",final_img);


        // ****************************************************************************************************
        // ****************************************************************************************************
        // ****************************************************************************************************
        //  找轉換後的底在哪裡(trans_down_point) 就是左下角和右下角去手動轉，然後看哪一個比較"上"面囉！(測過覺得"下"面比較難看)
        //  右下角
        double trans_down_point_right_x = right_most[staff_num] - left_most[staff_num];           // right_most[staff_num];
        double trans_down_point_right_y = down_most [staff_num] - up_most  [staff_num] + extend * 2; //  down_most[staff_num] + extend;
        Perspective_trans(trans_down_point_right_x, trans_down_point_right_y, warp_matrix,
                          trans_down_point_right_x, trans_down_point_right_y);
        // 左下角
        double trans_down_point_left_x = left_most[staff_num];
        double trans_down_point_left_y = down_most[staff_num] - up_most[staff_num] + extend * 2;/// down_most[staff_num] + extend;
        Perspective_trans(trans_down_point_left_x, trans_down_point_left_y, warp_matrix,
                          trans_down_point_left_x, trans_down_point_left_y);
        // 看哪個比較上面
        double trans_down_point = 0;
        if(trans_down_point_left_y < trans_down_point_right_y)
             trans_down_point = trans_down_point_left_y;
        else trans_down_point = trans_down_point_right_y;

        // 用比較下面的點去切roi
        final_img_roi           [staff_num] = final_img           (Rect(0, 0, 1156 + padding_lr * 2, trans_down_point));
        final_img_roi_erase_line[staff_num] = final_img_erase_line(Rect(0, 0, 1156 + padding_lr * 2, trans_down_point));
        ///****************************************************************************************************
        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_0-srcimg.jpg",src_bin_cropped);
        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_1-result.bmp",final_img_roi);
        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_2-rl-result.bmp",final_img_roi_erase_line);
        // ***************************************************************************************************
        // ***************************************************************************************************
        // ***************************************************************************************************
        // 看 "五線譜範圍最左上角的點" 轉換後會在哪裡~~~ 不能直接用 最左邊和最上的點喔!!!! 因為他們並不一定等於五線譜的最左上角的點~~
        if(  left_point[staff_num][0][0] < left_most[staff_num] )
             trans_start_point_x[staff_num] = left_point[staff_num][0][0];
        else trans_start_point_x[staff_num] = left_point[staff_num][0][0] - left_most[staff_num];

        if(  left_point[staff_num][0][1] < up_most[staff_num] )
             trans_start_point_y[staff_num] = left_point[staff_num][0][1];
        else trans_start_point_y[staff_num] = left_point[staff_num][0][1] - up_most  [staff_num] + extend;


        // trans_start_point_x[staff_num] = (trans_proc_2[0]/trans_proc_2[2]);
        // trans_start_point_y[staff_num] = (trans_proc_2[1]/trans_proc_2[2]);
        Perspective_trans( trans_start_point_x[staff_num] , trans_start_point_y[staff_num], warp_matrix,
                           trans_start_point_x[staff_num] , trans_start_point_y[staff_num] );

        cout << "trans_start_point_x = " << trans_start_point_x
            <<", trans_start_point_y = " << trans_start_point_y << endl;

        Mat debug = final_img_roi[staff_num].clone();
        cvtColor(final_img_roi[staff_num],debug,CV_GRAY2BGR);
        line(debug,Point(trans_start_point_x[staff_num],trans_start_point_y[staff_num]),Point(trans_start_point_x[staff_num],trans_start_point_y[staff_num]+43),Scalar(0,255,0),5);
        // imshow("debug2",debug);
        // waitKey(0);
        // imshow("warp_perspective_roi",final_img_roi[staff_num]);
        // imshow("warp_perspective_rl_roi",final_img_roi_erase_line[staff_num]);
        // waitKey(0);

        Mat vertical_map(final_img_roi_erase_line[staff_num].rows,
                         final_img_roi_erase_line[staff_num].cols, CV_8UC1, Scalar(255));
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
