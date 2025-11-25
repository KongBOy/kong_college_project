/*
這個程式的功能是：
把 recognition_1 找到的 可能是的符號 依據 輸入符號的特徵做篩選
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "Recognition.h"
#include "recognition_0_array_tools.h"
#include "Maybe_head_tool.h"

using namespace cv;
using namespace std;

float black_count_function(Mat image, int top, int down, int left, int right, Mat& debug_img){ // 都是 array index
    int black_count = 0;
    for(int go_row = top ; go_row <= down  ; go_row++){
        for(int go_col = left ; go_col <= right ; go_col++){
            // 防呆
            if(go_row > image.rows - 1 || go_row < 0 || 
               go_col > image.cols - 1 || go_col < 0) continue;

            if(!image.at<uchar>(go_row, go_col)){
                black_count++;
                // circle(debug_img, Point(go_col,go_row), 1, Scalar(0, 0, 255), 1);  // 黑色的地方 標 紅色
            }
        }
    }

    float test_area = (down - top + 1) * (right - left + 1);
    float black_rate = black_count / test_area;
    // cout << "test_area=" << test_area << ", black_count=" << black_count << ", black_rate=" << black_rate << endl;
    return black_rate;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Recognition_staff_img::recognition_2_a_head_charactristic(int head_type, Mat template_img){
    debuging = debuging_recog2;
    Mat debug_img;
    cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);
    // 用明顯特徵過濾以前 先顯示一下目前結果, 等等做完後會再顯示一次可以來比較
    if(debuging){
        cv::imshow  ("template_img", template_img);
        cvMoveWindow("template_img", 10, 10);

        MaybeHead_draw_w_color(debug_img, template_img, maybe_head_count, maybe_head);
        imshow("before", debug_img);
        cvMoveWindow("before", 10, 80);
        waitKey(0);

        // 怕太亂 重新整理debug_img 回 staff_img_erase_line
        cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);
    }

    // 把會用到的變數先宣告出來
    int maybe_head_x;
    int maybe_head_y;
    int maybe_head_confidence;
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
        maybe_head_x = maybe_head[0][go_head];
        maybe_head_y = maybe_head[1][go_head];
        maybe_head_confidence = maybe_head[2][go_head];
        if(debuging) cout << "mayby_head_x=" << maybe_head_x << ", maybe_head_y=" << maybe_head_y << ", maybe_head_confidence = " << maybe_head_confidence << endl;
        switch(head_type){
            // 二分音符, 四分音符, 偵測頭的中心是否為實心的, 實心就代表為四分音符->排除二分音符, 空心就代表為二分音符->排除四分音符
            case 2:
            case 4:{
                // 檢查 以頭為中心往周圍算 check_size 格, 此區域內為實心還是空心
                int check_size = 6;
            
                // 實心位置 的左上角: 取 頭的正中心 往左 check_size / 2 
                Point check_left_top(maybe_head_x + 0.5*template_img.cols - check_size/2 ,
                                     maybe_head_y + 0.5*template_img.rows - check_size/2 );
                // debug用, 看一下目前框待檢查的實心框框在哪邊
                if(debuging){
                    
                    rectangle( debug_img, Point(check_left_top.x,check_left_top.y), Point(check_left_top.x + check_size,check_left_top.y + check_size), Scalar(255,200,100), 1, 8, 0 );
                    imshow("debug_img", debug_img);
                    cvMoveWindow("debug_img", 10, 250);
                }
                
                // 走遍框框的所有pixel, 如果遇到白色就代表空心, 如果沒遇到白色就代表實心
                bool empty_inside = false;
                for(int go_y = check_left_top.y ; go_y < check_left_top.y + check_size ; go_y++){
                    for(int go_x = check_left_top.x ; go_x < check_left_top.x + check_size ; go_x++){
                        if(staff_img_erase_line.at<uchar>(go_y,go_x)){
                            empty_inside = true;
                            
                            if(debuging) circle(debug_img, Point(go_x, go_y), 1, Scalar(0, 0, 255), 3); // 找到的白色位置 標 紅色
                            // 如果偵測到非實心, 代表是二分音符頭, 所以如果現在是在偵測四分音符的話就可以刪除此頭了
                            if(head_type == 4){
                                position_erase(maybe_head_count,maybe_head,go_head);
                                go_head--;
                            }
                            else if(head_type == 2);  //  do_nothing
                        }
                        // 如果目前row的某個col 已經偵測到非實心, 不用再繼續檢查是否為實心囉
                        if(empty_inside) break;  //  兩層都要加！
                    }
                    // 如果目前row 已經偵測到非實心, 不用再繼續檢查是否為實心囉
                    if(empty_inside) break;  //  兩層都要加！
                }
                // 小心如果要show的話需要 if(empty_inside == false) 要不然頭剛被刪掉的話 go_head會 == -1，讀資料會gg

                if(head_type == 2){
                    // 如果是空心的, 代表是二分音符頭
                    if(empty_inside == true);  // do_nothing
                    // 如果是實心的, 代表是四分音符頭, 所以如果現在是在偵測二分音符的話就可以刪除此頭了
                    else{
                        position_erase(maybe_head_count, maybe_head, go_head);
                        go_head--;
                    }
                }
                if(debuging) cv::waitKey(0);
            }
            break;

            // 全休止, 二分休止, 補充一下後來發現 本身已經找得更好了, 就不用明顯特徵來過濾囉
            case 1:   // 全休止   在未消線的圖中 以自身往外延伸 的左上 且 右上 都有黑色線, 如果此特徵不夠多納就排除 是 全休止
            case 3:{  // 二分休止 在未消線的圖中 以自身往外延伸 的左下 且 右下 都有黑色線, 如果此特徵不夠多納就排除 是 二分休止
                int shift = 2;        // 靠近自身偏移多少
                int check_width = 6;  // 往外搜尋多少
                int l_right = maybe_head[0][go_head] -1 + shift;
                int l_left  = l_right - check_width;

                int r_left  = maybe_head[0][go_head] + template_img.cols + 1 -shift;
                int r_right = r_left + check_width;

                int l_top = 0;
                int l_down = 0;

                if(head_type == 1){
                    l_top  = maybe_head_y;  // -shift;
                    l_down = l_top + check_width;

                }
                else if(head_type == 3){
                    l_down = maybe_head_y + template_img.rows;  //  +shift;
                    l_top = l_down - check_width;
                }
                int r_top  = l_top;
                int r_down = l_down;

                if(debuging){
                    rectangle( debug_img, Point(l_left, l_top), Point(l_right, l_down), Scalar(  0, 255, 0), 1, 8, 0 );
                    rectangle( debug_img, Point(r_left, r_top), Point(r_right, r_down), Scalar(255,   0, 0), 1, 8, 0 );
                    rectangle( debug_img, Point(maybe_head_x, maybe_head_y), Point(maybe_head_x + template_img.cols, maybe_head_y + template_img.rows), Scalar(0, 0,255), 1, 8, 0 );
                    imshow("debug_img", debug_img);
                    cvMoveWindow("debug_img", 10, 250);
                }

                float l_black_rate = black_count_function(staff_img, l_top, l_down, l_left, l_right, debug_img);
                float r_black_rate = black_count_function(staff_img, r_top, r_down, r_left, r_right, debug_img);

                if(debuging) cout << "l_black_rate = " << l_black_rate << " , r_black_rate = " << r_black_rate << endl;
                // 如果自身往外的黑色線不夠多, 那就代表不是 全休止 或 二分休止, 就把它排除掉囉
                if(l_black_rate < 0.1 || r_black_rate < 0.1){
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
            }
            break;


            // 十六分休止, 三十二分休止, 八分休止 的 右邊偏上 和 左邊偏下 會是空白沒東西的, 有東西的話就排除
            // 主要是為了防止
            //    十六分  休止 下半部的 八分休止 被重複找到, 
            //    三十二分休止 下半部的 十六分休止, 八分休止 被重複找到
            case 6:
            case 8:{
                int check_width = 3;
                int ru_right = maybe_head_x + template_img.cols ;
                int ru_left  = ru_right - template_img.cols / 2;
                int ru_down  = maybe_head_y - 4;
                int ru_top   = ru_down - check_width;

                int ld_left  = maybe_head_x;
                int ld_right = ld_left + template_img.cols / 2 ;
                int ld_top   = maybe_head_y + template_img.rows;
                int ld_down  = ld_top + check_width;
                
                float ru_black_rate = black_count_function(staff_img_erase_line, ru_top, ru_down, ru_left, ru_right, debug_img);
                float ld_black_rate = black_count_function(staff_img_erase_line, ld_top, ld_down, ld_left, ld_right, debug_img);
                
                // 畫一下 框框框在哪裡
                if(debuging){
                    rectangle( debug_img, Point(ru_left, ru_top), Point(ru_right, ru_down), Scalar(255,200,100), 1, 8, 0 );
                    rectangle( debug_img, Point(ld_left, ld_top), Point(ld_right, ld_down), Scalar(255,200,100), 1, 8, 0 );
                    rectangle( debug_img, Point(maybe_head_x, maybe_head_y), Point(maybe_head_x + template_img.cols, maybe_head_y + template_img.rows), Scalar(0, 0,255), 1, 8, 0 );
                    imshow("debug_img", debug_img);
                    cvMoveWindow("debug_img", 10, 250);
                    cv::waitKey(0);
                }

                if(ru_black_rate > 0.25 || ld_black_rate > 0.25){ // 24~32格, 裡面有5~9格以上都是黑色的就太多囉
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
            }
            break;
        }
    }
    // 用明顯特徵過濾以後 也先顯示一下結果
    if(debuging){
        MaybeHead_draw_w_color(debug_img, template_img, maybe_head_count, maybe_head);
        imshow("after", debug_img);
        cvMoveWindow("after", 10, 450);
        waitKey(0);
        cv::destroyAllWindows();
    }
}
