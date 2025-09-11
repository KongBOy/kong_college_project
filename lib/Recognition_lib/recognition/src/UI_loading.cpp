/*
這個程式的功能是：
系統介面的 進度條
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include <iostream>

using namespace std;
using namespace cv;

Mat screen(500,1270,CV_8UC3,Scalar(255,255,255));
Mat UI2_img = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/UI_bass2.png",1);

int screen_center_x = screen.cols/2;
int screen_center_y = screen.rows/2;

int loading_bar = 0;




static void Perspective_trans(double x,double y,Mat warp_matrix,double & result_x,double& result_y, bool debuging){
    float trans_proc_1[3];

    trans_proc_1[0] = x;
    trans_proc_1[1] = y;
    trans_proc_1[2] = 1;
    // cout<<"_1[0] = "<<trans_proc_1[0]<<" , _1[1] = "<<trans_proc_1[1]<<" , _1[2] = "<<trans_proc_1[2]<<endl;

    float trans_proc_2[3];
    for(int j = 0 ; j < 3 ; j++)
    {
        trans_proc_2[j]  = trans_proc_1[0]*warp_matrix.at<double>(j,0);
        trans_proc_2[j] += trans_proc_1[1]*warp_matrix.at<double>(j,1);
        trans_proc_2[j] += trans_proc_1[2]*warp_matrix.at<double>(j,2);
    }
    if(debuging) cout<<"_2[0] ="<<trans_proc_2[0]<<" , _2[1] ="<<trans_proc_2[1]<<" , _2[2] ="<<trans_proc_2[2]<<endl;

    result_x = (trans_proc_2[0]/trans_proc_2[2]);
    result_y = (trans_proc_2[1]/trans_proc_2[2]);
    if(debuging) cout<<endl;


    if(debuging) cout<< "result_x =" << result_x << ", result_y ="<<result_y<<endl;

}


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
Mat loading_bar_item_4          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_4_green.bmp"      , 1);  // 04,11,15
Mat loading_bar_item_8          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_8_green.bmp"      , 1);  // 01,07
Mat loading_bar_item_16         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_16_green.bmp"     , 1);  // 02,12,14
Mat loading_bar_item_rest_8     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_rest_8_green.bmp" , 1);  // 03,10
Mat loading_bar_item_flat       = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_flat_green.bmp"   , 1);  // 05,08
Mat loading_bar_item_sharp      = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_sharp_green.bmp"  , 1);  // 06,09,13
Mat loading_bar_item_finish     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish_green.bmp" , 1);  // 16
Mat loading_bar_item_staff_line = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish_green.bmp" , 1);  // 16
*/

///讀灰階~~
Mat loading_bar_item_4          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_4.bmp"      , 0);  // 04,11,15
Mat loading_bar_item_8          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_8.bmp"      , 0);  // 01,07
Mat loading_bar_item_16         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_16.bmp"     , 0);  // 02,12,14
Mat loading_bar_item_rest_8     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_rest_8.bmp" , 0);  // 03,10
Mat loading_bar_item_flat       = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_flat.bmp"   , 0);  // 05,08
Mat loading_bar_item_sharp      = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_sharp.bmp"  , 0);  // 06,09,13
Mat loading_bar_item_finish     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp" , 0);  // 16
Mat loading_bar_item_staff_line = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp" , 0);  // 16


// 252 577 staff_line
// 1224 593 loading

// b153 g225 r113
void set_item_img(int position,Mat& setted_img){
    switch(position){
        case 4:
        case 11:
        case 15:{
            setted_img = loading_bar_item_4.clone();
        }
        break;

        case 1:
        case 7:{
            setted_img = loading_bar_item_8.clone();
        }
        break;

        case 2:
        case 12:
        case 14:{
            setted_img = loading_bar_item_16.clone();
        }
        break;

        case 3:
        case 10:{
            setted_img = loading_bar_item_rest_8.clone();
        }
        break;

        case 5:
        case 8:{
            setted_img = loading_bar_item_flat.clone();
        }
        break;

        case 6:
        case 9:
        case 13:{
            setted_img = loading_bar_item_sharp.clone();
        }
        break;

        case 16:{
            setted_img = loading_bar_item_finish.clone();
        }
        break;
    }
}
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

