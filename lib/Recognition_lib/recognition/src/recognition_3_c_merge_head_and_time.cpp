/*
這個程式的功能是：
把符桿的時間長度和符號合併
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include "recognition_0_debug_tools.h"
#include "recognition_3_c_merge_head_and_time.h"

using namespace std;
using namespace cv;

void recognition_4_merge_head_and_time(int head_type, 
                                       Mat template_img, Mat reduce_line, 
                                       int maybe_head_count, float maybe_head[][200], 
                                       int bars_count, short bars[][200], bool bars_dir[][200], int bars_time[200], 
                                       int& note_count, int note[][1000]){
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    draw_bars(debug_img, bars_count, bars, bars_dir);
    // 合併成真的NOTE囉
    int dist_error = 4;
    int go_bar = 0;
    // cout註解 確認一下想像中的range對不對
    // cout << "((template_img.cols + dist_error) * -1) = " << ((template_img.cols + dist_error) * -1) << endl;

    for(int go_head = 0; go_head < maybe_head_count; go_head++){
        // cout註解 看一下頭的資訊
        // cout << "maybe_head[0][" << go_head << "] = " << maybe_head[0][go_head]
        //      << ", bars[0]["    << go_bar << "] = " << bars[0][go_bar]
        //      << ", value = "     << maybe_head[0][go_head] - bars[0][go_bar];

        rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]), Point(maybe_head[0][go_head] + template_img.cols, maybe_head[1][go_head] +template_img.rows), Scalar(0, 255, 0), 2);
        line( debug_img, Point(bars[0][go_bar], bars[1][go_bar]), Point(bars[0][go_bar], bars[1][go_bar] + bars[2][go_bar]), Scalar(100, 200, 0), 2 );

        // ******************************************
        // ~~~~~~ debug 用來看合併的過程~~~~~~~~~~
        //        imshow("debug", debug_img);
        //        waitKey(0);

        // check有沒有 線(time_bar)

        int distance = maybe_head[0][go_head] - bars[0][go_bar]; // 值代表大小，+-代表方向，-往左，+往右
        // 左邊的離線太遠 不抓線，直接存
        if((maybe_head[0][go_head] - bars[0][go_bar]) < ((template_img.cols + dist_error) * -1) ){
            // cout註解 標記現在在哪個case
            // cout << ", case 1";
            note[0][note_count] =  maybe_head[0][go_head];
            note[1][note_count] =  maybe_head[1][go_head];
            note[2][note_count] =  head_type;
            note[3][note_count] =  0; // 不抓線
            note_count++;
            //note[4][note_count] = ;
        }
        // 和線夠近 抓 同一條 線
        else if( ((maybe_head[0][go_head] - bars[0][go_bar]) >= ((template_img.cols + dist_error) * -1)) &&
                 ((maybe_head[0][go_head] - bars[0][go_bar]) <= (0 + dist_error) ) ){
            // cout註解 標記現在在哪個case
            // cout << ", case 2";

            // 如果是很擠的狀況~~就非常尷尬拉~~要看他是和 此條線比較靠近() 還是和 下一條線比較接近
            if( distance >= 0 && distance <= (0 + dist_error) &&
                bars_dir[0][go_bar] == false &&
                bars_dir[1][go_bar] == true   ){
                if(go_bar >= bars_count-1);  // 如果是在最後一條線出現此case的話，就一定是併排的那種拉，防呆 ~~~do nothing~~~
                else{
                    // cout註解 標記現在在哪個case
                    // cout << endl << "embarass~~case " << endl;

                    int distance_next = maybe_head[0][go_head] - bars[0][go_bar+1];
                    distance_next += template_img.cols;
                    // cout註解 看一下想像中的range對不對
                    // cout << "distance = " << distance  << ", distance_next = " << distance_next;

                    if(abs( distance) < abs(distance_next));// 現在的頭 和 現在這條線比較近，就繼續做case2~~
                    else{ // (abs( distance) >= abs(distance_next)) 現在的頭 和 下一條線比較接近，做case3換下一條線拉！
                    
                        // cout註解 標記現在在哪個case
                        // cout << ", case 3";
                        go_bar++;
                        go_head--;
                        cout << endl;
                        continue;
                    }
                }
            }
            // ***************************************************************************
            note[0][note_count] =  maybe_head[0][go_head];
            note[1][note_count] =  maybe_head[1][go_head];
            note[2][note_count] =  head_type;
            note[3][note_count] =  bars_time[go_bar]; // 抓同條線
            note_count++;
        }
        // 右邊的離線太遠  換 下一條線 ， 這顆頭 換了下一條線 後 需要再跟 這條新的線 比一次，所以head--
        else{ // 即if((maybe_head[0][go_head] - bars[0][go_bar] > (0 + dist_error))
        
            // cout註解 標記現在在哪個case
            // cout << ", case 3";
            go_bar++;
            go_head--;
        }
        // cout << endl;
    }
}
