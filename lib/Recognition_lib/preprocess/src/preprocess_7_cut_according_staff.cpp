/*
這個程式的功能是：
利用 preprocess_6 找出的 五線譜端點
把 輸入譜的圖片 以五線譜為單位 盡可能透射成長方形的五線譜 並 做切割(把透射結果放進適當大小的畫布中就有切割的效果囉)
*/

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include <cstring>
#include <direct.h>
#include <iomanip>

#include "preprocess_7_cut_according_staff.h"

#include "string_tools.h"

#define STAFF_LINE_COUNT 5

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
               Mat staff_img_erase_line[],Mat staff_img[],
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
        int padding_U   =    (1.5 + 0.2) * staff_height;  // 超出五線譜外都可能還有音喔, 所以保險起見多留1.5組五線譜的空間這樣子, 高音的部分還要放辨識出來的音高圈圈, 所以多留 0.2 組五線譜
        int padding_D   =     1.5        * staff_height;  // 超出五線譜外都可能還有音喔, 所以保險起見多留1.5組五線譜的空間這樣子,
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
        staff_img           [staff_num] = final_img           .clone();
        staff_img_erase_line[staff_num] = final_img_erase_line.clone();
        // 轉換後的 五線譜 起始x, 起始y (五線譜的左上角)
        trans_start_point_x[staff_num] = padding_L;
        trans_start_point_y[staff_num] = padding_U;



        // 以下都 debug 顯示用
        cout << "trans_start_point_x[staff_num] = " << trans_start_point_x[staff_num]
            <<", trans_start_point_y[staff_num] = " << trans_start_point_y[staff_num] << endl;

        Mat debug_ref_line = staff_img[staff_num].clone();
        cvtColor(staff_img[staff_num], debug_ref_line,CV_GRAY2BGR);
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
        imwrite(debug_path                , staff_img           [staff_num]);
        imwrite(debug_path_erase_line     , staff_img_erase_line[staff_num]);
        imwrite(debug_path_reference_line , debug_ref_line);
        // waitKey(0);

    }

}
