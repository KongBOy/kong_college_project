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
#include "Maybe_head_tool.h"
#include "recognition_1_find_all_maybe_head.h"


using namespace cv;
using namespace std;


static int start_time = 0;
static int end_time = 0;



// 位置, 顏色 都有匹配的話 才算算一格 similar
void matchTemplate2(Mat src_img, Mat template_test, Mat& result){
    float total_pix = template_test.rows * template_test.cols;
    float similar;
    for(int go_s_row = 0; go_s_row < src_img.rows - template_test.rows +1; go_s_row++){
        for(int go_s_col = 0; go_s_col < src_img.cols - template_test.cols +1; go_s_col++){
            similar = 0;
            for(int go_t_row = 0; go_t_row < template_test.rows; go_t_row++){
                for(int go_t_col = 0; go_t_col < template_test.cols; go_t_col++){
                     // 另一種寫法：if( !(template_test.at<uchar>(go_t_row, go_t_col) - src_img.at<uchar>(go_s_row + go_t_row, go_s_col + go_t_col)) )
                    if(template_test.at<uchar>(go_t_row, go_t_col) == src_img.at<uchar>(go_s_row + go_t_row, go_s_col + go_t_col)){
                        similar++;
                    }
                }
            }
            // float similar_rate = similar / total_pix;
            result.at<float>(go_s_row, go_s_col) = similar / total_pix;// similar_rate;
            // cout<<"similar_rate = "<<similar_rate<<endl;
        }
    }
    // imshow("template_test", result);
    // waitKey(0);
}

void debug_draw_result_map_on_staff_bin_erase_line(Mat staff_result_map, Mat staff_bin_erase_line, Mat template_img, int l, int r, int t, int d, Scalar color, string window_name){
    // ************ 防呆 ***************
    if( l < 0) l = 0;
    if( t < 0) t = 0;
    if( r > staff_result_map.cols-1) r = staff_result_map.cols -1;
    if( d > staff_result_map.rows-1) d = staff_result_map.rows -1;

    Mat staff_bin_erase_line_color;
    cvtColor(staff_bin_erase_line, staff_bin_erase_line_color, CV_GRAY2BGR);
    for(int go_row = t; go_row <= d; go_row++)
        for(int go_col = l; go_col <= r; go_col++)
            if(staff_result_map.at<float>(go_row, go_col) ) 
                rectangle( staff_bin_erase_line_color, Point(go_col, go_row), Point( go_col + template_img.cols, go_row + template_img.rows ), Scalar(0, 0, 255), 1, 8, 0 );
    imshow(window_name, staff_bin_erase_line_color);
}

void debug_draw_merging_where(Mat staff_result_map, Mat staff_bin_erase_line, Mat template_img, int x, int y, Scalar color, string window_name, bool print_result_map){
    int check_l, check_r, check_t, check_d;
    check_l = x - 0.5 * template_img.cols;
    check_r = x + 0.5 * template_img.cols;
    check_t = y - 0.5 * template_img.rows;
    check_d = y + 0.5 * template_img.rows;
    // ************ 防呆 ***************
    if( check_l < 0) check_l = 0;
    if( check_t < 0) check_t = 0;
    if( check_r > staff_result_map.cols-1) check_r = staff_result_map.cols -1;
    if( check_d > staff_result_map.rows-1) check_d = staff_result_map.rows -1;
    // ************ 防呆 ***************

    // 看 merge 的範圍在哪裡
    Mat result_map_color;
    cvtColor(staff_result_map, result_map_color, CV_GRAY2BGR);
    rectangle(result_map_color, Point(check_l, check_t), Point(check_r, check_d), color, 1, 8, 0);
    imshow(window_name, result_map_color);
    imshow("template_img", template_img);
    if(print_result_map) cout << staff_result_map( Rect(check_l, check_t, check_r - check_l, check_d - check_t) ) << " " << endl;

    // 只看 merge 範圍內 的 staff_result_map 在 staff_bin_erase_line 的狀況
    debug_draw_result_map_on_staff_bin_erase_line(staff_result_map, staff_bin_erase_line, template_img, check_l, check_r, check_t, check_d, Scalar(0, 0, 255), window_name + "_staff_line");
    
    // 看整張圖 的 staff_result_map 在 staff_bin_erase_line 的狀況
    debug_draw_result_map_on_staff_bin_erase_line(staff_result_map, staff_bin_erase_line, template_img, 0, staff_result_map.cols - 1, 0, staff_result_map.rows - 1, Scalar(0, 0, 255), window_name + "_staff_line_all");
}


