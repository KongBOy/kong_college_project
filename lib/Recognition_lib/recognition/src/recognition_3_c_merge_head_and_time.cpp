/*
這個程式的功能是：
把符桿的時間長度和符號合併
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include "Bar_tool.h"
#include "recognition_3_c_merge_head_and_time.h"
#include "recognition_0_array_tools.h"

using namespace std;
using namespace cv;

#define DOWNTOTOP 0
#define TOPTODOWN 1

// 水平畫箭頭
void arrowLR(Mat img, Point start_point, Point end_point, Scalar color, int thickness){
    // 箭頭的水平線
    line( img, start_point, end_point, color, thickness );

    // 往左的箭頭, end_point 往右上右下畫斜線
    if      (end_point.x - start_point.x < 0){
        line( img, end_point, Point(end_point.x + 5, end_point.y - 5), color, thickness );
        line( img, end_point, Point(end_point.x + 5, end_point.y + 5), color, thickness );
    }
    // 往右的箭頭, end_point 往左上左下畫斜線
    else if (end_point.x - start_point.x > 0){
        line( img, end_point, Point(end_point.x - 5, end_point.y - 5), color, thickness );
        line( img, end_point, Point(end_point.x - 5, end_point.y + 5), color, thickness );
    }
    // 沒有移動的話, 就原點點大顆一點
    else{
        line( img, start_point, end_point, color, 5 );
    }
}

void recognition_4_merge_head_and_time(int head_type, 
                                       Mat template_img, Mat reduce_line, 
                                       int maybe_head_count, float maybe_head[][200], 
                                       int bars_count, short bars[][200], bool bars_dir[200], int bars_time[200], 
                                       int& note_count, int note[][1000],
                                       bool debuging){
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    // 看一下目前的bar有哪些
    if(debuging) draw_bars(debug_img, bars_count, bars, bars_dir);

    // 會用到的變數先宣告
    int dist_error = 4;
    int go_bar = 0;
    
    int maybe_head_x;
    int maybe_head_y;
    int bar_x;
    int bar_y;
    int bar_len;
    bool bar_dir;
    int vector_x;
    bubbleSort_bars(bars_count, bars, bars_dir, X_INDEX);
    bubbleSort_maybe_head(maybe_head_count, maybe_head, X_INDEX);

    // 走訪每顆頭來找線 來和 bars 從頭開始 比較, 夠近的就抓線 並 換下一顆頭, 此時不一定要換下一條bar喔, 因為可能一條bar有多顆頭, 直到換頭換到 線離 頭的左邊夠遠, 才會換下一條bar
    for(int go_head = 0; go_head < maybe_head_count; go_head++){
        maybe_head_x = maybe_head[0][go_head];
        maybe_head_y = maybe_head[1][go_head];
        bar_x   = bars[0][go_bar];
        bar_y   = bars[1][go_bar];
        bar_len = bars[2][go_bar];
        bar_dir = bars_dir[go_bar];

        // head走到bar 的x向量, 值代表大小，+-代表方向，-往左，+往右
        vector_x = bar_x - maybe_head_x;

        // 看一下處理中的 head, bar, head到bar的vector
        if(debuging){
            // 目前處理的head
            rectangle(debug_img, Point(maybe_head_x, maybe_head_y), Point(maybe_head_x + template_img.cols, maybe_head_y +template_img.rows), Scalar(0, 255, 0), 2);
            // 目前處理的bar
            if     (bar_dir == TOPTODOWN) line( debug_img, Point(bar_x, bar_y), Point(bar_x, bar_y + bar_len), Scalar( 36, 135,  0), 2 );
            else if(bar_dir == DOWNTOTOP) line( debug_img, Point(bar_x, bar_y), Point(bar_x, bar_y - bar_len), Scalar( 36, 135,  0), 2 );
            // 目前head 走到 目前bar 的 向量
            arrowLR(debug_img, Point(maybe_head_x, maybe_head_y), Point(bar_x, maybe_head_y), Scalar(160, 160, 160), 2 );

            // 目前head 走到 目前bar 往左走的合格向量
            arrowLR(debug_img, Point(maybe_head_x, maybe_head_y + template_img.rows ), Point(maybe_head_x + dist_error * -1               , maybe_head_y + template_img.rows ), Scalar( 36, 135,  0), 2 );
            // 目前head 走到 目前bar 往右走的合格向量
            arrowLR(debug_img, Point(maybe_head_x, maybe_head_y + template_img.rows ), Point(maybe_head_x + template_img.cols + dist_error, maybe_head_y + template_img.rows ), Scalar( 36, 135,  0), 2 );
            // 目前適用head的左上角的x 為基準的點
            cv::circle(debug_img, Point(maybe_head_x, maybe_head_y + template_img.rows ), 1, Scalar( 0, 135,  255), 2 );
            
            // 文字顯示
            cout << "maybe_head_x[" << go_head << "]=" << maybe_head_x << ", bar_x[" << go_bar << "]=" << bar_x << ", vector_x=" << vector_x << endl;
            imshow("debug", debug_img);
            cv::waitKey(0);
        }

        // 頭的左邊 往左走太左, 該換下一條線了, 這顆頭 換了下一條線 後 需要再跟 這條新的線 比一次，所以go_head--
        if( vector_x <  dist_error * -1 ){
            if(debuging){
                arrowLR(debug_img, Point(maybe_head_x, maybe_head_y), Point(bar_x, maybe_head_y), Scalar(0, 0, 255), 1 );
                cout << "  case 3 bar too left, change bar" << endl;
            }
            go_bar++;
            go_head--;
        }
        // 頭的左邊 往左走 dist_error ~ 往右走 template_img.cols + dist_error 都算夠近, 抓 同一條 線(go_bar沒有要++喔, 除非符合一些特例 比如 擁擠case)
        else if(  dist_error * -1 <= vector_x && vector_x <= template_img.cols + dist_error  ){
            if(debuging){
                arrowLR(debug_img, Point(maybe_head_x, maybe_head_y), Point(bar_x, maybe_head_y), Scalar(255, 0, 0), 1 );
                rectangle(debug_img, Point(maybe_head_x, maybe_head_y), Point(maybe_head_x + template_img.cols, maybe_head_y +template_img.rows), Scalar(255, 0, 0), 1);
                cout << "  case 2 bar OK" << endl;
            }
            
            // 擁擠case
            // 符桿通常是出現在頭的右上或左下, 如果是很擠的狀況, 就要看他是和 現在條線比較靠近 還是和 下一條線比較接近
            //  |
            //  | 目前在這條bar
            // *  
            //    * 目前在這顆頭, 
            //   | 這條目前是 next_bar, 以此圖例的話是離next_bar較近所以希望抓這條
            //   |
            if( template_img.cols <= vector_x  && vector_x <= template_img.cols + dist_error && bar_dir == DOWNTOTOP ){
                if(go_bar < bars_count-1){  // 防呆, 因為要看下一條線, 所以只能做到倒數第二條
                    int distance_next = bars[0][go_bar+1] - maybe_head_x;
                    if(debuging) cout << "    check crowd, vector_x = " << vector_x  << ", distance_next = " << distance_next;
                    
                    // 現在的頭 和 下一條線比較接近，做case4 next bar colser so choose next 換下一條線拉！
                    if(abs( vector_x) >= abs(distance_next)){   
                        if(debuging) cout << "    case 4 next bar colser so choose next" << endl;
                        go_bar++;
                        go_head--;
                        continue;
                    }

                    // 如果走到這裡代表不是很壅擠沒問題 繼續往下做事
                    if(debuging) cout << ", not crowd OK" << endl;
                }
            }
            // ***************************************************************************
            note[0][note_count] =  maybe_head_x;
            note[1][note_count] =  maybe_head_y;
            note[2][note_count] =  head_type;
            note[3][note_count] =  bars_time[go_bar]; // 抓同條線
            note_count++;
        }
        // 頭的左邊 往右走太右, 不抓線 且 這顆頭忽略不儲存 直接continue到下顆頭
        else{
            if(debuging){
                cout << "  case 1 bar too left" << endl;
                arrowLR(debug_img, Point(maybe_head_x, maybe_head_y), Point(bar_x, maybe_head_y), Scalar(0, 255, 0), 1 );
            }
            // 在 maybe_head 和 bar 都已經對x排序了以後,
            // 目前的機制是 走訪所有的頭 來找線, 換下顆頭時 對下顆頭來說 目前的bar通常會變左或右一點點(一個bar有多顆音) 或 直接變太左, 太左多就會換下條bar, 所以不會有變bar太右太多的機會
            // 所以如果太右了 唯一可能就是發生在 最一開始 第一顆頭 遇到 第一條bar 太右
            // 如果 第一條bar 還在 頭 的 太右邊, 代表這顆頭怪怪的, 所以直接 continue 掉 不存這顆頭 覺得比較合理
            continue;
        }
        imshow("debug", debug_img);
        waitKey(0);
    }
}
