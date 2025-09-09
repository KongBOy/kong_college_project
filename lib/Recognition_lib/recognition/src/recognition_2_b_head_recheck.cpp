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
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include "recognition_2_b_head_recheck.h"

#include "string_tools.h"


static void matchTemplate2(Mat src_img,Mat template_test,Mat& result)
{
    // 另一種寫法：
    /*
        Mat test2 = src_img(Rect(0,0,template_test.cols,template_test.rows)).clone();
        test = template_test-test2;
        cout << test << ' ' << endl;
        imshow("mat subscrib",test);
        waitKey(0);
    */
    // 另一種寫法：這寫法雖然要簡潔，但反而比較慢喔~~
    /*
        Mat test2(template_test.rows,template_test.cols,CV_8UC1);
        float total_pix = template_test.rows * template_test.cols;
        for(int go_s_row = 0 ; go_s_row < src_img.rows - template_test.rows +1 ; go_s_row++)
        {
            for(int go_s_col = 0 ; go_s_col < src_img.cols - template_test.cols +1 ; go_s_col++)
            {
                float similar = 0;
                test2 = src_img(Rect(go_s_col,go_s_row,template_test.cols,template_test.rows)).clone();
                test2 -= template_test;
                for(int go_t_row = 0 ; go_t_row < template_test.rows ; go_t_row++)
                {
                    for(int go_t_col = 0 ; go_t_col < template_test.cols ; go_t_col++)
                    {
                        if( !test2.at<uchar>(go_t_row,go_t_col) ) similar++;
                    }
                }
            }
        }
    */

    float total_pix = template_test.rows * template_test.cols;
    for(int go_s_row = 0 ; go_s_row < src_img.rows - template_test.rows +1 ; go_s_row++){
        for(int go_s_col = 0 ; go_s_col < src_img.cols - template_test.cols +1 ; go_s_col++){
            float similar = 0;
            for(int go_t_row = 0 ; go_t_row < template_test.rows ; go_t_row++){
                for(int go_t_col = 0 ; go_t_col < template_test.cols ; go_t_col++){
                    //  另一種寫法：if( !(template_test.at<uchar>(go_t_row,go_t_col) - src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)) )
                    if(template_test.at<uchar>(go_t_row,go_t_col) == src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)){
                        similar++;
                    }
                }
            }
            // float similar_rate = similar / total_pix;
            result.at<float>(go_s_row,go_s_col) = similar / total_pix;// similar_rate;
            // cout << "similar_rate = " << similar_rate << endl;
        }
    }
    // imshow("template_test",result);
    // waitKey(0);
}