// staff_result_map 遇到非0時 將其周圍 0.5 template 大小的區域 只留下一點最大值
void MaybeHead_MergeCloseHead(Mat& staff_result_map, Mat staff_bin_erase_line, Mat template_img){
    for(int go_row = 0; go_row < staff_result_map.rows; go_row++){
        for(int go_col = 0; go_col < staff_result_map.cols; go_col++){
            if(staff_result_map.at<float>(go_row, go_col)){
                // 一、框result_map的框框，防呆變數設定START //
                int left  = go_col - 0.5 * template_img.cols;
                int right = go_col + 0.5 * template_img.cols;
                int top   = go_row - 0.5 * template_img.rows;  // 0.5那個是因為音符不可能會兩顆重疊在一起
                int down  = go_row + 0.5 * template_img.rows;  // 0.5那個是因為音符不可能會兩顆重疊在一起

                // 防呆
                if(top   < 0) top = 0;
                if(left  < 0) left = 0;
                if(right > staff_result_map.cols-1) right = staff_result_map.cols - 1;
                if(down  > staff_result_map.rows-1) down  = staff_result_map.rows - 1;

                int range_width  = right - left;
                int range_height = down - top;

                // before merge 看一下
                // debug_draw_merging_where(staff_result_map, staff_bin_erase_line, template_img, go_col, go_row, Scalar(0, 0, 255), "mergeing where", true);
                // waitKey(0);

                // 二、找出框框內最好的點(最像的地方)
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;

                minMaxLoc( staff_result_map( Rect(left, top, range_width+1, range_height+1) ) , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                maxLoc.x += left; // 因為上面是用ROI，所以要位移到正確的位置
                maxLoc.y += top;  // 因為上面是用ROI，所以要位移到正確的位置


                // 三、只留下那一點
                // 因為做threshold 清不乾淨所以乾脆直接自己寫for迴圈囉~~
                for(int go_note_row = top; go_note_row <= down; go_note_row++ )
                    for(int go_note_col = left; go_note_col <= right; go_note_col++)
                        staff_result_map.at<float>(go_note_row, go_note_col) = 0;
                staff_result_map.at<float>(maxLoc.y, maxLoc.x) = maxVal;
                
                // after merge 看一下
                // debug_draw_merging_where(staff_result_map, staff_bin_erase_line, template_img, go_col, go_row, Scalar(0, 0, 255), "mergeing where", false);
                // cout << "value = " << maxVal << endl;
                // waitKey(0);
            }
        }
    }

}

void Grab_MaybeHead_from_ResultMap(Mat staff_result_map, int& maybe_head_count, float maybe_head[][200], int pitch_base_y, Mat staff_bin_erase_line, Mat template_img, float thresh_hold){
    Mat temp_show;
    cvtColor(staff_bin_erase_line, temp_show, CV_GRAY2BGR);
    // MaybeHead_draw(temp_show, template_img, maybe_head_count, maybe_head);
    // cv::imshow("before merge", staff_result_map);
    // cv::waitKey(0);

    // 五、簡單篩一下，取大量喔！就是找出可能是要找的頭的概念！之後再用特徵篩一次~~
    // normalize( staff_result_map, staff_result_map, 0, 1, NORM_MINMAX, -1, Mat() );  //如果用這個的話就連其他版本的譜好像也可以，thr設0.75
    threshold(staff_result_map, staff_result_map, thresh_hold , 1.0 , CV_THRESH_TOZERO);
    // debug用  把所有找到的地方都框出來，還沒有合併附近很像的地方
    // debug_draw_result_map_on_staff_bin_erase_line(staff_result_map, staff_bin_erase_line, template_img, 0, staff_result_map.cols - 1, 0, staff_result_map.rows - 1, Scalar(0, 0, 255), "before merge");
    // *******************************************************************************************************
    // 把附近找出來一堆很像的的東西合併成一個：
    // 一、先框好範圍，
    // 二、找出框框內最好的點，
    // 三、只留下那個點其他點去掉，
    MaybeHead_MergeCloseHead(staff_result_map, staff_bin_erase_line, template_img);
    // debug用 合併完也看看
    // debug_draw_result_map_on_staff_bin_erase_line(staff_result_map, staff_bin_erase_line, template_img, 0, staff_result_map.cols - 1, 0, staff_result_map.rows - 1, Scalar(0, 0, 255), "after merge");
    // waitKey(0);
    
    // *******************************************************************************************************
    // 自己設資料結構 移到外面去囉
    //    int maybe_head_count = 0;
    //    float maybe_head[3][200];
    //    for(int i = 0; i < 3; i++)
    //        for(int j = 0; j < 200; j++)
    //            maybe_head[i][j] = 0;
    // 四、把可能是頭的點存進我的data structure，改寫from SHOW START 沒有用我的資料結構 //
    int far_from_staff_limit = 65;  // 如果離五線譜太遠(超過 far_from_staff_limit) 也不存
    for(int go_row = 0; go_row < staff_result_map.rows; go_row++){
        for(int go_col = 0; go_col < staff_result_map.cols; go_col++){
            if( (staff_result_map.at<float>(go_row, go_col) ) &&
                (go_row - pitch_base_y >= -1 * far_from_staff_limit) && (go_row - pitch_base_y <= 50 + far_from_staff_limit) ){  
                maybe_head[0][maybe_head_count] = go_col;  // x
                maybe_head[1][maybe_head_count] = go_row;  // y
                maybe_head[2][maybe_head_count] = staff_result_map.at<float>(go_row, go_col);  // value
                maybe_head_count++;
            }
        }
    }
    // 把可能是頭的點存進我的 data_structure，改寫from SHOW END 沒有用我的資料結構 //
    
    bubbleSort_maybe_head(maybe_head_count, maybe_head,Y_INDEX);
    bubbleSort_maybe_head(maybe_head_count, maybe_head,X_INDEX);
    // imshow("after_merge", temp_show);
    
    // debug整合
    // MaybeHead_draw(temp_show, template_img, maybe_head_count, maybe_head);
    // imshow("MaybeHead_draw", temp_show);
    // waitKey(0);
}

void recognition_1_find_all_MaybeHead(Mat& staff_result_map, Mat template_img, Mat staff_bin_erase_line,
                                       int e_count, int* l_edge, int* distance,
                                       string method, bool debuging){

    // 一、整個大圖片的 staff_result_map 容器
    // 二、加速，看想看的小地方地方即可：根據垂直投影找出來的mountain切
    // 三、對想看的小地方做template_match，結果存在小result
    // 四、把 小結果result 加回去 大圖result_map

    // 一、建立 放原始圖片做樣本比對結果的容器 或 使用外面丟進來的 staff_result_map 承接之前的結果 繼續累加現在的比對的結果
    // 因為是用樣本比對 用 樣本一格格滑過原始影像 計算 每隔往右切往下切樣本大小的影像 與 樣本 比對相似度, 所以容器大小是: 原圖大小 - template大小 + 1
    int cur_template_result_map_row = staff_bin_erase_line.rows - template_img.rows+1;
    int cur_template_result_map_col = staff_bin_erase_line.cols - template_img.cols+1;
    // 如果外面 沒有丟 staff_result_map, 那就自己建立新的 staff_result_map
    if(staff_result_map.empty()){
        staff_result_map = Mat(cur_template_result_map_row, cur_template_result_map_col, CV_32FC1, Scalar(0));
    }
    // 如果外面 有丟 staff_result_map, 使用外面丟進來的 staff_result_map 承接之前的結果 繼續累加現在的比對的結果,
    // 但也要注意 目前的template產生的result_map比較大的話, 要先把 外面丟進來的result_map 換到比較大的容器 才放得下 此次template 產生的 staff_result_map
    // 換句話說,目前的template產生的result_map比較小的話, 就用預設沿用 外面的,
    // 總之, 直接通通都重建一個新容器, 看哪個容器比較大, 新容器寬高各別取大的一方囉
    else{
        cur_template_result_map_row = std::max(cur_template_result_map_row, staff_result_map.rows);
        cur_template_result_map_col = std::max(cur_template_result_map_col, staff_result_map.cols);

        Mat temp(cur_template_result_map_row, cur_template_result_map_col, CV_32FC1, Scalar(0));
        temp(Rect(0, 0, staff_result_map.cols, staff_result_map.rows)) += staff_result_map;
        staff_result_map = temp;
    }
    // cout << "cur_template_result_map_row = " << cur_template_result_map_row << endl;
    // cout << "cur_template_result_map_col = " << cur_template_result_map_col << endl;
    
    // 二、加速，專注於想看的小地方地方即可不用整張圖都看
    int grab_left;
    int grab_width;
    int extend_full;
    int extend_half;
    int right_boundary_index = staff_bin_erase_line.cols - 1;
    // 走訪每座山
    for(int go_mountain = 0; go_mountain < e_count; go_mountain++){
        // 防呆, 如果template的寬度比較大的話, 就把要看的山的距離拉大點囉
        grab_left  = l_edge[go_mountain];
        grab_width = distance[go_mountain];
        if(debuging) cout << "staff_bin_erase_line.cols:" << staff_bin_erase_line.cols << ", template_img.cols:" << template_img.cols << ", grab_left:" << grab_left << ", grab_width:" << grab_width << endl;
        if (template_img.cols > grab_width){
            // 往左右各延伸 extend_full 來把 山的距離拉到足夠大, 要補的隔數 如果是 偶數格左右可以拉得剛剛好, 基數格 往右補(給 grab_width)
            extend_full = (template_img.cols - grab_width) / 2;
            extend_half = (template_img.cols - grab_width) % 2;
            grab_left  -= extend_full;
            grab_width += extend_full * 2 + extend_half;
            // 如果往左補 超出範圍, 往左補超過範圍的部分 丟給右邊
            if(grab_left < 0){
                grab_width += grab_width + std::abs(grab_left);
                grab_left   = 0;
            }
            // 如果往右補 超出範圍, 往右補超過範圍的部分 丟給左邊
            if(grab_left + grab_width > right_boundary_index){
                grab_left  -= (grab_left + grab_width) - (right_boundary_index);  // 往右補的目的地 - 右邊界 == 往右補超過的部分
                grab_width  = (right_boundary_index) - grab_left;                 // 現在就是 右邊界 當作目的地, 用此來重新計算 grab_width
            }
        }
        if(debuging) cout << "cur_template_result_map_col:" << cur_template_result_map_col << ", template_img.cols:" << template_img.cols << ", grab_left:" << grab_left << ", grab_width:" << grab_width << endl << endl;

        // 二、根據上面防呆後的垂直投影找出來的mountain 根據 左邊界 和 distance 來切圖, 因為 很多符號 都是 瘦瘦高高, 所以 只對寬度仔細切, 高度抓全部 比較安全
        Mat proc_img = staff_bin_erase_line(Rect(grab_left, 0, grab_width, staff_bin_erase_line.rows ));

        // 三前置、 建立 放 根據垂直投影切出來的影像做樣本比對結果的容器，根據垂直投影找出來的mountain切, 所以容器大小是: 山圖的大小 - template大小 + 1
        int result_row = staff_bin_erase_line.rows - template_img.rows +1;
        int result_col = grab_width     - template_img.cols +1;
        // cout << "result_row = " << result_row << endl;
        // cout << "result_col = " << result_col << endl;
        Mat result(result_row, result_col, CV_32FC1);

        // 三、 每座山圖 做樣本比對
        if(method == "method1") matchTemplate (proc_img, template_img, result, CV_TM_CCOEFF_NORMED);
        else                    matchTemplate2(proc_img, template_img, result);
        // threshold(result, result, 0.5, 1., CV_THRESH_TOZERO);

        // 四、 山圖樣本比對的結果圖 根據 左邊界 加回 原始影像比對的結果圖 相應的位置
        staff_result_map( Rect(grab_left, 0, result_col, result_row) ) += result;

        // imshow("proc_img", proc_img);
        // imshow("staff_result_map", staff_result_map);
        // cv::imshow("template_img", template_img);
        // cout << result << endl << endl << endl;
        // waitKey(0);
    }
}
