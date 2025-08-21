/*
這個程式的功能是：
找出符桿的時間長度
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "recognition_0_debug_tools.h"
#include "recognition_3_b_find_time_bar.h"

#define DOWNTOTOP 1
#define TOPTODOWN 0

using namespace cv;
using namespace std;

int find_bars_time( Mat reduce_line, int line_y, int left , int right , int test_depth , bool direction , Mat & debug_img){

    const int time_bar_type_count = 5;  // 0 1 2 3, 4 是垃圾桶，所以共五種
    int  white_coda    = 0;
    int  white_ok_coda = 5;
    bool have_black    = false;
    int  error_shift   = 13;

    int stand_x = left;    // 不會動、固定的x
    int stand_y = line_y;  // 不會動、固定的y
    // cout << "stand_x = " << stand_x << ", stand_y = " << stand_y << endl;

    int time_bar[time_bar_type_count];
    for(int i = 0 ; i < time_bar_type_count ; i++) time_bar[i] = 0;

    
    
    if(direction == DOWNTOTOP){
        for(int go_x = stand_x ; go_x <= right ; go_x++){
            // 先位移到黑色的位置
            stand_y = line_y;
            for(int go_error = 0 ; go_error < error_shift ; go_error++){
                //  *********** 防呆 ***********
                if( (stand_y +go_error +1 > reduce_line.rows -1) ||
                    (stand_y +go_error    > reduce_line.rows -1) ||
                    (stand_y -go_error +1 < 0) ||
                    (stand_y -go_error    < 0)   )break;
                //  *********** 防呆 ***********

                if( (reduce_line.at<uchar>(stand_y + go_error + 1 , go_x) == 255) && (reduce_line.at<uchar>(stand_y + go_error, go_x) == 0) ){
                    stand_y += go_error;
                    break;
                }
                if( (reduce_line.at<uchar>(stand_y - go_error + 1 , go_x) == 255) && (reduce_line.at<uchar>(stand_y - go_error, go_x) == 0) ){
                    stand_y -= go_error;
                    break;
                }
            }

            // 開始移動 go_y
            int go_y = stand_y;
            // **************************
            // line(debug_img, Point(go_x, stand_y), Point(go_x, stand_y - test_depth), Scalar(123, 245, 210), 1);

            // 怕 time_bar 是斜的, 所以一開始就是白連續的一堆白色, 所以 需要white_coda 計算
            // 因為我現在是要做很多次, 所以white_coda要設回initial值！要不然上個的結果會被繼續用!!!
            white_coda = 0;
            for(int go_depth = 0 ; go_depth < test_depth ; go_depth++){
                if( stand_y - go_depth < 0) break;  // 防呆
                if( reduce_line.at<uchar>( stand_y - go_depth , go_x) == 0 ){
                    go_y -= white_coda;  // 如果之前有測到white，white_coda要減回來，如果沒有測到white，反正white_coda會是0所以減了也沒差，所以不用寫if分case囉！
                    go_y--;
                    white_coda = 0;

                    line(debug_img, Point(go_x, stand_y - go_depth), Point(go_x, stand_y - go_depth), Scalar(255,   0,   0), 2);  // 藍色
                    line(debug_img, Point(go_x, go_y              ), Point(go_x, go_y              ), Scalar(230, 100, 150), 2);  // 淺紫色
                    // imshow("time", debug_img);
                    // waitKey(0);
                }
                else{
                    white_coda++;
                }
                if(white_coda > white_ok_coda) break;  // 連續出現white_ok_coda個全白，就代表找到頭了
            }

            int time_bar_length = abs(go_y - stand_y);
            if(time_bar_length > 0) time_bar_length = time_bar_length / 13 + 1;

            if(time_bar_length < 4 ) time_bar[time_bar_length]++;
            else time_bar[4]++;

            // cout註解 看找完time_bar的狀況
            // cout << "left = " << left << ", right = " << right << ", go_x = " << go_x << ", bar = " << abs(go_y - stand_y) << endl;
            line( debug_img , Point(go_x, go_y), Point(go_x, go_y), Scalar(0, 0, 255), 3, CV_AA);
            // imshow("debug_img", debug_img);
            // waitKey(0);
            // 找右邊的線  END 每條線的 線頭(right_line_t)都應該要被找到囉！
        }

    }
    else if(direction == TOPTODOWN){
        for(int go_x = stand_x ; go_x <= right ; go_x++){
            // cout << "go_x = " << go_x << endl;
            
            stand_y = line_y;
            for(int go_error = 0 ; go_error < error_shift ; go_error++){

                // cout << "go_error = " << go_error << endl;
                // *********** 防呆 ***********
               if(  (stand_y -go_error +1 > reduce_line.rows -1) ||
                    (stand_y +go_error    > reduce_line.rows -1) ||
                    (stand_y -go_error -1 < 0) ||
                    (stand_y -go_error    < 0)   ) break;
                // *********** 防呆 ***********
                if( (reduce_line.at<uchar>(stand_y + go_error -1 , go_x) == 255) && (reduce_line.at<uchar>(stand_y + go_error, go_x) == 0) ){
                    stand_y += go_error;
                    break;
                }
                if( (reduce_line.at<uchar>(stand_y - go_error -1 , go_x) == 255) && (reduce_line.at<uchar>(stand_y - go_error, go_x) == 0) ){
                    stand_y -= go_error;
                    break;
                }
            }

            //  開始移動 go_y
            int go_y = stand_y;
            // **************************
            // line(debug_img, Point(go_x, stand_y), Point(go_x, stand_y + test_depth), Scalar(123, 245, 210), 1);

            // 怕 time_bar 是斜的, 所以往右看的時候一開始就是白連續的一堆白色, 所以先位移到正確的位置
            // 因為我現在是要做很多次，所以white_coda要設回initial值！要不然上個的結果會被繼續用!!!
            white_coda = 0;
            for(int go_depth = 0 ; go_depth < test_depth ; go_depth++){
                // cout << "go_depth = " << go_depth << endl;
                if( stand_y + go_depth > reduce_line.rows -1) break;  // 防呆
                if( reduce_line.at<uchar>( stand_y + go_depth , go_x) == 0 ){
                    go_y += white_coda;  // 如果之前有測到white，white_coda要減回來，如果沒有測到white，反正white_coda會是0所以減了也沒差，所以不用寫if分case囉！
                    go_y ++;
                    // cout << "white_coda = " << white_coda << endl;
                    white_coda = 0;

                    line(debug_img, Point(go_x, stand_y + go_depth), Point(go_x, stand_y + go_depth), Scalar(255, 0, 0), 2);
                    line(debug_img, Point(go_x, go_y), Point(go_x, go_y), Scalar(230, 100, 150), 2);
                    // imshow("time", debug_img);
                    // waitKey(0);
                }
                else{
                    white_coda++;
                }
                if(white_coda > white_ok_coda) break;  // 連續出現white_ok_coda個全白，就代表找到頭了
            }

            int time_bar_length = abs(go_y - stand_y);
            if(time_bar_length > 0) time_bar_length = time_bar_length / 13 +1;

            if(time_bar_length < 4 ) time_bar[time_bar_length]++;
            else time_bar[4]++;

            line( debug_img , Point(go_x, go_y), Point(go_x, go_y), Scalar(0, 0, 255), 3, CV_AA);
            // 找右邊的線  END 每條線的 線頭(right_line_t)都應該要被找到囉！
        }


    // for(int i = 0 ; i < time_bar_type_count ; i++)    cout << time_bar[i] << ' ';
    // cout << endl;

    }

    int max_place = 0;
    for(int i = 1 ; i < (time_bar_type_count -1);i++){ // 垃圾桶不用比，所以-1
        if(time_bar[max_place] < time_bar[i]) max_place = i;
    }
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


        // 定位出 right框, left框 的 上的位置 (或 TOPTODOWN 框的下的位置), 因為上下跟y座標有關係, 所以需要根據 bar_dir 的方向來走 到符桿上(或符桿下)
        int time_start_search_y;
        if(bar_dir == false) time_start_search_y = bar_y - bar_len;  // bar_dir 為 DOWNTOTOP
        else                 time_start_search_y = bar_y + bar_len;  // bar_dir 為 TOPTODOWN

        // 定位完 right框, left框 後就可以把 time_length 找出來囉(bar_dir 為 TOPTODOWN 同理)
        int length_r = find_bars_time(reduce_line, time_start_search_y, time_right_l, time_right_r, test_depth, bar_dir, debug_img);
        int length_l = find_bars_time(reduce_line, time_start_search_y, time_left_l , time_left_r , test_depth, bar_dir, debug_img);
        if(length_l > length_r) bars_time[go_bar] = length_l;
        else                    bars_time[go_bar] = length_r;

    }

    // imshow("debug", debug_img);
    // waitKey(0);
    // waitKey(0);
}