void recognition_2_b_head_recheck(int head_type,Mat reduce_line,int& maybe_head_count,float maybe_head[][200]){
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line, debug_img, CV_GRAY2BGR);

    draw_head(debug_img, Mat(13, 15, CV_8UC1), maybe_head_count, maybe_head);

    // ~~~template Matching
    Mat template_recheck ;
    if(head_type == 0) template_recheck = imread("Resource/note/0/0.bmp",0);
    if(head_type == 2) template_recheck = imread("Resource/note/2/2.bmp",0);
    if(head_type == 4) template_recheck = imread("Resource/note/4/4.bmp",0);
    if(head_type == 5) template_recheck = imread("Resource/note/4-rest/4-rest-white-both-1.bmp",0);
    if(head_type == 1) template_recheck = imread("Resource/note/0-rest/0-rest-14-white-both-3-3-3.bmp",0);
    if(head_type == 3) template_recheck = imread("Resource/note/2-rest/2-rest-14-white-both-3-3-3.bmp",0);
    if(head_type == 8) template_recheck = imread("Resource/note/8-rest/8-rest-white-both-2-2.bmp",0);   // 上下要留白，八分辨識度 & 區別度較高
    if(head_type == 6) template_recheck = imread("Resource/note/6-rest/6-rest-white-both-2-2-3.bmp",0); // 上下不要留白，留白會抓到八分的休止符
    if(head_type == 7) template_recheck = imread("Resource/note/32-rest/7-white-both-2.bmp",0);         // 上下不要留白，留白會抓到八分的休止符

    if(head_type == 9) template_recheck = imread("Resource/note/9/9-bin.bmp",0);
    // cout註解
    // cout << "template_recheck.cols = " << template_recheck.cols << endl;
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head ++){
        // 測試很多次, 信心0.75以上就是辨識成功了, 不用再recheck了直接指定信心100%
        if(maybe_head[2][go_head] >=0.75 && head_type != 1 && head_type != 3) {
            rectangle(debug_img,Point(maybe_head[0][go_head],maybe_head[1][go_head]) , Point(maybe_head[0][go_head]+template_recheck.cols,maybe_head[1][go_head]+template_recheck.rows),Scalar(255,0,0),1);
            maybe_head[2][go_head] = 1.0;
        }
        // 如果 信心沒有達到 0.75 就要 recheck
        else if(maybe_head[2][go_head] < 0.75 || //  如果太不像了~~recheck~~
                head_type == 1 ||  // 或者 是 全休止   這種超級容易搞混的東西
                head_type == 3){   // 或者 是 二分休止 這種超級容易搞混的東西
            int extend = 6;
            int recheck_l = maybe_head[0][go_head] - extend;
            int recheck_r = recheck_l + template_recheck.cols + extend*2;
            int recheck_t = maybe_head[1][go_head] - extend;
            int recheck_d = recheck_t + template_recheck.rows + extend*2;
            if(recheck_l < 0                  ) recheck_l = 0;
            if(recheck_r > reduce_line.cols -1) recheck_r = reduce_line.cols -1;
            if(recheck_t < 0                  ) recheck_t = 0;
            if(recheck_d > reduce_line.rows -1) recheck_d = reduce_line.rows -1;

            int recheck_width  = recheck_r - recheck_l;
            int recheck_height = recheck_d - recheck_t;
            // cout << "recheck_l = " << recheck_l << " , recheck_r = " << recheck_r << endl;
            // **************************************
            rectangle(debug_img, Point(recheck_l, recheck_t), Point(recheck_r, recheck_d), Scalar(0, 0, 255), 1);
            // imshow("recheck",debug_img);
            // waitKey(0);
            // *********************************************************


            bool recheck_sucess = false;
            // 疊加樣本比對結果的容器
            Mat proc_result = Mat(recheck_height, recheck_width, CV_32FC1, Scalar(0));
            
            // 八分休止 recheck,
            // 先用原本有白色外框的 8-rest-white-both-2-2.bmp 把原本的基礎定出來,
            // 再疊加上 八分休止 最具特色的上半部分 8_up_very_fit2.bmp, 
            // 綜合兩層的結果 取平均, 信心高於 0.60 就OK囉
            if(head_type == 8){
                // 原本的 有外邊框的八分休止 做樣本比對
                int recheck_result_row = recheck_height - template_recheck.rows +1;
                int recheck_result_col = recheck_width  - template_recheck.cols +1;
                Mat recheck_result;
                matchTemplate(reduce_line(Rect( recheck_l,recheck_t,recheck_width,recheck_height )  ), template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                proc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                
                // 八分休止最具特色的上半部分 做樣本比對
                template_recheck = imread("Resource/note/8-rest/8_up_very_fit2.bmp",0);   //
                recheck_result_row = recheck_height - template_recheck.rows +1;
                recheck_result_col = recheck_width  - template_recheck.cols +1;
                matchTemplate(reduce_line(Rect( recheck_l,recheck_t,recheck_width,recheck_height )  ), template_recheck, recheck_result, CV_TM_CCOEFF_NORMED);
                // 有外邊框的八分休止  疊加上 八分休止最具特色的上半部分 做樣本比對的結果
                proc_result(  Rect(0, 0, recheck_result_col, recheck_result_row) ) += recheck_result;
                // 取平均
                proc_result /= 2;

                // 觀察後覺得 超過0.60 就是八分休止
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;
                minMaxLoc( proc_result , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                // cout << "8 rest maxVal=" << maxVal << endl;
                // cv::imshow("debug_img", debug_img);
                // cv::imshow("proc_result", proc_result);
                // cv::waitKey(0);
                if(maxVal > 0.60){
                    recheck_sucess = true;
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img,Point(maybe_head[0][go_head],maybe_head[1][go_head]) , Point(maybe_head[0][go_head]+template_recheck.cols,maybe_head[1][go_head]+template_recheck.rows),Scalar(255,0,0),1);
                } 
            }

            for(int size = 14 ; size <= 16 ; size++ ){
                // cout註解 看現在正在處理哪顆頭
                // cout << "go_head = " << go_head << " , ";
                if(head_type == 4) template_recheck = imread("Resource/note/4/4-" + IntToString(size) +"-white-both-2.bmp",0);
                if(head_type == 2) template_recheck = imread("Resource/note/2/2-" + IntToString(size) +"-white-both-2.bmp",0);
                if(head_type == 0) template_recheck = imread("Resource/note/0/0-" + IntToString(size) +"-white-both-2.bmp",0);

                // cout << template_recheck << " " << endl;

                // imshow("tempalte_recheck",template_recheck);
                // waitKey(0);
                // destroyWindow("tempalte_recheck");

                int recheck_result_row = recheck_height - template_recheck.rows +1;
                int recheck_result_col = recheck_width  - template_recheck.cols +1;
                Mat recheck_result(recheck_result_row,recheck_result_col,CV_32FC1);
                matchTemplate2(reduce_line(Rect( recheck_l,recheck_t,recheck_width,recheck_height )  ), template_recheck, recheck_result);

                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;

                minMaxLoc( recheck_result , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                if(head_type == 2)                               maxVal += (float)34/(float)(template_recheck.rows*template_recheck.cols);
                if(head_type == 2 && (size == 15 || size == 16)) maxVal += (float) 8/(float)(template_recheck.rows*template_recheck.cols);
                if(head_type == 0 && (size == 14)) maxVal += (float)32/(float)(template_recheck.rows*template_recheck.cols);
                if(head_type == 0 && (size == 15)) maxVal += (float)46/(float)(template_recheck.rows*template_recheck.cols);
                if(head_type == 0 && (size == 16)) maxVal += (float)59/(float)(template_recheck.rows*template_recheck.cols);

                if(head_type == 6 && (size == 14)) maxVal -= 0.08 ; // 再多 -0.01是因為如果音符是8分音符，maxVal也會很高~~所以多-0.1讓他比較不會超過門檻~~
                if(head_type == 7 && (size == 14)) maxVal -= 0.08 ; // 再多 -0.01是因為如果音符是8分音符，maxVal也會很高~~所以多-0.1讓他比較不會超過門檻~~
                // if(head_type == 1 && (size == 14)) maxVal += (float)14/(float)(template_recheck.rows*template_recheck.cols);

                // cout註解 看recheck後的相似度 和原來的相似度
                cout << "old_value = " << maybe_head[2][go_head] << ", recheck_size" << size << " , max_value_kong = " << maxVal;
                if(maxVal >= 0.800){
                    /*
                    Mat debug_img2 = reduce_line.clone();
                    cvtColor(reduce_line,debug_img2,CV_GRAY2BGR);

                    rectangle(debug_img,Point(recheck_l,recheck_t),Point(recheck_r,recheck_d),Scalar(255,0,0),2);
                    for(int debug_y = 0 ; debug_y < template_recheck.rows ; debug_y++){
                        for(int debug_x = 0 ; debug_x < template_recheck.cols ; debug_x++){
                            if(   reduce_line     .at<uchar>(debug_y + recheck_t + maxLoc.y , debug_x + recheck_l + maxLoc.x)
                               == template_recheck.at<uchar>(debug_y                        , debug_x)){
                                line(debug_img2,Point(debug_x + recheck_l + maxLoc.x,debug_y + recheck_t + maxLoc.y),
                                                Point(debug_x + recheck_l + maxLoc.x,debug_y + recheck_t + maxLoc.y),Scalar(0,255,0),1);
                            }
                        }
                    }

                    imshow("recheck_debug", debug_img2);
                    waitKey(0);
                    */
                    // ***************************
                    // imshow("recheck",debug_img);
                    // waitKey(0);

                    recheck_sucess = true;
                    // cout註解 recheck成功的話標記一下
                    // cout << "recheck_sucess";
                    maybe_head[0][go_head] = recheck_l + maxLoc.x;
                    maybe_head[1][go_head] = recheck_t + maxLoc.y;
                    maybe_head[2][go_head] = maxVal;
                    rectangle(debug_img,Point(maybe_head[0][go_head],maybe_head[1][go_head]) , Point(maybe_head[0][go_head]+template_recheck.cols,maybe_head[1][go_head]+template_recheck.rows),Scalar(255,0,0),1);
                    // *****************************
                    // imshow("recheck",debug_img);
                    // waitKey(0);
                }
                if(recheck_sucess == true) break;
                if(head_type == 5) break;  // 四分休止
                if(head_type == 1) break;  // 全休止
                if(head_type == 3) break;  // 二分休
                if(head_type == 8) break;  // 八分休止
                if(head_type == 6) break;  // 十六分休止
                if(head_type == 7) break;  // 三十二分休止
                if(head_type == 9) break;  // 高音譜記號
            }


            if(recheck_sucess == false){
                // cout註解 recheck失敗也標記一下
                // cout << "this head might not be head" << endl;
                rectangle(debug_img,Point(recheck_l,recheck_t),Point(recheck_r,recheck_d),Scalar(0, 255, 0), 1);
                // **************************
                // imshow("recheck",debug_img);
                // waitKey(0);
                position_erase(maybe_head_count,maybe_head,go_head);
                go_head--;
            }
            cout << endl;
        }
    }
    // **************************
    // imshow("recheck",debug_img);

    //  debug整合
    // imshow("debug",debug_img);
    // waitKey(0);
    // destroyWindow("recheck");
}