void Show_loading_bar(Mat UI_bass,string UI_WINDOW_NAME,int start_num , int end_num){
    Mat color_load_img;
    Mat gray_load_img;
    int color_load_img_left = 0;
    int color_load_img_top  = 0;

    start_num = set_item_position(start_num);
    end_num   = set_item_position(end_num);


    for(int go_item = start_num ; go_item <= end_num ; go_item++){
        set_item_img(go_item,gray_load_img);
        cvtColor(gray_load_img,color_load_img,CV_GRAY2BGR);
        // set_item_img(go_item,color_load_img);
        // cvtColor(color_load_img,gray_load_img,CV_BGR2GRAY);

        color_load_img_left = loading_item_position[go_item].x;
        color_load_img_top  = loading_item_position[go_item].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if  (gray_load_img.at<uchar>(go_row,go_col) == 255) continue;
                else{
                    UI_bass.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
                    UI2_img.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
                }
            }
        }
        imshow(UI_WINDOW_NAME,UI_bass);
        waitKey(100);
    }
    imshow(UI_WINDOW_NAME,UI_bass);

    if(end_num == 15){
        set_item_img(16,gray_load_img);
        cvtColor(gray_load_img,color_load_img,CV_GRAY2BGR);
        // set_item_img(16,color_load_img);
        // cvtColor(color_load_img,gray_load_img,CV_BGR2GRAY);

        color_load_img_left = loading_item_position[16].x;
        color_load_img_top  = loading_item_position[16].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if(gray_load_img.at<uchar>(go_row,go_col) == 255) continue;
                UI_bass.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
                UI2_img.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
            }
        }
        imshow(UI_WINDOW_NAME,UI_bass);
        waitKey(1000);
    }
}


void UI_loading_preprocess(Mat ord_img,
                Mat test_bin,
                int staff_count, int*** left_point , int*** right_point,
                Mat UI_bass,string UI_WINDOW_NAME,
                bool debuging){
    loading_bar = 0;
    UI2_img = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/UI_bass2.bmp",1);
    UI_bass = UI2_img.clone();
    imshow(UI_WINDOW_NAME,UI_bass);



    int resize_img_height = screen.rows;
    int resize_img_width = ord_img.cols * (screen.rows/(double)ord_img.rows);

    Point2f srcTri[4];
    srcTri[0] = Point2f(0,0);
    srcTri[1] = Point2f(0,ord_img.rows-1);
    srcTri[2] = Point2f(ord_img.cols-1,0);
    srcTri[3] = Point2f(ord_img.cols-1,ord_img.rows-1);

    Point2f dstTri[4];
    dstTri[0] = Point2f( 0,0);
    dstTri[1] = Point2f( 0,resize_img_height-1 );
    dstTri[2] = Point2f( resize_img_width-1,0);
    dstTri[3] = Point2f( resize_img_width-1,resize_img_height-1);


    Mat warp_matrix = getPerspectiveTransform(srcTri,dstTri);

    Mat resize_ord_img(resize_img_height,resize_img_width,ord_img.depth());
    warpPerspective(ord_img,resize_ord_img,warp_matrix, resize_ord_img.size(),0,0,255);

    Mat resize_bin_img = resize_ord_img.clone();
    warpPerspective(test_bin,resize_bin_img,warp_matrix, resize_ord_img.size(),0,0,255);



    Mat color_load_img = resize_ord_img.clone();
    int color_load_img_left = screen_center_x - color_load_img.cols/2;
    int color_load_img_top  = screen_center_y - color_load_img.rows/2;

    // screen(Rect(screen_center_x - load_ord_img.cols/2,0,load_ord_img.cols,load_ord_img.rows)) = color_load_img.clone();


    cvtColor(resize_ord_img,color_load_img,CV_GRAY2BGR);
    for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++)
        for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++)
            // screen.at<Vec3b>(go_row,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
            UI_bass.at<Vec3b>(go_row,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
    imshow(UI_WINDOW_NAME,UI_bass);
    // imshow("screen",screen);
    waitKey(500);


    cvtColor(resize_bin_img,color_load_img,CV_GRAY2BGR);
    for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++)
        for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++)
            // screen.at<Vec3b>(go_row,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
            UI_bass.at<Vec3b>(go_row,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);

    imshow(UI_WINDOW_NAME,UI_bass);
    // imshow("screen",screen);
    waitKey(500);


    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        for(int go_line = 0 ; go_line < 5 ; go_line++){
            double trans_left_x = left_point[go_staff][go_line][0];
            double trans_left_y = left_point[go_staff][go_line][1];
            Perspective_trans(trans_left_x,trans_left_y,warp_matrix,trans_left_x,trans_left_y, debuging);
            trans_left_x += color_load_img_left;


            double trans_right_x = right_point[go_staff][go_line][0];
            double trans_right_y = right_point[go_staff][go_line][1];
            Perspective_trans(trans_right_x,trans_right_y,warp_matrix,trans_right_x,trans_right_y, debuging);
            trans_right_x += color_load_img_left;

            // line(screen,Point(trans_left_x ,trans_left_y ),
            line(UI_bass,Point(trans_left_x ,trans_left_y ),
                         Point(trans_right_x,trans_right_y),
                         Scalar(0,0,255),1);

            imshow(UI_WINDOW_NAME,UI_bass);
            // imshow("screen",screen);
            waitKey(20);
        }
    }
    loading_bar +=30;
    Show_loading_bar(UI_bass,UI_WINDOW_NAME,0,loading_bar);
}




