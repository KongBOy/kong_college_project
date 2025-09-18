/*
這個程式的功能是：
找出 輸入符號 的精確位置，但是找到得符號還不確定是確的，只能說"可能是"
比對符號的方法是opencv的function
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include <cstring>
#include <direct.h>
#include <iomanip>

#include "recognition_0_array_tools.h"
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include "recognition_1_find_all_maybe_head.h"


using namespace cv;
using namespace std;


static int start_time = 0;
static int end_time = 0;


void minMaxLoc2(Mat img,double& maxVal ,Point& maxLoc){
    maxVal = -10000;
    for(int go_row = 0 ; go_row < img.rows ; go_row++){
        for(int go_col = 0 ; go_col < img.cols ; go_col++){
            if(img.at<float>(go_row,go_col) > maxVal){
                maxVal = img.at<float>(go_row,go_col);
                maxLoc.x = go_col;
                maxLoc.y = go_row;
            }
        }
    }
}

void matchTemplate2(Mat src_img,Mat template_test,Mat& result){
    float total_pix = template_test.rows * template_test.cols;
    for(int go_s_row = 0 ; go_s_row < src_img.rows - template_test.rows +1 ; go_s_row++){
        for(int go_s_col = 0 ; go_s_col < src_img.cols - template_test.cols +1 ; go_s_col++){
            float similar = 0;
            for(int go_t_row = 0 ; go_t_row < template_test.rows ; go_t_row++){
                for(int go_t_col = 0 ; go_t_col < template_test.cols ; go_t_col++){
                    /// 另一種寫法：if( !(template_test.at<uchar>(go_t_row,go_t_col) - src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)) )
                    if(template_test.at<uchar>(go_t_row,go_t_col) == src_img.at<uchar>(go_s_row + go_t_row,go_s_col + go_t_col)){
                        similar++;
                    }
                }
            }
            // float similar_rate = similar / total_pix;
            result.at<float>(go_s_row,go_s_col) = similar / total_pix;// similar_rate;
            // cout<<"similar_rate = "<<similar_rate<<endl;
        }
    }
    // imshow("template_test",result);
    // waitKey(0);
}

void debug_draw_result_map_on_staff_bin_erase_line(Mat result_map, Mat staff_bin_erase_line, Mat template_img, int l, int r, int t, int d, Scalar color, string window_name){
    Mat staff_bin_erase_line_color;
    cvtColor(staff_bin_erase_line, staff_bin_erase_line_color, CV_GRAY2BGR);
    for(int go_row = t; go_row <= d; go_row++)
        for(int go_col = l; go_col <= r; go_col++)
            if(result_map.at<float>(go_row, go_col) ) 
                rectangle( staff_bin_erase_line_color, Point(go_col, go_row), Point( go_col + template_img.cols, go_row + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
    imshow(window_name, staff_bin_erase_line_color);
}
void debug_draw_merging_where(Mat result_map, Mat staff_bin_erase_line, Mat template_img, int x, int y, Scalar color, string window_name){
    int check_l, check_r, check_t, check_d;
    check_l = x - 0.5 * template_img.cols;
    check_r = x + 0.5 * template_img.cols;
    check_t = y - 0.5 * template_img.rows;
    check_d = y + 0.5 * template_img.rows;
    // ************ 防呆 ***************
    if( check_l < 0) check_l = 0;
    if( check_t < 0) check_t = 0;
    if( check_r > result_map.cols-1) check_r = result_map.cols -1;
    if( check_d > result_map.rows-1) check_d = result_map.rows -1;
    // ************ 防呆 ***************

    // 看 merge 的範圍在哪裡
    Mat result_map_color;
    cvtColor(result_map, result_map_color, CV_GRAY2BGR);
    rectangle(result_map_color, Point(check_l, check_t), Point(check_r, check_d), color, 1, 8, 0);
    imshow(window_name, result_map_color);
    imshow("template_img", template_img);
    cout << result_map( Rect(check_l, check_t, template_img.cols, template_img.rows) ) << " " << endl;

    // 只看 merge 範圍內 的 result_map 在 staff_bin_erase_line 的狀況
    debug_draw_result_map_on_staff_bin_erase_line(result_map, staff_bin_erase_line, template_img, check_l, check_r, check_t, check_d, Scalar(0, 0, 255), window_name + "_staff_line");
    
    // 看整張圖 的 result_map 在 staff_bin_erase_line 的狀況
    debug_draw_result_map_on_staff_bin_erase_line(result_map, staff_bin_erase_line, template_img, 0, result_map.cols - 1, 0, result_map.rows - 1, Scalar(0, 0, 255), window_name + "_staff_line_all");
}


// 
void MaybeHead_MergeCloseHead(Mat& result_map, Mat staff_bin_erase_line, Mat template_img){
    for(int go_row = 0 ; go_row < result_map.rows ; go_row++){
        for(int go_col = 0 ; go_col < result_map.cols ; go_col++){
            if(result_map.at<float>(go_row,go_col)){
                // 一、框result_map的框框，防呆變數設定START///
                int left  = go_col - 0.5 * template_img.cols;
                int right = go_col + 0.5 * template_img.cols;
                int top   = go_row - 0.5 * template_img.rows;  // 0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~
                int down  = go_row + 0.5 * template_img.rows;  // 0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~

                // 如果超出範圍，就設定為最大範圍
                if(top   < 0) top = 0;
                if(left  < 0) left = 0;
                if(right > result_map.cols-1) right = result_map.cols - 1;
                if(down  > result_map.rows-1) down  = result_map.rows - 1;

                int range_width  = right - left;
                int range_height = down - top;

                // before merge 看一下
                // debug_draw_merging_where(result_map, staff_bin_erase_line, template_img, go_col, go_row, Scalar(0, 0, 255), "mergeing where");
                // waitKey(0);

                // 二、找出框框內最好的點(最像的地方)
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;

                minMaxLoc( result_map( Rect(left,top,range_width+1,range_height+1) ) , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                ///minMaxLoc2(result_map(Rect(left,top,range_width,range_height)),maxVal,maxLoc);
                maxLoc.x += left; // 因為上面是用ROI，所以要位移到正確的位置
                maxLoc.y += top;  // 因為上面是用ROI，所以要位移到正確的位置


                // 三、只留下那一點
                // 因為做threshold 清不乾淨所以乾脆直接自己寫for迴圈囉~~
                for(int go_note_row = top ; go_note_row <= down ; go_note_row++ )
                    for(int go_note_col = left ; go_note_col <= right ; go_note_col++)
                        result_map.at<float>(go_note_row, go_note_col) = 0;
                result_map.at<float>(maxLoc.y, maxLoc.x) = maxVal;
                cout << "value = " << maxVal << endl;

                // after merge 看一下
                // debug_draw_merging_where(result_map, staff_bin_erase_line, template_img, go_col, go_row, Scalar(0, 0, 255), "mergeing where");
                // waitKey(0);
            }
        }
    }

}

void Grab_MaybeHead_from_ResultMap(Mat result_map, int& maybe_head_count,float maybe_head[][200], int pitch_base_y, Mat staff_bin_erase_line, Mat template_img, float thresh_hold){
    // 五、簡單篩一下，取大量喔！就是找出可能是要找的頭的概念！之後再用特徵篩一次~~
    // normalize( result_map, result_map, 0, 1, NORM_MINMAX, -1, Mat() ); ///如果用這個的話就連其他版本的譜好像也可以，thr設0.75
    threshold(result_map, result_map, thresh_hold , 1.0 , CV_THRESH_TOZERO);
    // debug用  把所有找到的地方都框出來，還沒有合併附近很像的地方
    // debug_draw_result_map_on_staff_bin_erase_line(result_map, staff_bin_erase_line, template_img, 0, result_map.cols - 1, 0, result_map.rows - 1, Scalar(0, 0, 255), "before merge");
    // *******************************************************************************************************
    // 把附近找出來一堆很像的的東西合併成一個：
    // 一、先框好範圍，
    // 二、找出框框內最好的點，
    // 三、只留下那個點其他點去掉，
    MaybeHead_MergeCloseHead(result_map, staff_bin_erase_line,template_img);
    // debug用 合併完也看看
    // debug_draw_result_map_on_staff_bin_erase_line(result_map, staff_bin_erase_line, template_img, 0, result_map.cols - 1, 0, result_map.rows - 1, Scalar(0, 0, 255), "after merge");
    // waitKey(0);
    
    // *******************************************************************************************************
    // 自己設資料結構 移到外面去囉
    //    int maybe_head_count = 0;
    //    float maybe_head[3][200];
    //    for(int i = 0 ; i < 3 ; i++)
    //        for(int j = 0 ; j < 200 ; j++)
    //            maybe_head[i][j] = 0;
    // 四、把可能是頭的點存進我的data structure，改寫from SHOW START 沒有用我的資料結構///
    int far_from_staff_limit = 65;  // 如果離五線譜太遠(超過 far_from_staff_limit) 也不存
    for(int go_row = 0; go_row < result_map.rows ; go_row++){
        for(int go_col = 0 ; go_col < result_map.cols ; go_col++){
            if( (result_map.at<float>(go_row,go_col) ) &&
                (go_row - pitch_base_y >= -1 * far_from_staff_limit) && (go_row - pitch_base_y <= 50 + far_from_staff_limit) ){  
                /// ~~~~~~ debug用 ~~~~~~
                /*
                if     (result_map.at<float>(go_row,go_col) >= 0.70                                               ) rectangle( temp_show, Point(go_col,go_row), Point( go_col + template_img.cols ,go_row + template_img.rows ), Scalar(255,   0,   0), 1, 8, 0 );
                else if(result_map.at<float>(go_row,go_col) <  0.70 && result_map.at<float>(go_row,go_col) >= 0.49) rectangle( temp_show, Point(go_col,go_row), Point( go_col + template_img.cols ,go_row + template_img.rows ), Scalar(  0, 255,   0), 1, 8, 0 );
                else if(result_map.at<float>(go_row,go_col) <  0.49 && result_map.at<float>(go_row,go_col) >= 0.43) rectangle( temp_show, Point(go_col,go_row), Point( go_col + template_img.cols ,go_row + template_img.rows ), Scalar(  0,   0, 255), 1, 8, 0 );
                else                                                                                                rectangle( temp_show, Point(go_col,go_row), Point( go_col + template_img.cols ,go_row + template_img.rows ), Scalar( 50, 150, 255), 2, 8, 0 );
                */

                maybe_head[0][maybe_head_count] = go_col; /// x
                maybe_head[1][maybe_head_count] = go_row; /// y
                maybe_head[2][maybe_head_count] = result_map.at<float>(go_row,go_col); /// value
                maybe_head_count++;
            }
        }
    }

    Mat temp_show;
    cvtColor(staff_bin_erase_line, temp_show, CV_GRAY2BGR);
    draw_head(temp_show,template_img,maybe_head_count,maybe_head);

    // 把可能是頭的點存進我的 data_structure，改寫from SHOW END 沒有用我的資料結構///

    bubbleSort_maybe_head(maybe_head_count,maybe_head,Y_INDEX);
    bubbleSort_maybe_head(maybe_head_count,maybe_head,X_INDEX);
    // imshow("after_merge",temp_show);

    // debug整合
    // imshow("debug",temp_show);
    // waitKey(0);
}

