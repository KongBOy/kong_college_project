/*
這個程式的功能是：
可以把找到的note資訊顯示出來.
    list：列出note資訊
    watch：把note 顯示 在輸入圖上 (不改變輸入圖)
    draw ：把note  畫  在輸入圖上 (會改變輸入圖)
*/

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_0_debug_tools.h"

using namespace std;
using namespace cv;

/*
static Mat template_img_4 = imread("Resource/note/4/4.bmp",0);
static Mat template_img_5 = imread("Resource/note/4-rest/4-rest.bmp",0);
static Mat template_img_2 = imread("Resource/note/2/2.bmp",0);
static Mat template_img_0 = imread("Resource/note/0/0.bmp",0);
static Mat template_img_1 = imread("Resource/note/0-rest/0-rest.bmp",0);
static Mat template_img_3 = imread("Resource/note/2-rest/2-rest.bmp",0);
static Mat template_img_8 = imread("Resource/note/8-rest/8-rest-3.bmp",0);
static Mat template_img_6 = imread("Resource/note/6-rest/6-rest-2.bmp",0);
static Mat template_img_7 = imread("Resource/note/32-rest/7-2.bmp",0);
*/

void list_lines_info(int lines_count,short lines[][200],bool lines_dir[][200]){
    cout << "lines_count = " << lines_count<<endl;
    for(int go_line = 0 ; go_line < lines_count ; go_line++)
        cout << "go_line = "   << go_line
             << " , x = "      << lines[0][go_line]
             << " , y = "      << lines[1][go_line]
             << " , length = " << lines[2][go_line]
             << " , down = "   << lines_dir[0][go_line]
             << " , top = "    << lines_dir[1][go_line] << endl;
    cout << endl;
}

void draw_lines(Mat& debug_img,int lines_count,short lines[][200],bool lines_dir[][200]){
    // 顯示 線
    for(int go_line = 0 ; go_line < lines_count ; go_line++){
        // 左
        if(lines_dir[0][go_line] == true && lines_dir[1][go_line] == false){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(123,255,123),2 );
        }
        // 右
        else if(lines_dir[0][go_line] == false && lines_dir[1][go_line] == true){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(23,255,223),2 );
        }
        // 右+左
        else if((lines_dir[0][go_line] == true && lines_dir[1][go_line] == true) || (lines_dir[0][go_line] == false && lines_dir[1][go_line] == false)){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(0,0,255),2 );
        }

        // cout<<"go_line = "<<go_line<<" , x = "<<lines[0][go_line]<<" , y = "<<lines[1][go_line]<<" , length = "<<lines[2][go_line]<<" , left = "<<lines_dir[0][go_line]<<" , right = "<<lines_dir[1][go_line]<<endl;
        // imshow("lines",debug_img);

        // waitKey(0); ///一條一條慢慢看
    }
}


void watch_lines(Mat debug_img,int lines_count,short lines[][200],bool lines_dir[][200]){
    // 顯示 線
    for(int go_line = 0 ; go_line < lines_count ; go_line++){
        // 左
        if(lines_dir[0][go_line] == true && lines_dir[1][go_line] == false){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(123,255,123),2 );
        }
        // 右
        else if(lines_dir[0][go_line] == false && lines_dir[1][go_line] == true){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(23,255,223),2 );
        }
        // 右+左
        else if((lines_dir[0][go_line] == true && lines_dir[1][go_line] == true) || (lines_dir[0][go_line] == false && lines_dir[1][go_line] == false)){
            line(debug_img, Point(lines[0][go_line],lines[1][go_line]),Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(0,0,255),2 );
        }

        // cout<<"go_line = "<<go_line<<" , x = "<<lines[0][go_line]<<" , y = "<<lines[1][go_line]<<" , length = "<<lines[2][go_line]<<" , left = "<<lines_dir[0][go_line]<<" , right = "<<lines_dir[1][go_line]<<endl;
        // imshow("lines",debug_img);

        waitKey(0); ///一條一條慢慢看
    }

    imshow("watch_lines()",debug_img);
    waitKey(0);
}
