/*
這個程式的功能是：
找出符桿的時間長度
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "Bar_tool.h"
#include "recognition_3_b_find_time_bar.h"

#define DOWNTOTOP 0
#define TOPTODOWN 1

using namespace cv;
using namespace std;

int find_bars_time( Mat reduce_line, int left , int right , int test_depth, int bar_y, int bar_len, bool direction , Mat & debug_img){
    // 在指定的 left ~ right範圍內 的 每個 x,
    // 根據 bar_direction 從 bar_y 走到符桿的位置 搜尋 符桿種類
    // 以 bar_direction 如果是 DOWNTOTOP 為例, 代表 bar 是從下走到上可以 找到符桿, 所以就先從下往上走到底, 再往下(one_step == +1)搜尋符桿就搜的到了, 反之亦然(TOPTODOWN)
    int start_search_y;
    if(direction == false) start_search_y = bar_y - bar_len;  // bar_dir 為 DOWNTOTOP
    else                   start_search_y = bar_y + bar_len;  // bar_dir 為 TOPTODOWN

    int one_step;
    if     (direction == DOWNTOTOP) one_step = +1;
    else if(direction == TOPTODOWN) one_step = -1;
    cv::rectangle(debug_img, cv::Point(left, start_search_y + test_depth * one_step), cv::Point(right, start_search_y), cv::Scalar(123, 251, 20), 1);  // 螢光綠


    // 每個x都會找到自己的 八分符桿種類 並到該類別++ 做累積,
    // left ~ right 走完累積最多的 那種八分符桿種類 就是最終找到的結果
    const int time_bar_type_amount = 5;  // 0 1 2 3 個八分符桿, 4 是垃圾桶，所以共五種
    int time_bar_acc[time_bar_type_amount];
    for(int i = 0 ; i < time_bar_type_amount ; i++) time_bar_acc[i] = 0;


    // 將 start_search_y 位移到確定是黑色的 符桿上, 就將這個位置定為 基準位置floor_y, 要這樣做是因為怕 符桿 會因為樂句是上行 或 下行 是斜的, 所以一開始可能是白連續的一堆白色(不在符桿上)
    // 會用到的變數先宣告在這邊
    const int find_floor_range = 13;
    cv::rectangle(debug_img, cv::Point(left, start_search_y + find_floor_range), cv::Point(right, start_search_y - find_floor_range), cv::Scalar(0, 0, 255), 1);  // 紅色
    cv::imshow("finding floor range", debug_img);
    cv::waitKey(0);
    int floor_y;  // 定位符桿頭的基準y
    int stand_curr_u_row;
    int stand_curr_d_row;
    int stand_befo_u_row;
    int stand_befo_d_row;
    // 以基準floor_y為起點, 開始找time_length 用的變數宣告在這邊
    int go_y;
    int curr_row;
    int white_coda = 0;
    const int white_ok_coda = 5;
    // 走訪指定的 left ~ right範圍內 的 每個 x
    for(int go_x = left ; go_x <= right ; go_x++){
        // 將 start_search_y 位移到確定是黑色的 符桿上, 移到黑色的位置時 不能只單純看本格是黑色而已, 還要自己的前一格是白色 才比較能保證是在邊緣的地方
        floor_y = start_search_y;
        for(int go_range = 0 ; go_range < find_floor_range ; go_range++){
            // 先往下找 本格黑 前格白, 再往上找, 其實順序沒差, 因為也不知道現在是 上行或下行, 所以兩個方向哪個先跑都沒有優勢
            stand_curr_d_row = floor_y + go_range;
            stand_befo_d_row = floor_y + go_range - one_step;
            //  *********** 防呆 ***********
            if( (stand_curr_d_row > reduce_line.rows -1) || (stand_curr_d_row < 0) ||
                (stand_befo_d_row > reduce_line.rows -1) || (stand_befo_d_row < 0)) break;
            //  *********** 防呆 ***********
            if((reduce_line.at<uchar>(stand_curr_d_row, go_x) == 0) && (reduce_line.at<uchar>(stand_befo_d_row , go_x) == 255) ){
                floor_y += go_range;
                break;
            }
            
            // 再往上找 本格黑 前格白
            stand_curr_u_row = floor_y - go_range;
            stand_befo_u_row = floor_y - go_range - one_step;
            //  *********** 防呆 ***********
            if( (stand_curr_u_row > reduce_line.rows -1) || (stand_curr_u_row < 0) ||
                (stand_befo_u_row > reduce_line.rows -1) || (stand_befo_u_row < 0)) break;
            //  *********** 防呆 ***********
            if((reduce_line.at<uchar>(stand_curr_u_row, go_x) == 0) && (reduce_line.at<uchar>(stand_befo_u_row , go_x) == 255) ){
                floor_y -= go_range;
                break;
            }
        }

        // **************************
        // 基準floor_y為起點, 開始移動 go_y 找 time_length
        go_y = floor_y;
        
        // white_coda 用來記錄 白色容忍值, 一方面怕原始影像品質差中間有斷掉要留些coda跳過空白, 另一方面也是找到頭的線索
        // 因為我現在是要做很多次, 所以white_coda要設回initial值, 要不然上個的結果會被繼續用喔~
        white_coda = 0;
        // 走訪此 go_x 內 指定深度的 所有 go_depth
        for(int go_depth = 0 ; go_depth < test_depth ; go_depth++){
            curr_row = floor_y + go_depth * one_step;
            if( curr_row < 0 || curr_row > reduce_line.rows -1 ) break;  // 防呆

            // 如果 有遇到黑色, 就更新 go_y
            if( reduce_line.at<uchar>( curr_row , go_x) == 0 ){
                go_y = curr_row;
                white_coda = 0;
                
                line(debug_img, Point(go_x, go_y), Point(go_x, go_y), Scalar(230, 100, 150), 2);  // 淺紫色
                // imshow("time", debug_img);
                // waitKey(0);
            }
            // 如果 沒遇到黑色, 就 白色容忍值 累加
            else{
                white_coda++;
            }
            // 當連續出現 white_coda_ok 個row 是全白的, 就代表找到頭了
            if(white_coda > white_ok_coda) break;  // 連續出現white_ok_coda個全白，就代表找到頭了
        }

        // 計算 時間長度, 除13 是觀察很多樣本後 覺得 一個八分符桿 的長度大概就 13pixel 這樣子
        int time_bar_length = abs(go_y - floor_y);
        if(time_bar_length > 0) time_bar_length = time_bar_length / 13 + 1;

        // 如果 時間長度太長( >= 4)就垃圾筒的位置++, 剩下的就根據 time_bar_length 的位置++做累積
        if(time_bar_length < 4 ) time_bar_acc[time_bar_length]++;
        else                     time_bar_acc[4]++;

        // cout註解 看找完time_bar的狀況
        cout << "left = " << left << ", right = " << right << ", go_x = " << go_x << ", time_bar_length = " << time_bar_length << endl;
        // line( debug_img , Point(go_x, go_y), Point(go_x, go_y), Scalar(0, 0, 255), 3, CV_AA);
        // imshow("debug_img", debug_img);
        // waitKey(0);
    }

    // 看 time_bar_length 哪邊累積的最多就候選
    int max_place = 0;
    for(int i = 1; i < (time_bar_type_amount -1); i++){  // 垃圾桶不用比，所以-1
        if(time_bar_acc[max_place] < time_bar_acc[i]) max_place = i;
    }
    cout << "max_place:" << max_place << endl;
    return max_place;
}


void recognition_3_b_find_time_bar(Mat template_img, 
                                   int bars_count, short bars[][200], bool bars_dir[200], 
                                   int bars_time[200],
                                   Mat reduce_line){
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    // 開始判斷時間長度囉, 定位出 符桿的起始位置(bar順著bar_dir走到尾 然後 往自身偏外幾個pixel後, 框一個小範圍找 符桿)
    // tr = top_right, tl = top_left, dr = down_right, dl = down_left
    int  test_width;  // 框一個小範圍的寬度
    int  test_depth;  // 框一個小範圍的高度
    int  shift;       // 自身外偏儀幾個pixel
    int  bar_x;       // bar連接head的x座標, 為了增加可讀性, 先把要用的東西拿出來
    int  bar_y;       // bar連接head的y座標, 為了增加可讀性, 先把要用的東西拿出來
    int  bar_len;     // bar的長度         , 為了增加可讀性, 先把要用的東西拿出來
    bool bar_dir;     // bar的方向         , 為了增加可讀性, 先把要用的東西拿出來
    for(int go_bar = 0 ; go_bar < bars_count ; go_bar++){
        // cout << "go_bar = " << go_bar;
        test_width = 7;                      // 框一個小範圍的寬度
        shift      = 2;                      // 自身外偏儀幾個pixel
        test_depth = template_img.rows*2.0;  // 框一個小範圍的高度
        bar_x      = bars[0][go_bar];        // bar連接head的x座標, 為了增加可讀性, 先把要用的東西拿出來
        bar_y      = bars[1][go_bar];        // bar連接head的x座標, 為了增加可讀性, 先把要用的東西拿出來
        bar_len    = bars[2][go_bar];        // bar的長度         , 為了增加可讀性, 先把要用的東西拿出來
        bar_dir    = bars_dir[go_bar];       // bar的方向         , 為了增加可讀性, 先把要用的東西拿出來

        // 以下都是用 DOWNTOTOP 來舉例
        // 定位出bar找符桿的 right框, left框 各別的 l, r, 因為 只是定位左右, 跟y座標沒關係 不需要看 bar_dir, 所以 TOPTODOWN 可以直接套用
        //  ******** 上右 *********
        int time_right_l = bar_x + shift;              // right框 的 右
        int time_right_r = time_right_l + test_width;  // right框 的 左
        //  防呆
        if(time_right_l < 0 ) time_right_l = 0;
        if(time_right_r > reduce_line.cols-1) time_right_r = reduce_line.cols -1;

        // ******** 上左 ********
        int time_left_r = bar_x - shift;             // left框 的 右
        int time_left_l = time_left_r - test_width;  // left框 的 左
        //  防呆
        if(time_left_l < 0 ) time_left_l = 0;
        if(time_left_r > reduce_line.cols-1) time_left_r = reduce_line.cols -1;
        // cout << "time_left_l = " << time_left_l << ", time_left_r = " << time_left_r << endl;


        // 定位完 right框, left框 後就可以把 time_length 找出來囉(bar_dir 為 TOPTODOWN 同理)
        int length_r = find_bars_time(reduce_line, time_right_l, time_right_r, test_depth, bar_y, bar_len, bar_dir, debug_img);
        int length_l = find_bars_time(reduce_line, time_left_l , time_left_r , test_depth, bar_y, bar_len, bar_dir, debug_img);
        if(length_l > length_r) bars_time[go_bar] = length_l;
        else                    bars_time[go_bar] = length_r;

    }

    // imshow("debug", debug_img);
    // waitKey(0);
    // waitKey(0);
}
