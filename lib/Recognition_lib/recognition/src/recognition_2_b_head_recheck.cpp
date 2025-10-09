/*
這個程式的功能是：
把 recognition_1 找到的 可能是的符號 依據 自己寫的 match function 做第二次篩選
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#include "recognition_0_array_tools.h"
#include "Maybe_head_tool.h"
#include "recognition_2_b_head_recheck.h"

#include "string_tools.h"
#include <iomanip>


static void matchTemplate2(Mat src_img, Mat template_test, Mat& result){
    // result部分 模仿opencv 無論如何都重建一個新的 不會用外面傳進來原本的Mat
    int recheck_result_row = src_img.rows - template_test.rows +1;
    int recheck_result_col = src_img.cols - template_test.cols +1;
    result = Mat(recheck_result_row, recheck_result_col, CV_32FC1);

    // 位置, 顏色 都有匹配的話 才算算一格 similar
    float total_pix = template_test.rows * template_test.cols;
    float similar;
    for(int go_s_row = 0; go_s_row < src_img.rows - template_test.rows +1; go_s_row++){
        for(int go_s_col = 0 ; go_s_col < src_img.cols - template_test.cols +1 ; go_s_col++){
            similar = 0;
            for(int go_t_row = 0 ; go_t_row < template_test.rows ; go_t_row++){
                for(int go_t_col = 0 ; go_t_col < template_test.cols ; go_t_col++){
                    //  另一種寫法：if( !(template_test.at<uchar>(go_t_row, go_t_col) - src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)) )
                    if(template_test.at<uchar>(go_t_row, go_t_col) == src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col) ){
                        similar++;
                    }
                }
            }
            result.at<float>(go_s_row,go_s_col) = similar / total_pix;
            // cout << "similar_rate = " << similar_rate << endl;
        }
    }
    // imshow("template_test",result);
    // waitKey(0);
}

// 有些音符黑色部分太少白色背景太多容易被拉走到背景多的地方相似度高, 所以就寫了此funtion只看黑色部分正確才similar++
static void matchTemplate2black(Mat src_img, Mat template_test, Mat& result){
    // copy matchTemplate 只有在最中間的if 多加 template_test.at<uchar>(go_t_row, go_t_col) == 0 這個條件
    float total_pix = template_test.rows * template_test.cols;
    for(int go_s_row = 0; go_s_row < src_img.rows - template_test.rows +1; go_s_row++){
        for(int go_s_col = 0 ; go_s_col < src_img.cols - template_test.cols +1 ; go_s_col++){
            float similar = 0;
            for(int go_t_row = 0 ; go_t_row < template_test.rows ; go_t_row++){
                for(int go_t_col = 0 ; go_t_col < template_test.cols ; go_t_col++){
                    //  另一種寫法：if( !(template_test.at<uchar>(go_t_row, go_t_col) - src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)) )
                    if(template_test.at<uchar>(go_t_row, go_t_col) == src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col) && 
                       template_test.at<uchar>(go_t_row, go_t_col) == 0    ){
                        similar++;
                    }
                }
            }
            result.at<float>(go_s_row,go_s_col) = similar / total_pix;
            // cout << "similar_rate = " << similar_rate << endl;
        }
    }
    // imshow("template_test",result);
    // waitKey(0);
}

// matchTemplate2 的視覺化版本, 把有完全對上的pixel標上色
void debug_matchTemplate2(Mat src_img, Mat template_img, int left, int top){
    Mat src_img_draw;
    cvtColor(src_img, src_img_draw, CV_GRAY2BGR);
    int right = left + template_img.cols - 1;
    int down  = top  + template_img.rows - 1;
    // 防呆
    if(left  < 0              ) left  = 0;
    if(right > src_img.cols -1) right = src_img.cols -1;
    if(top   < 0              ) top   = 0;
    if(down  > src_img.rows -1) down  = src_img.rows -1;
    int width  = right - left + 1;
    int height = down - top + 1;

    float total_pix = height * width;
    float similar = 0;
    float no_sim  = 0;
    float ok_ratio  = 0;
    // 完全匹配的地方, 白色標淺綠, 黑色標深綠
    // 不全匹配的地方, 白色標紅色, 黑色標深紅
    for(int go_y = 0 ; go_y < height; go_y++){
        for(int go_x = 0 ; go_x < width; go_x++){
            if( src_img.at<uchar>(go_y + top, go_x + left) == template_img.at<uchar>(go_y, go_x)){
                similar++;
                if     (src_img.at<uchar>(go_y + top, go_x + left) == 255) circle(src_img_draw, Point(go_x + left, go_y + top), 1, Scalar(  0, 255, 170), 1);
                else                                                       circle(src_img_draw, Point(go_x + left, go_y + top), 1, Scalar( 36, 135,   0), 1);
            }
            else{
                no_sim++;
                if     (src_img.at<uchar>(go_y + top, go_x + left) == 255) circle(src_img_draw, Point(go_x + left, go_y + top), 1, Scalar(  0,   0, 255), 1);
                else                                                       circle(src_img_draw, Point(go_x + left, go_y + top), 1, Scalar( 10,   0, 105), 1);
                
            }
        }
    }
    ok_ratio = similar / total_pix;

    // ok_ratio 轉 string
    stringstream ss;
    ss << setw(2) <<setfill('0') <<ok_ratio;
    string ok_ratio_str = ss.str();

    // 放大顯示
    int show_width = 150;
    float show_ratio = show_width / template_img.cols;
    cv::resize   (template_img, template_img, cv::Size(show_ratio * template_img.cols, show_ratio * template_img.rows));
    imshow       ("template_img", template_img);
    cvMoveWindow ("template_img", 10, 300);
    cv::resize   (src_img_draw, src_img_draw, cv::Size(show_ratio * src_img_draw.cols, show_ratio * src_img_draw.rows));
    imshow       (ok_ratio_str.c_str(), src_img_draw);
    cvMoveWindow (ok_ratio_str.c_str(), 180, 300);
    cv::resize   (src_img, src_img, cv::Size(show_ratio * src_img.cols, show_ratio * src_img.rows));
    imshow       ("src_img", src_img);
    cvMoveWindow ("src_img", 450, 300);

    cout << "src_img_draw.cols:" << src_img_draw.cols << endl;
    cout << "template_img.cols:" << template_img.cols << endl;
    waitKey(0);
    destroyWindow (ok_ratio_str.c_str());
    destroyWindow ("src_img");
}


void recognition_2_b_head_recheck(int head_type, Mat MaybeHead_final_template, Mat reduce_line, int& maybe_head_count, float maybe_head[][200], bool debuging){
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    // 先看一下 recheck 之前的 maybe_head 狀況
    if(debuging){
        cv::imshow("template_img", MaybeHead_final_template);
        MaybeHead_draw_w_color(debug_img, Mat(13, 15, CV_8UC1), maybe_head_count, maybe_head);
        cv::imshow("debug_img", debug_img);
        cvMoveWindow("debug_img", 10, 80);
    }
    // 會用到的變數先宣告
    Mat template_recheck;
    int maybe_head_x;
    int maybe_head_y;

    Mat recheck_region;
    int extend;
    int recheck_l;
    int recheck_r;
    int recheck_t;
    int recheck_d;
    int recheck_width;
    int recheck_height;
    bool recheck_sucess;
    Mat acc_result;  // 疊加樣本比對結果的容器

    for(int go_head = 0 ; go_head < maybe_head_count ; go_head ++){
        maybe_head_x = maybe_head[0][go_head];
        maybe_head_y = maybe_head[1][go_head];

        // 測試很多次, 信心0.75以上就是辨識成功了, 不用再recheck了直接指定信心100%
        if( maybe_head[2][go_head] >= 0.75) {
            maybe_head[2][go_head]  = 1.0;
            if(debuging){
                rectangle(debug_img, Point(maybe_head_x, maybe_head_y) , Point(maybe_head_x + template_recheck.cols, maybe_head_y + template_recheck.rows), Scalar(255, 0, 0), 2);
                cv::imshow  ("debug_img", debug_img);
                cvMoveWindow("debug_img", 10, 80);
            }
        }
        // 如果 信心沒有達到 0.75 就要 recheck
        else if(maybe_head[2][go_head] < 0.75 ){ //  如果太不像了~~recheck~~
            // 定出 recheck範圍: maybe_head 本身範圍 往左右上下 延伸 extend 個 pixel
            extend = 6;
            recheck_l = maybe_head[0][go_head] - extend;
            recheck_r = recheck_l + MaybeHead_final_template.cols + extend*2;
            recheck_t = maybe_head[1][go_head] - extend;
            recheck_d = recheck_t + MaybeHead_final_template.rows + extend*2;
            if(recheck_l < 0                  ) recheck_l = 0;
            if(recheck_r > reduce_line.cols -1) recheck_r = reduce_line.cols -1;
            if(recheck_t < 0                  ) recheck_t = 0;
            if(recheck_d > reduce_line.rows -1) recheck_d = reduce_line.rows -1;

            recheck_width  = recheck_r - recheck_l;
            recheck_height = recheck_d - recheck_t;
            // recheck範圍 看一下有沒有圈對
            if(debuging){
                cout << "recheck_l=" << recheck_l << " , recheck_r=" << recheck_r << ", recheck_t=" << recheck_t << " , recheck_d=" << recheck_d << endl;
                rectangle(debug_img, Point(recheck_l, recheck_t), Point(recheck_r, recheck_d), Scalar(0, 0, 255), 2);
                imshow("debug_img", debug_img);
            }
            // recheck的region 實際拉出來
            recheck_region = reduce_line(Rect( recheck_l, recheck_t, recheck_width, recheck_height )  );
            // *********************************************************

            recheck_sucess = false;
            // 疊加樣本比對結果的容器
            acc_result = Mat(recheck_height, recheck_width, CV_32FC1, Scalar(0));
            
            // 八分休止 recheck,
            // 先用原本有白色外框的 8-rest-white-both-2-2.bmp 把原本的基礎定出來,
            // 再疊加上 八分休止 最具特色的上半部分 8_up_very_fit2.bmp, 
            // 綜合兩層的結果 取平均, 信心高於 0.60 就OK囉
            if(head_type == 8){
                // 原本的 有外邊框的八分休止 做樣本比對
                template_recheck = imread("Resource/note/8-rest/8-rest-white-both-2-2.bmp", 0);   // 上下要留白，八分辨識度 & 區別度較高
                if(debuging) cout << "template_recheck.cols = " << template_recheck.cols << endl;

                int recheck_result_row = recheck_height - template_recheck.rows +1;
                int recheck_result_col = recheck_width  - template_recheck.cols +1;
                Mat recheck_result;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                // 八分休止最具特色的上半部分 做樣本比對
                template_recheck = imread("Resource/note/8-rest/8_up_very_fit2.bmp", 0);   //
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                // 有外邊框的八分休止  疊加上 八分休止最具特色的上半部分 做樣本比對的結果
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                // 取平均
                acc_result /= 2;

                // 觀察後覺得 超過0.60 就是八分休止
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( acc_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                // cout << "8 rest maxVal=" << maxVal << endl;
                // cv::imshow("debug_img", debug_img);
                // cv::imshow("acc_result", acc_result);
                // cv::waitKey(0);
                if(maxVal > 0.50){
                    recheck_sucess = true;
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 1);
                } 
            }
            else if(head_type == 6){
                // 原本的 有外邊框的十六分休止 做樣本比對
                int recheck_result_row;
                int recheck_result_col;

                Mat recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-white-both-1-1.bmp", 0);   // 上下不要留白，留白會抓到八分的休止符
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-01.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-02.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-03.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-04.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-05.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-06.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-07.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-08.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-09.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-10.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-11.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-12.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-13.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/6-rest/6-rest-hard-14.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                acc_result /= 15;

                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( acc_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );


                // 十六分休止 下面是白色的 做樣本比對
                template_recheck = imread("Resource/note/6-rest/6-rest-white-both-1-1_down_white.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                Mat recheck_result2(recheck_result_row,recheck_result_col, CV_32FC1);
                matchTemplate(recheck_region, template_recheck, recheck_result2, CV_TM_CCOEFF_NORMED);

                double minVal2; double maxVal2; Point minLoc2; Point maxLoc2;
                Point matchLoc2;
                minMaxLoc( recheck_result2 , &minVal2, &maxVal2, &minLoc2, &maxLoc2, Mat() );

                cout <<  "ord maxVal:" << maxVal << ", down white maxVal:" << maxVal2 << endl;
                if( maxVal > 0.40 and maxVal2 < 0.65){
                    cout << "OK" << endl;
                    recheck_sucess = true;
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 1);
                }
                // cv::imshow("debug_img", debug_img);
                // cv::waitKey(0);
            }
            else if(head_type == 7){
                // 原本的 有外邊框的三十二分休止 做樣本比對
                int recheck_result_row;
                int recheck_result_col;

                Mat recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-1-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                template_recheck = imread("Resource/note/32-rest/7-2-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-3-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-4-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-5-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-6-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-7-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                template_recheck = imread("Resource/note/32-rest/7-8-up15w.bmp", 0);
                if(template_recheck.rows > recheck_height) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                if(template_recheck.cols > recheck_width ) continue;  // 有時在太邊緣被切太多 切到比template小的話 這顆頭就跳過吧
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                matchTemplate2black(recheck_region, template_recheck, recheck_result);
                acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                acc_result /= 14;

                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( recheck_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

                cout <<  "ord maxVal:" << maxVal << endl;
                if( maxVal > 0.15 ){
                    recheck_sucess = true;
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 1);
                }
                // cv::imshow("debug_img", debug_img);
                // cv::waitKey(0);
            }
            else if(head_type == 2){
                // 二分音符
                for(int size = 14 ; size <= 15 ; size++ ){
                    template_recheck = imread("Resource/note/2/2-" + IntToString(size) +"-white-both-2.bmp", 0);
                    Mat recheck_result;
                    int recheck_result_row = recheck_height - template_recheck.rows +1;
                    int recheck_result_col = recheck_width  - template_recheck.cols +1;

                    matchTemplate2(recheck_region, template_recheck, recheck_result);

                    double minVal; double maxVal; Point minLoc; Point maxLoc;
                    Point matchLoc;
                    minMaxLoc( recheck_result , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                    debug_matchTemplate2(recheck_region, template_recheck, maxLoc.x, maxLoc.y);
                    cout << "kong2=" << maxVal << endl;
                    
                    acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                    

                    // CV_TM_CCOEFF_NORMED 有助於 把八分音符符尾 的 similarity 壓低, 所以 * 2
                    matchTemplate(recheck_region, template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                    minMaxLoc( recheck_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                    cout << "cv2=" << maxVal << endl;
                    
                    acc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result * 2;
                }
                acc_result /= 6;
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( acc_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                // cout << "old_value = " << maybe_head[2][go_head] <<  " , max_value_mean = " << maxVal << endl;
                // cv::imshow("recheck_result", reduce_line(Rect( recheck_l, recheck_t, recheck_width, recheck_height )));
                
                if(maxVal >= 0.49){
                    recheck_sucess = true;
                    // cout註解 recheck成功的話標記一下
                    // cout << "recheck_sucess";
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 1);
                    // *****************************
                    // imshow("recheck", debug_img);
                    // waitKey(0);
                }
            }
            else if(head_type == 5 || head_type == 9){
                // 四分休止符 和 高音譜記號
                if(head_type == 5) template_recheck = imread("Resource/note/4-rest/4-rest-white-both-1.bmp", 0);
                if(head_type == 9) template_recheck = imread("Resource/note/9/9-bin.bmp", 0);

                int recheck_result_row = recheck_height - template_recheck.rows +1;
                int recheck_result_col = recheck_width  - template_recheck.cols +1;
                Mat recheck_result(recheck_result_row,recheck_result_col, CV_32FC1);
                matchTemplate2(recheck_region, template_recheck, recheck_result);
    
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( recheck_result , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

                // 如果在某個size 有信心直超過0.80 就當作過關
                if(maxVal >= 0.800){
                    recheck_sucess = true;
                    // cout << "recheck_sucess";
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 1);
                }
            }
            else if(head_type == 0 || head_type == 4){
                // 全音符, 四分音符
                // 不同size 做樣本比對
                for(int size = 14 ; size <= 16 ; size++ ){
                    // cout << "go_head = " << go_head << " , ";
                    if(head_type == 4) template_recheck = imread("Resource/note/4/4-" + IntToString(size) +"-white-both-2.bmp", 0);
                    if(head_type == 0) template_recheck = imread("Resource/note/0/0-" + IntToString(size) +"-white-both-2.bmp", 0);
                    // imshow("template_recheck", template_recheck);
                    // waitKey(0);
                    // destroyWindow("template_recheck");

                    int recheck_result_row = recheck_height - template_recheck.rows +1;
                    int recheck_result_col = recheck_width  - template_recheck.cols +1;
                    Mat recheck_result(recheck_result_row,recheck_result_col, CV_32FC1);
                    matchTemplate2(recheck_region, template_recheck, recheck_result);
                    

                    double minVal; double maxVal; Point minLoc; Point maxLoc;
                    Point matchLoc;
                    minMaxLoc( recheck_result , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                    debug_matchTemplate2(recheck_region, template_recheck, maxLoc.x, maxLoc.y);

                    if(head_type == 0 && (size == 14)) maxVal += (float)32/(float)(template_recheck.rows*template_recheck.cols);
                    if(head_type == 0 && (size == 15)) maxVal += (float)46/(float)(template_recheck.rows*template_recheck.cols);
                    if(head_type == 0 && (size == 16)) maxVal += (float)59/(float)(template_recheck.rows*template_recheck.cols);

                    // 如果在某個size 有信心直超過0.80 就當作過關
                    if(maxVal >= 0.800){
                        recheck_sucess = true;
                        maybe_head[0][go_head] = recheck_l + maxLoc.x;
                        maybe_head[1][go_head] = recheck_t + maxLoc.y;
                        maybe_head[2][go_head] = maxVal;
                        rectangle(debug_img, Point(maybe_head[0][go_head], maybe_head[1][go_head]) , Point(maybe_head[0][go_head] + template_recheck.cols, maybe_head[1][go_head] + template_recheck.rows), Scalar(255, 0, 0), 2);
                        // *****************************
                        // imshow("recheck", debug_img);
                        // waitKey(0);
                    }
                    if(recheck_sucess == true) break;
                }
            }
            // head_type == 1(全休止符), 3(二分休止符) 前面的step已經做得很好了 不用recheck 所以這邊就沒有寫了

            if(recheck_sucess == false){
                // recheck失敗也標記一下
                if(debuging) cout << "recheck fail" << endl;
                rectangle(debug_img, Point(recheck_l, recheck_t), Point(recheck_r, recheck_d), Scalar(0, 0, 255), 1);
                // **************************
                // imshow("recheck", debug_img);
                // waitKey(0);
                position_erase(maybe_head_count, maybe_head, go_head);
                go_head--;
            }
            // cout << endl;
        }
    }
    // **************************
    // imshow("recheck", debug_img);

    //  debug整合
    // imshow("debug",debug_img);
    // waitKey(0);
    // destroyWindow("recheck");
    if(debuging){
        cv::destroyAllWindows();
    }

}