void recognition_1_find_all_MaybeHead(Mat& result_map, Mat template_img, Mat staff_bin_erase_line,
                                       int e_count, int* l_edge, int* distance,
                                       string method){

    /// 一、整個大圖片的 result_map 容器
    /// 二、加速，看想看的小地方地方即可：根據垂直投影找出來的mountain切
    /// 三、對想看的小地方做template_match，結果存在小result
    /// 四、把 小結果result 加回去 大圖result_map

    // 一、建立 放原始圖片做樣本比對結果的容器
    // 因為是用樣本比對 用 樣本一格格滑過原始影像 計算 每隔往右切往下切樣本大小的影像 與 樣本 比對相似度, 所以容器大小是: 原圖大小 - template大小 + 1
    int result_map_row = staff_bin_erase_line.rows - template_img.rows+1;
    int result_map_col = staff_bin_erase_line.cols - template_img.cols+1;
    if(result_map.empty()){
        result_map = Mat(result_map_row, result_map_col, CV_32FC1, Scalar(0));
    }
    else{
        if(result_map_row > result_map.rows || result_map_col > result_map.cols){
            Mat temp(result_map_row, result_map_col, CV_32FC1, Scalar(0));
            temp(Rect(0, 0, result_map.cols, result_map.rows)) += result_map;
            result_map = temp;
        }
    }

    // cout << "result_map_row = " << result_map_row << endl;
    // cout << "result_map_col = " << result_map_col << endl;
    
    // 走訪每座山
    int grab_left;
    int grab_width;
    for(int go_mountain = 0 ; go_mountain < e_count ; go_mountain++){
        // 防呆, 如果template的寬度比較大的話, 就把要看的山的距離拉大點囉
        int width_error_max = 40;  // 最大可以差40個組 width_error
        int width_error_go  =  0;
        grab_left  = l_edge[go_mountain];
        grab_width = distance[go_mountain];
        while(grab_width < template_img.cols && width_error_go < width_error_max){
            if(grab_left > 0 ) grab_left--;  // 如果在圖片範圍內, 左擴1
            if(grab_left+grab_width < staff_bin_erase_line.cols-1) grab_width += 2;  // 如果在圖片範圍內, 右擴1(因為grab_left--了, 所以想要右擴1 需要 grab_width +2 來抵銷 grab_left--)
            width_error_go++;
            // cout << "width_error_go = " << width_error_go << " , grab_width = " << grab_width << endl;
        }

        // 二、根據上面防呆後的垂直投影找出來的mountain 根據 左邊界 和 distance 來切圖, 因為 很多符號 都是 瘦瘦高高, 所以 只對寬度仔細切, 高度抓全部 比較安全
        Mat proc_img = staff_bin_erase_line(Rect(grab_left,0, grab_width, staff_bin_erase_line.rows ));

        // 三前置、 建立 放 根據垂直投影切出來的影像做樣本比對結果的容器，根據垂直投影找出來的mountain切, 所以容器大小是: 山圖的大小 - template大小 + 1
        int result_row = staff_bin_erase_line.rows - template_img.rows +1;
        int result_col = grab_width     - template_img.cols +1;
        cout << "result_row = " << result_row << endl;
        cout << "result_col = " << result_col << endl;
        Mat result(result_row,result_col,CV_32FC1);

        // 三、 每座山圖 做樣本比對
        if(method == "method1") matchTemplate (proc_img, template_img, result, CV_TM_CCOEFF_NORMED);
        else                    matchTemplate2(proc_img, template_img, result);
        // threshold(result, result, 0.5, 1., CV_THRESH_TOZERO);

        // 四、 山圖樣本比對的結果圖 根據 左邊界 加回 原始影像比對的結果圖 相應的位置
        result_map( Rect(grab_left,0,result_col, result_row) ) += result;

        // imshow("proc_img", proc_img);
        // imshow("result_map", result_map);
        // cv::imshow("template_img", template_img);
        // cout << result << endl << endl << endl;
        // waitKey(0);
    }



}
