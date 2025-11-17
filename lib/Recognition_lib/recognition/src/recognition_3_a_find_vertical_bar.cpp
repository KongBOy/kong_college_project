/*
這個程式的功能是：
找出符號的符桿
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Recognition.h"
#include "recognition_0_array_tools.h"
#include "Bar_tool.h"
#include "Note_infos.h"
#include "recognition_3_a_find_vertical_bar.h"

#define DOWNTOTOP 0
#define TOPTODOWN 1

using namespace cv;
using namespace std;


int find_vertical_bar(Mat reduce_line, int left, int right, int floor_stand, int test_depth, bool direction, Mat& debug, bool debuging){
    // left, right: 要搜尋的 寬度
    // test_depth : 要搜尋的 高度
    // floor_stand, direction:
    //     方向是 DOWNTOTOP 的話, 代表從 floor_go 從 floor_stand 開始, 搜尋 left 到 right 的範圍內 如果有遇到黑色點, 就移動 floor_go 往上移動一格, 共測試 test_depth 格深度, 最後停的點就是 bar 的 top  , 所以 用 DOWNTOTOP 時 floor_stand 建議要設定在 head 的左上角 和 右上角 的 y座標
    //     方向是 TOPTODOWN 的話, 代表從 floor_go 從 floor_stand 開始, 搜尋 left 到 right 的範圍內 如果有遇到黑色點, 就移動 floor_go 往下移動一格, 共測試 test_depth 格深度, 最後停的點就是 bar 的 down , 所以 用 TOPTODOWN 時 floor_stand 建議要設定在 head 的左下角 和 右下角 的 y座標
    int floor_go       = floor_stand;  // 從固定的底 走向 TOP(DOWNTOTOP) 或 DOWN(TOPTODOWN)

    int white_coda_acc = 0; 
    int white_coda_ok  = 3;
    
    int one_step;
    if      (direction == DOWNTOTOP) one_step = -1;  // 往上找, -1
    else if (direction == TOPTODOWN) one_step = +1;  // 往下找, +1
    
    int  searching_row;         // 目前正在搜尋的row
    bool searching_have_black;  // 目前正在搜尋的row 有沒有找到 黑色
    for(int go_depth = 0 ; go_depth < test_depth ; go_depth++){
        searching_have_black = false;                              // 還沒開搜 當然沒有黑色, 初始化設 false
        searching_row        = floor_stand + go_depth * one_step;  // 根據方向 定位 目前要搜尋的row
        // 防呆
        if( searching_row < 0 || searching_row > reduce_line.rows-1 ) break;
        // 走訪此row 的 left ~ right 的所有pixel
        for(int go_width = left ; go_width <= right ; go_width++){
            // 防呆
            if( go_width > reduce_line.cols-1) break;

            // 如果此row 一碰到 黑色
            if(reduce_line.at<uchar>(searching_row, go_width) == 0){
                if(debuging) circle( debug, Point(go_width, floor_go), 1, Scalar(0, 22, 163), 1);  // 深咖啡
                searching_have_black = true;   // 標記此row有黑色
                floor_go = searching_row;      // floor 更新 
                white_coda_acc = 0;            // white_coda_acc 重新計算
                break;                         // break直接換下個row
            }
        }

        // 如果走到這裡, 一種可能是 此row有找到黑色跳出迴圈(searching_have_black==true), OK正常繼續下一個row, 
        // 另一種可能是 此row跑完for迴圈 沒碰到任一點是黑色的(searching_have_black==false), 可能到頭了 也可能 中間因為畫質差黑色小斷掉

        // 跑完for迴圈 沒碰到任一點是黑色的, 就代表此row全白, 白色容忍值++, 一方面怕原始影像品質差中間有斷掉要留些coda跳過空白, 另一方面也是找到頭的線索
        if( searching_have_black == false){
            if(debuging){
                cout<<"now white_coda_acc = "<< white_coda_acc <<endl;
                line( debug, Point(left, searching_row), Point(right, searching_row), Scalar(204, 255, 0), 1, CV_AA);  // 蔚藍
            }
            white_coda_acc++;
        }

        // 每跑一個row就看一次結果 的話就註解拿掉
        // if(debuging){
        //     cv::imshow("searching vertical bar", debug);
        //     cv::waitKey(0);
        // }
        
        // 連續出現 white_coda_ok 個row 是全白的, 就代表找到頭了
        if(white_coda_acc > white_coda_ok) break;
    }

    // 走道這邊都應該要被找到囉！
    if(debuging){
        line( debug, Point(left, floor_go), Point(right, floor_go), Scalar(0, 0, 255), 2);  // 紅色
        cv::imshow("searching vertical bar", debug);
        cv::waitKey(0);
    }

    return floor_go;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Recognition_staff_img::recognition_3_a_find_vertical_bar(Mat template_img){
    debuging = debuging_recog3a;
    // 需要找線的原因 是 需要找 符桿 來判斷 8, 16, 32, ... 音符
    Mat debug_img = staff_img_erase_line.clone();
    cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);

    // check bar 框框 範圍設定
    int test_depth = 12 * template_img.rows;        // 大概三個八度左右囉~~~
    int test_width = 11;                            // 用眼睛看 11 不錯
    int test_area = test_depth * test_width * 0.1;
    float test_ok_depth = 1.0 * template_img.rows;

    // 每顆頭在 左上 右上 左下 右下 都找找看線
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
        // 定出要找的範圍 右上
        int i_feel_top_r_shift = -2;
        int top_right_r  = maybe_head[0][go_head] + template_img.cols + test_width/2 + i_feel_top_r_shift;  // 框框的右邊
        int top_right_l  = top_right_r - test_width;    // 總共想測 test_width 格
        int top_right_d  = maybe_head[1][go_head] -1;   // 框框的上一格
        if(debuging){
            cout<<"top_r_distance = "<<top_right_r - top_right_l <<endl;
            rectangle( debug_img, Point(top_right_l , top_right_d -test_depth), Point( top_right_r , top_right_d  ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 左上
        int i_feel_top_l_shift = +2;
        int top_left_l   = maybe_head[0][go_head] - test_width/2 + i_feel_top_l_shift;//框框的右邊
        int top_left_r   = top_left_l + test_width ;   // 總共想測 test_width 格
        int top_left_d   = maybe_head[1][go_head] -1;  // 框框的上一格
        if(debuging){
            cout<<"top_l_distance = "<<top_left_r - top_left_l  <<endl;
            rectangle( debug_img, Point(top_left_l  , top_right_d -test_depth), Point( top_left_r  , top_left_d   ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 右下
        int i_feel_down_l_shift = +2;
        int down_left_l  = maybe_head[0][go_head] - test_width/2 + i_feel_down_l_shift; // 框框的左邊
        int down_left_r  = down_left_l + test_width ;                      // 總共想測 test_width 格
        int down_left_t  = maybe_head[1][go_head] + template_img.rows +1;  // 框框的下
        if(debuging){
            cout<<"down_l_distance = "<<down_left_r - down_left_l <<endl;
            rectangle( debug_img, Point(down_left_l , down_left_t ), Point( down_left_r , down_left_t +test_depth ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 左下
        int i_feel_down_r_shift = -2;
        int down_right_l = maybe_head[0][go_head] + template_img.cols - test_width/2 + i_feel_down_r_shift;  // 框框的左邊
        int down_right_r = down_right_l + test_width ;                     // 總共想測 test_width 格
        int down_right_t = maybe_head[1][go_head] + template_img.rows +1;  // 框框的下
        if(debuging){
            cout<<"down_r_distance = "<<down_right_r - down_right_l<<endl;
            rectangle( debug_img, Point(down_right_l, down_right_t), Point( down_right_r, down_left_t+test_depth ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 看一下圈找線的範圍對不對
        if(debuging){
            cv::imshow("see 4 corner", debug_img);
            cv::waitKey(0);
        }
        
        // 在圈定的範圍內找bar
        int top_right_t  = find_vertical_bar(staff_img_erase_line, top_right_l , top_right_r , top_right_d , test_depth, DOWNTOTOP, debug_img, debuging);  // 右上
        int top_left_t   = find_vertical_bar(staff_img_erase_line, top_left_l  , top_left_r  , top_left_d  , test_depth, DOWNTOTOP, debug_img, debuging);  // 左上
        int down_right_d = find_vertical_bar(staff_img_erase_line, down_right_l, down_right_r, down_right_t, test_depth, TOPTODOWN, debug_img, debuging);  // 右下
        int down_left_d  = find_vertical_bar(staff_img_erase_line, down_left_l , down_left_r , down_left_t , test_depth, TOPTODOWN, debug_img, debuging);  // 左下
        // 看一下找線的結果
        if(debuging){
            cv::imshow("see 4 corner", debug_img);
            cv::waitKey(0);
        }
        
        // 此顆頭 往右上, 左上, 右下, 左下 找到的線的長度
        int top_right_length  = top_right_d  - top_right_t ;
        int top_left_length   = top_left_d   - top_left_t  ;
        int down_right_length = down_right_d - down_right_t;
        int down_left_length  = down_left_d  - down_left_t ;
        if(debuging){
            cout <<  "head.x = "    << maybe_head[0][go_head]
                <<", head.y = "    << maybe_head[1][go_head]
                <<", top_line = "  << top_left_length <<", " << top_right_length
                <<", down_line = " << down_left_length<<", " << down_right_length
                <<endl;
        }


        // 不一定所有的線都要存喔, 存右上 + 左下的就好了其實
        // 不用擔心一些特例比如
        //  |
        //  |
        // *|  右下有線, 右上有線
        //  |
        //  |* 左上有線, 左下有線
        // *   右上有線, 只會保留這條最長最主要的線
        // 因為線存在的條件是 一邊頂點是頭 另一邊頂點是符桿 才會形成bar
        // 所以 頭 到 符桿 之間連成的線 就會是最長的線,
        // 因為我們找 bar 的主要目的是為了 找 符桿,
        // 所以 如果遇到多條線, 存 最長的那條就對了, 存中間的短線 無法幫助我們找到符桿,
        // 在下面我會有合併線的機制 如圖這種左上的線會被合併掉, 所以乾脆一開始就不用找, 右下也同理
        // 多測左上 + 右下只是想順便 刪除一些不合格的頭而已,
        // 會呼叫 找bar的話(二分, 四分的頭), 代表期望他四個角其中一定會有線存在, 如果四角都找不到線, 就代表這顆頭有問題要刪掉喔

        //  檢查線的長度，夠長就存起來
        if( down_left_length >= test_ok_depth){
            // head 左下的頂點 的 根部
            bars[0][bars_count] = maybe_head[0][go_head];       // bar連接note的.x
            bars[1][bars_count] = down_left_t ;                 // bar連接note的.y
            bars[2][bars_count] = down_left_d - down_left_t;    // bar長度
            bars_dir[bars_count] = true;                        // bar方向 [0]左(下, TOPTODOWN)
            bars_count++;
        }
        if( top_right_length >= test_ok_depth){
            // head 右上的頂點 的 根部
            bars[0][bars_count] = maybe_head[0][go_head]+template_img.cols-2;  // bar連接note的.x, -2是用眼睛看的誤差修正
            bars[1][bars_count] = top_right_d ;                                // bar連接note的.y
            bars[2][bars_count] = top_right_d - top_right_t ;                  // bar長度
            bars_dir[bars_count] = false;                                      // bar方向 [1]右(上, DOWNTOTOP)
            bars_count++;
        }

        // 如果這顆頭"四角"都找不到線的話 代表這顆頭有問題要刪掉喔
        if( ( top_right_length  < test_ok_depth ) && ( top_left_length  < test_ok_depth ) &&
            ( down_right_length < test_ok_depth ) && ( down_left_length < test_ok_depth ) ){
            position_erase(maybe_head_count, maybe_head, go_head);
            go_head--;
        }
    }

    // 看一下 找到的 bars before merge
    if(debuging){
        cv::cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);  // 重製一下debug圖要不然話太多東西太亂了
        draw_bars(debug_img, bars_count, bars, bars_dir);
        cv::imshow("bars result before merge", debug_img);
        cv::waitKey(0);
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 合併線長度, 方向
    //  |
    //  |
    // *|  右上
    //  |  
    //  |* 左下
    // *   右上 三條線是同一條線
    // 特性是 排序後, x 離很近
    bubbleSort_bars(bars_count, bars, bars_dir, BAR_LENGTH_INDEX);
    bubbleSort_bars(bars_count, bars, bars_dir, Y_INDEX);
    bubbleSort_bars(bars_count, bars, bars_dir, X_INDEX);
    for(int go_bar = 0 ; go_bar < bars_count -1 ; go_bar++){
        int next = go_bar + 1;
        int curr_x   = bars[0] [go_bar];
        int curr_y   = bars[1] [go_bar];
        int curr_len = bars[2] [go_bar];
        int curr_dir = bars_dir[go_bar];

        int next_x   = bars[0] [next];
        int next_y   = bars[1] [next];
        int next_len = bars[2] [next];
        int next_dir = bars_dir[next];
        
        // merge bar 的過程
        if(debuging){
            Mat debug_img2;
            cvtColor(staff_img_erase_line, debug_img2, CV_GRAY2BGR);
            cout << "now , x = " << curr_x << ", y = " << curr_y << ", length = " << curr_len << ", tail = " << curr_y + curr_len << endl;
            if      (curr_dir == TOPTODOWN) line(debug_img2, Point(curr_x, curr_y),  Point(curr_x, curr_y + curr_len), Scalar(0, 255, 0), 2);
            else if (curr_dir == DOWNTOTOP) line(debug_img2, Point(curr_x, curr_y),  Point(curr_x, curr_y - curr_len), Scalar(0, 255, 0), 2);
            if      (next_dir == TOPTODOWN) line(debug_img2, Point(next_x, next_y),  Point(next_x, next_y + next_len), Scalar(0, 0, 255), 2);
            else if (next_dir == DOWNTOTOP) line(debug_img2, Point(next_x, next_y),  Point(next_x, next_y - next_len), Scalar(0, 0, 255), 2);
            imshow("bar now green and next red", debug_img2);
            waitKey(0);
        }
        
        int distance_x = abs( next_x - curr_x );
        // cout<<"distance_x = "<<distance_x<<endl;
        // 如果 x 離的夠近, x座標取平均, y座標,長度,方向 就挑長的存
        if( distance_x  <= template_img.cols / 2){
            if(debuging){
                cout<<"case merge_line, bars_count = "<<bars_count<<endl;
                cout << "now , x = " << curr_x << ", y = "<< curr_y << ", length = "<< curr_len << ", dir" << curr_dir << endl;
                cout << "next, x = " << next_x << ", y = "<< next_y << ", length = "<< next_len << ", dir" << next_dir << endl;
            }

            // x座標取平均
            bars[0][go_bar] = (bars[0][next] + bars[0][go_bar])/2 ;  //x存中間
            // y座標,長度,方向 就挑長的存
            if(curr_len <= next_len){
                bars[1][go_bar] = next_y;     // 存長的 y
                bars[2][go_bar] = next_len;   // 存長的 len
                bars_dir[go_bar] = next_dir;  // 存長的 dir
            }
            // 被合併的線就刪掉囉
            position_erase_bar(bars_count, bars, bars_dir, next);
            go_bar--;
        }
    }

    // 看一下 找到的 bars after merge
    if(debuging){
        cv::cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);  // 重製一下debug圖要不然話太多東西太亂了
        draw_bars(debug_img, bars_count, bars, bars_dir);
        cv::imshow("bars result after merge", debug_img);
        cv::waitKey(0);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void recognition_3_a_find_vertical_bar(Mat template_img, Mat reduce_line, 
                                       int& maybe_head_count, float maybe_head[][200], 
                                       int& bars_count, short bars[][200], bool bars_dir[200],
                                       bool debuging){
    // 需要找線的原因 是 需要找 符桿 來判斷 8, 16, 32, ... 音符
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    // check bar 框框 範圍設定
    int test_depth = 12 * template_img.rows;        // 大概三個八度左右囉~~~
    int test_width = 11;                            // 用眼睛看 11 不錯
    int test_area = test_depth * test_width * 0.1;
    float test_ok_depth = 1.0 * template_img.rows;

    // 每顆頭在 左上 右上 左下 右下 都找找看線
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
        // 定出要找的範圍 右上
        int i_feel_top_r_shift = -2;
        int top_right_r  = maybe_head[0][go_head] + template_img.cols + test_width/2 + i_feel_top_r_shift;  // 框框的右邊
        int top_right_l  = top_right_r - test_width;    // 總共想測 test_width 格
        int top_right_d  = maybe_head[1][go_head] -1;   // 框框的上一格
        if(debuging){
            cout<<"top_r_distance = "<<top_right_r - top_right_l <<endl;
            rectangle( debug_img, Point(top_right_l , top_right_d -test_depth), Point( top_right_r , top_right_d  ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 左上
        int i_feel_top_l_shift = +2;
        int top_left_l   = maybe_head[0][go_head] - test_width/2 + i_feel_top_l_shift;//框框的右邊
        int top_left_r   = top_left_l + test_width ;   // 總共想測 test_width 格
        int top_left_d   = maybe_head[1][go_head] -1;  // 框框的上一格
        if(debuging){
            cout<<"top_l_distance = "<<top_left_r - top_left_l  <<endl;
            rectangle( debug_img, Point(top_left_l  , top_right_d -test_depth), Point( top_left_r  , top_left_d   ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 右下
        int i_feel_down_l_shift = +2;
        int down_left_l  = maybe_head[0][go_head] - test_width/2 + i_feel_down_l_shift; // 框框的左邊
        int down_left_r  = down_left_l + test_width ;                      // 總共想測 test_width 格
        int down_left_t  = maybe_head[1][go_head] + template_img.rows +1;  // 框框的下
        if(debuging){
            cout<<"down_l_distance = "<<down_left_r - down_left_l <<endl;
            rectangle( debug_img, Point(down_left_l , down_left_t ), Point( down_left_r , down_left_t +test_depth ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 定出要找的範圍 左下
        int i_feel_down_r_shift = -2;
        int down_right_l = maybe_head[0][go_head] + template_img.cols - test_width/2 + i_feel_down_r_shift;  // 框框的左邊
        int down_right_r = down_right_l + test_width ;                     // 總共想測 test_width 格
        int down_right_t = maybe_head[1][go_head] + template_img.rows +1;  // 框框的下
        if(debuging){
            cout<<"down_r_distance = "<<down_right_r - down_right_l<<endl;
            rectangle( debug_img, Point(down_right_l, down_right_t), Point( down_right_r, down_left_t+test_depth ), Scalar(200, 155, 255), 1, 8, 0 );  // 粉紅色
        }

        // 看一下圈找線的範圍對不對
        if(debuging){
            cv::imshow("see 4 corner", debug_img);
            cv::waitKey(0);
        }
        
        // 在圈定的範圍內找bar
        int top_right_t  = find_vertical_bar(reduce_line, top_right_l , top_right_r , top_right_d , test_depth, DOWNTOTOP, debug_img, debuging);  // 右上
        int top_left_t   = find_vertical_bar(reduce_line, top_left_l  , top_left_r  , top_left_d  , test_depth, DOWNTOTOP, debug_img, debuging);  // 左上
        int down_right_d = find_vertical_bar(reduce_line, down_right_l, down_right_r, down_right_t, test_depth, TOPTODOWN, debug_img, debuging);  // 右下
        int down_left_d  = find_vertical_bar(reduce_line, down_left_l , down_left_r , down_left_t , test_depth, TOPTODOWN, debug_img, debuging);  // 左下
        // 看一下找線的結果
        if(debuging){
            cv::imshow("see 4 corner", debug_img);
            cv::waitKey(0);
        }
        
        // 此顆頭 往右上, 左上, 右下, 左下 找到的線的長度
        int top_right_length  = top_right_d  - top_right_t ;
        int top_left_length   = top_left_d   - top_left_t  ;
        int down_right_length = down_right_d - down_right_t;
        int down_left_length  = down_left_d  - down_left_t ;
        if(debuging){
            cout <<  "head.x = "    << maybe_head[0][go_head]
                <<", head.y = "    << maybe_head[1][go_head]
                <<", top_line = "  << top_left_length <<", " << top_right_length
                <<", down_line = " << down_left_length<<", " << down_right_length
                <<endl;
        }


        // 不一定所有的線都要存喔, 存右上 + 左下的就好了其實
        // 不用擔心一些特例比如
        //  |
        //  |
        // *|  右下有線, 右上有線
        //  |
        //  |* 左上有線, 左下有線
        // *   右上有線, 只會保留這條最長最主要的線
        // 因為線存在的條件是 一邊頂點是頭 另一邊頂點是符桿 才會形成bar
        // 所以 頭 到 符桿 之間連成的線 就會是最長的線,
        // 因為我們找 bar 的主要目的是為了 找 符桿,
        // 所以 如果遇到多條線, 存 最長的那條就對了, 存中間的短線 無法幫助我們找到符桿,
        // 在下面我會有合併線的機制 如圖這種左上的線會被合併掉, 所以乾脆一開始就不用找, 右下也同理
        // 多測左上 + 右下只是想順便 刪除一些不合格的頭而已,
        // 會呼叫 找bar的話(二分, 四分的頭), 代表期望他四個角其中一定會有線存在, 如果四角都找不到線, 就代表這顆頭有問題要刪掉喔

        //  檢查線的長度，夠長就存起來
        if( down_left_length >= test_ok_depth){
            // head 左下的頂點 的 根部
            bars[0][bars_count] = maybe_head[0][go_head];       // bar連接note的.x
            bars[1][bars_count] = down_left_t ;                 // bar連接note的.y
            bars[2][bars_count] = down_left_d - down_left_t;    // bar長度
            bars_dir[bars_count] = true;                        // bar方向 [0]左(下, TOPTODOWN)
            bars_count++;
        }
        if( top_right_length >= test_ok_depth){
            // head 右上的頂點 的 根部
            bars[0][bars_count] = maybe_head[0][go_head]+template_img.cols-2;  // bar連接note的.x, -2是用眼睛看的誤差修正
            bars[1][bars_count] = top_right_d ;                                // bar連接note的.y
            bars[2][bars_count] = top_right_d - top_right_t ;                  // bar長度
            bars_dir[bars_count] = false;                                      // bar方向 [1]右(上, DOWNTOTOP)
            bars_count++;
        }

        // 如果這顆頭"四角"都找不到線的話 代表這顆頭有問題要刪掉喔
        if( ( top_right_length  < test_ok_depth ) && ( top_left_length  < test_ok_depth ) &&
            ( down_right_length < test_ok_depth ) && ( down_left_length < test_ok_depth ) ){
            position_erase(maybe_head_count, maybe_head, go_head);
            go_head--;
        }
    }

    // 看一下 找到的 bars before merge
    if(debuging){
        cv::cvtColor(reduce_line, debug_img, CV_GRAY2BGR);  // 重製一下debug圖要不然話太多東西太亂了
        draw_bars(debug_img, bars_count, bars, bars_dir);
        cv::imshow("bars result before merge", debug_img);
        cv::waitKey(0);
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 合併線長度, 方向
    //  |
    //  |
    // *|  右上
    //  |  
    //  |* 左下
    // *   右上 三條線是同一條線
    // 特性是 排序後, x 離很近
    bubbleSort_bars(bars_count, bars, bars_dir, BAR_LENGTH_INDEX);
    bubbleSort_bars(bars_count, bars, bars_dir, Y_INDEX);
    bubbleSort_bars(bars_count, bars, bars_dir, X_INDEX);
    for(int go_bar = 0 ; go_bar < bars_count -1 ; go_bar++){
        int next = go_bar + 1;
        int curr_x   = bars[0] [go_bar];
        int curr_y   = bars[1] [go_bar];
        int curr_len = bars[2] [go_bar];
        int curr_dir = bars_dir[go_bar];

        int next_x   = bars[0] [next];
        int next_y   = bars[1] [next];
        int next_len = bars[2] [next];
        int next_dir = bars_dir[next];
        
        // merge bar 的過程
        if(debuging){
            Mat debug_img2;
            cvtColor(reduce_line, debug_img2, CV_GRAY2BGR);
            cout << "now , x = " << curr_x << ", y = " << curr_y << ", length = " << curr_len << ", tail = " << curr_y + curr_len << endl;
            if      (curr_dir == TOPTODOWN) line(debug_img2, Point(curr_x, curr_y),  Point(curr_x, curr_y + curr_len), Scalar(0, 255, 0), 2);
            else if (curr_dir == DOWNTOTOP) line(debug_img2, Point(curr_x, curr_y),  Point(curr_x, curr_y - curr_len), Scalar(0, 255, 0), 2);
            if      (next_dir == TOPTODOWN) line(debug_img2, Point(next_x, next_y),  Point(next_x, next_y + next_len), Scalar(0, 0, 255), 2);
            else if (next_dir == DOWNTOTOP) line(debug_img2, Point(next_x, next_y),  Point(next_x, next_y - next_len), Scalar(0, 0, 255), 2);
            imshow("bar now green and next red", debug_img2);
            waitKey(0);
        }
        
        int distance_x = abs( next_x - curr_x );
        // cout<<"distance_x = "<<distance_x<<endl;
        // 如果 x 離的夠近, x座標取平均, y座標,長度,方向 就挑長的存
        if( distance_x  <= template_img.cols / 2){
            if(debuging){
                cout<<"case merge_line, bars_count = "<<bars_count<<endl;
                cout << "now , x = " << curr_x << ", y = "<< curr_y << ", length = "<< curr_len << ", dir" << curr_dir << endl;
                cout << "next, x = " << next_x << ", y = "<< next_y << ", length = "<< next_len << ", dir" << next_dir << endl;
            }

            // x座標取平均
            bars[0][go_bar] = (bars[0][next] + bars[0][go_bar])/2 ;  //x存中間
            // y座標,長度,方向 就挑長的存
            if(curr_len <= next_len){
                bars[1][go_bar] = next_y;     // 存長的 y
                bars[2][go_bar] = next_len;   // 存長的 len
                bars_dir[go_bar] = next_dir;  // 存長的 dir
            }
            // 被合併的線就刪掉囉
            position_erase_bar(bars_count, bars, bars_dir, next);
            go_bar--;
        }
    }

    // 看一下 找到的 bars after merge
    if(debuging){
        cv::cvtColor(reduce_line, debug_img, CV_GRAY2BGR);  // 重製一下debug圖要不然話太多東西太亂了
        draw_bars(debug_img, bars_count, bars, bars_dir);
        cv::imshow("bars result after merge", debug_img);
        cv::waitKey(0);
    }
}
