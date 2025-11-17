/*
這個程式的功能是：
系統介面的 進度條
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Bar_tool.h"
#include "Note_infos.h"
#include <iostream>

using namespace std;
using namespace cv;


// 進度條
int loading_bar = 0;
int shift_x = -80;
Point loading_item_position[17] ={
    Point(shift_x +  200, 527), // 00 treble_clef
    Point(shift_x +  270, 539), // 01 8
    Point(shift_x +  344, 533), // 02 16
    Point(shift_x +  442, 564), // 03 rest_8
    Point(shift_x +  484, 531), // 04 4
    Point(shift_x +  564, 543), // 05 flat
    Point(shift_x +  613, 543), // 06 sharp
    Point(shift_x +  654, 544), // 07 8
    Point(shift_x +  733, 543), // 08 flat
    Point(shift_x +  784, 543), // 09 sharp
    Point(shift_x +  839, 570), // 10 rest_8
    Point(shift_x +  875, 538), // 11 4
    Point(shift_x +  940, 538), // 12 16
    Point(shift_x + 1011, 543), // 13 sharp
    Point(shift_x + 1057, 533), // 14 16
    Point(shift_x + 1120, 535), // 15 4
    Point(shift_x + 1209, 539)  // 16  finish
};

///讀彩色~~背景綠色，現在用下面的喔~~
/*
Mat loading_bar_item_4          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_4_green.bmp"     , 1);  // 04, 11, 15
Mat loading_bar_item_8          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_8_green.bmp"     , 1);  // 01, 07
Mat loading_bar_item_16         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_16_green.bmp"    , 1);  // 02, 12, 14
Mat loading_bar_item_rest_8     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_rest_8_green.bmp", 1);  // 03, 10
Mat loading_bar_item_flat       = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_flat_green.bmp"  , 1);  // 05, 08
Mat loading_bar_item_sharp      = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_sharp_green.bmp" , 1);  // 06, 09, 13
Mat loading_bar_item_finish     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish_green.bmp", 1);  // 16
Mat loading_bar_item_staff_line = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish_green.bmp", 1);  // 16
*/

// 進度條
///讀灰階~~
Mat loading_bar_item_4          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_4.bmp"     , 0);  // 04, 11, 15
Mat loading_bar_item_8          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_8.bmp"     , 0);  // 01, 07
Mat loading_bar_item_16         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_16.bmp"    , 0);  // 02, 12, 14
Mat loading_bar_item_rest_8     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_rest_8.bmp", 0);  // 03, 10
Mat loading_bar_item_flat       = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_flat.bmp"  , 0);  // 05, 08
Mat loading_bar_item_sharp      = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_sharp.bmp" , 0);  // 06, 09, 13
Mat loading_bar_item_finish     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp", 0);  // 16
Mat loading_bar_item_staff_line = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp", 0);  // 16

// 進度條
// 252 577 staff_line
// 1224 593 loading

// b153 g225 r113
void set_item_img(int position, Mat& setted_img){
    switch(position){
        case 4:
        case 11:
        case 15:
            setted_img = loading_bar_item_4.clone();
            break;
        case 1:
        case 7:
            setted_img = loading_bar_item_8.clone();
            break;
        case 2:
        case 12:
        case 14:
            setted_img = loading_bar_item_16.clone();
            break;
        case 3:
        case 10:
            setted_img = loading_bar_item_rest_8.clone();
            break;
        case 5:
        case 8:
            setted_img = loading_bar_item_flat.clone();
            break;
        case 6:
        case 9:
        case 13:
            setted_img = loading_bar_item_sharp.clone();
            break;
        case 16:
            setted_img = loading_bar_item_finish.clone();
            break;
    }
}
// 進度條
int set_item_position(int in_num){
         if (in_num >=   0 && in_num <=  5) return  0;
    else if (in_num >=   6 && in_num <= 12) return  1;
    else if (in_num >=  13 && in_num <= 19) return  2;
    else if (in_num >=  20 && in_num <= 25) return  3;
    else if (in_num >=  26 && in_num <= 32) return  4;
    else if (in_num >=  33 && in_num <= 39) return  5;
    else if (in_num >=  40 && in_num <= 45) return  6;
    else if (in_num >=  46 && in_num <= 52) return  7;
    else if (in_num >=  53 && in_num <= 59) return  8;
    else if (in_num >=  60 && in_num <= 65) return  9;
    else if (in_num >=  66 && in_num <= 72) return 10;
    else if (in_num >=  73 && in_num <= 79) return 11;
    else if (in_num >=  80 && in_num <= 85) return 12;
    else if (in_num >=  86 && in_num <= 92) return 13;
    else if (in_num >=  93 && in_num <= 99) return 14;
    else if (in_num >= 100 && in_num <=100) return 15;
}