int width_frame_acc = 0;///mod_width;

void UI_loading_recognition_row(int staff_count,Mat staff_img,int row_note_count,int row_note[][1000],
                                Mat UI_bass,string UI_WINDOW_NAME){
    int div_width = 70 / staff_count;
    int mod_width = 70 % staff_count;

    int old_loading_bar = loading_bar;

    UI_bass = UI2_img.clone();
    imshow(UI_WINDOW_NAME,UI_bass);

    Mat color_load_img = staff_img.clone();
    cvtColor(staff_img.clone(),color_load_img,CV_GRAY2BGR);

    int color_load_img_top  = screen_center_y - color_load_img.rows/2;
    int color_load_img_left = screen_center_x - color_load_img.cols/2;
    for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++)
        for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++)
            // screen.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
            UI_bass.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);

        imshow(UI_WINDOW_NAME,UI_bass);
        // imshow("screen",screen);


    for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
        // cout<<"go_note = "<<go_note<<endl;
        Mat template_img;
        Scalar color;
        set_formate(row_note[2][go_row_note],row_note[3][go_row_note],color,template_img);
        // rectangle(screen,Point(row_note[0][go_row_note]+color_load_img_left,row_note[1][go_row_note]+color_load_img_top),
        rectangle(UI_bass,Point(row_note[0][go_row_note]+color_load_img_left,row_note[1][go_row_note]+color_load_img_top),
                          Point(row_note[0][go_row_note]+template_img.cols+color_load_img_left,row_note[1][go_row_note]+template_img.rows+color_load_img_top),color,2);
        imshow(UI_WINDOW_NAME,UI_bass);
        // imshow("screen",screen);
        waitKey(10);

    }

    loading_bar     += div_width;
    width_frame_acc += mod_width;
    if(width_frame_acc/staff_count){
        loading_bar++;
        width_frame_acc %= staff_count;
    }

    cout<<"loading_bar = "<<loading_bar<<" , frame_acc = "<<width_frame_acc<<endl;
    Show_loading_bar(UI_bass,UI_WINDOW_NAME,old_loading_bar,loading_bar);
    waitKey(100);
}



/*
void UI_loading_recognition(int staff_count,Mat staff_img[],int note_count,int note[][1000],int row_note_count_array[],
                            Mat UI_bass,string UI_WINDOW_NAME){
    UI_bass = UI2_img.clone();
    imshow(UI_WINDOW_NAME,UI_bass);


    int go_note = 0;
    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        Mat color_load_img = staff_img[go_staff].clone();
        cvtColor(staff_img[go_staff].clone(),color_load_img,CV_GRAY2BGR);

        int color_load_img_top  = screen_center_y - color_load_img.rows/2;
        int color_load_img_left = screen_center_x - color_load_img.cols/2;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++)
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++)
                // screen.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);
                UI_bass.at<Vec3b>(go_row+color_load_img_top,go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row,go_col);

        imshow(UI_WINDOW_NAME,UI_bass);
        // imshow("screen",screen);

        // cout<<"row_note_count_array["<<go_staff<<"] = "<<row_note_count_array[go_staff]<<endl;


        for(int go_row_note = 0 ; go_row_note < row_note_count_array[go_staff] ; go_row_note++){
            // cout<<"go_note = "<<go_note<<endl;
            Mat template_img;
            Scalar color;
            set_formate(note[2][go_note],note[3][go_note],color,template_img);
            // rectangle(screen,Point(note[0][go_note]+color_load_img_left,note[1][go_note]+color_load_img_top),
            rectangle(UI_bass,Point(note[0][go_note]+color_load_img_left,note[1][go_note]+color_load_img_top),
                             Point(note[0][go_note]+template_img.cols+color_load_img_left,note[1][go_note]+template_img.rows+color_load_img_top),color,2);

            imshow(UI_WINDOW_NAME,UI_bass);
            ///imshow("screen",screen);
            waitKey(10);
            go_note++;
        }
        waitKey(500);
    }
}
*/