// 進度條
void Show_loading_bar(Mat UI_bass, string UI_WINDOW_NAME, int start_num, int end_num){
    Mat color_load_img;
    Mat gray_load_img;
    int color_load_img_left = 0;
    int color_load_img_top  = 0;

    start_num = set_item_position(start_num);
    end_num   = set_item_position(end_num);


    for(int go_item = start_num; go_item <= end_num; go_item++){
        set_item_img(go_item, gray_load_img);
        cvtColor(gray_load_img, color_load_img, CV_GRAY2BGR);
        // set_item_img(go_item, color_load_img);
        // cvtColor(color_load_img, gray_load_img, CV_BGR2GRAY);

        color_load_img_left = loading_item_position[go_item].x;
        color_load_img_top  = loading_item_position[go_item].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if  (gray_load_img.at<uchar>(go_row, go_col) == 255) continue;
                else{
                    UI_bass.at<Vec3b>(go_row+color_load_img_top, go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row, go_col);
                }
            }
        }
        imshow(UI_WINDOW_NAME, UI_bass);
        waitKey(100);
    }
    imshow(UI_WINDOW_NAME, UI_bass);

    if(end_num == 15){
        set_item_img(16, gray_load_img);
        cvtColor(gray_load_img, color_load_img, CV_GRAY2BGR);
        // set_item_img(16, color_load_img);
        // cvtColor(color_load_img, gray_load_img, CV_BGR2GRAY);

        color_load_img_left = loading_item_position[16].x;
        color_load_img_top  = loading_item_position[16].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if(gray_load_img.at<uchar>(go_row, go_col) == 255) continue;
                UI_bass.at<Vec3b>(go_row+color_load_img_top, go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row, go_col);
            }
        }
        imshow(UI_WINDOW_NAME, UI_bass);
        waitKey(1000);
    }
}


void UI_loading_preprocess(Mat src_img, 
                           Mat bin_img, 
                           int staff_count, int*** left_point, int*** right_point, 
                           Mat& UI_bass, string UI_WINDOW_NAME, 
                           bool debuging){
    // 進度條預設0
    loading_bar = 0;  

    // 讀出 UI圖片
    UI_bass = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/UI_bass2.bmp", 1);  // 空的進度條圖片
    imshow(UI_WINDOW_NAME, UI_bass);

    // 計算 UI畫面正中心x
    int UI_center_x = UI_bass.cols / 2.;
    
    // src_img 縮放成 show_bin_windows 的 ratio
    int show_bin_window_height = 500;
    double resize_ratio = show_bin_window_height / (double)src_img.rows;
    // 縮放後的 width, height 計算
    int resize_height = show_bin_window_height;
    int resize_width  = src_img.cols * resize_ratio;

    // src_img, bin_img 做縮放 成 resize_ord_img, 
    Mat resize_ord_img;
    cv::resize(src_img, resize_ord_img, cv::Size(resize_width, resize_height));
    
    Mat resize_bin_img;
    cv::resize(bin_img, resize_bin_img, cv::Size(resize_width, resize_height));


    // 計算 縮放後的 src_img, bin_img 左邊界, 上邊界
    int resize_bin_left = UI_center_x - resize_width  / 2;

    // 定位出 resize_ord_img, reisze_bin_img 要顯示在 UI 的哪裡
    Mat show_pre_roi = UI_bass( Rect(resize_bin_left, 0, resize_ord_img.cols, resize_ord_img.rows) );
    Mat show_pre_roi_back = show_pre_roi.clone();  // 先把這個roi區塊原始狀態存下來, 在結束時 可以把這個區塊 恢復原狀
    
    // 把 resize_ord_img 從 gray 轉成 RGB 後 貼在 定好的位置
    cvtColor(resize_ord_img, resize_ord_img, CV_GRAY2BGR);
    resize_ord_img.copyTo( show_pre_roi );
    imshow(UI_WINDOW_NAME, UI_bass);
    waitKey(500);
    
    // 把 resize_bin_img 從 gray 轉成 RGB 後 貼在 定好的位置
    cvtColor(resize_bin_img, resize_bin_img, CV_GRAY2BGR);
    resize_bin_img.copyTo( show_pre_roi );
    imshow(UI_WINDOW_NAME, UI_bass);
    waitKey(500);

    // 把 五線譜的頭 也做 縮放 和 平移 後 顯示出來
    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        for(int go_line = 0 ; go_line < 5 ; go_line++){
            double trans_left_x = left_point[go_staff][go_line][0] * resize_ratio;
            double trans_left_y = left_point[go_staff][go_line][1] * resize_ratio;
            trans_left_x += resize_bin_left;


            double trans_right_x = right_point[go_staff][go_line][0] * resize_ratio;
            double trans_right_y = right_point[go_staff][go_line][1] * resize_ratio;
            trans_right_x += resize_bin_left;

            line(UI_bass, Point(trans_left_x , trans_left_y ), 
                          Point(trans_right_x, trans_right_y), 
                          Scalar(0, 0, 255), 1);

            imshow(UI_WINDOW_NAME, UI_bass);
            waitKey(20);
        }
    }

    // 進度條 30%
    loading_bar +=30;
    Show_loading_bar(UI_bass, UI_WINDOW_NAME, 0, loading_bar);
    // roi區塊恢復原始狀態
    show_pre_roi_back.copyTo(show_pre_roi);
}




int width_frame_acc = 0;  // mod_width;

void UI_loading_recognition_row(int staff_count, Mat staff_img, int row_note_count, int row_note[][1000], 
                                Mat& UI_bass, string UI_WINDOW_NAME, bool debuging){
    // 計算 UI畫面上方正白色區域中心
    int UI_center_x = UI_bass.cols / 2.;
    int UI_center_y = 500 / 2.;
    
    // staff_img 轉成 RGB
    Mat staff_img_color;
    cvtColor(staff_img.clone(), staff_img_color, CV_GRAY2BGR);

    // 定位出 resize_ord_img, reisze_bin_img 要顯示在 UI 的哪裡
    int show_staff_top  = UI_center_y - staff_img_color.rows / 2;
    int show_staff_left = UI_center_x - staff_img_color.cols / 2;
    Mat show_staff_roi = UI_bass( Rect(show_staff_left, show_staff_top, staff_img_color.cols, staff_img_color.rows) );
    staff_img_color.copyTo(show_staff_roi);
    imshow(UI_WINDOW_NAME, UI_bass);

    // 標出辨識完的note
    Draw_note(UI_WINDOW_NAME, UI_bass, row_note_count, row_note, show_staff_left, show_staff_top);

    // 計算 一組五線譜 相當於 進度條 幾%
    int div_width = 70 / staff_count;
    int mod_width = 70 % staff_count;
    
    // 上次的進度先存一份下來
    int old_loading_bar = loading_bar;
    // 更新進度
    loading_bar     += div_width;
    width_frame_acc += mod_width;
    if(width_frame_acc / staff_count){
        loading_bar++;
        width_frame_acc %= staff_count;
    }
    if(debuging) cout<<"old_loading_bar:" << old_loading_bar << ", loading_bar:" << loading_bar << ", width_frame_acc:" << width_frame_acc << ", div_width:" << div_width << ", mod_width:" << mod_width << endl;

    // 更新一組五線譜組的進度條
    Show_loading_bar(UI_bass, UI_WINDOW_NAME, old_loading_bar, loading_bar);
    waitKey(100);
}
