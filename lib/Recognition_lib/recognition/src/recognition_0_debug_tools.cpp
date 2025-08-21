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

void list_bars_info(int bars_count,short bars[][200],bool bars_dir[200]){
    cout << "bars_count = " << bars_count<<endl;
    for(int go_bar = 0 ; go_bar < bars_count ; go_bar++)
        cout << "go_bar = "   << go_bar
             << " , x = "      << bars[0][go_bar]
             << " , y = "      << bars[1][go_bar]
             << " , length = " << bars[2][go_bar]
             << " , down = "   << bars_dir[go_bar]
             << " , top = "    << bars_dir[go_bar] << endl;
    cout << endl;
}

void draw_bars(Mat& debug_img,int bars_count,short bars[][200],bool bars_dir[200]){
    // 顯示 線
    for(int go_bar = 0 ; go_bar < bars_count ; go_bar++){
        // 左, TOPTODOWN
        if(bars_dir[go_bar] == true){
            line(debug_img, Point(bars[0][go_bar],bars[1][go_bar]),Point(bars[0][go_bar],bars[1][go_bar]+bars[2][go_bar]),Scalar(123,255,123),2 );  // 淺綠
        }
        // 右, DOWNTOTOP
        else if(bars_dir[go_bar] == false ){
            line(debug_img, Point(bars[0][go_bar],bars[1][go_bar]),Point(bars[0][go_bar],bars[1][go_bar]-bars[2][go_bar]),Scalar(23,255,223),2 );  // 淺黃綠
        }

        // cout<<"go_bar = "<<go_bar<<" , x = "<<bars[0][go_bar]<<" , y = "<<bars[1][go_bar]<<" , length = "<<bars[2][go_bar]<<" , left = "<<bars_dir[go_bar]<<" , right = "<<bars_dir[go_bar]<<endl;
        // imshow("bars",debug_img);

        // waitKey(0); ///一條一條慢慢看
    }
}


void watch_bars(Mat debug_img,int bars_count,short bars[][200],bool bars_dir[200]){
    // 顯示 線
    for(int go_bar = 0 ; go_bar < bars_count ; go_bar++){
        // 左
        if(bars_dir[go_bar] == true){
            line(debug_img, Point(bars[0][go_bar], bars[1][go_bar]), Point(bars[0][go_bar], bars[1][go_bar]+bars[2][go_bar]), Scalar(123,255,123), 2);
        }
        // 右
        else if(bars_dir[go_bar] == false ){
            line(debug_img, Point(bars[0][go_bar], bars[1][go_bar]), Point(bars[0][go_bar], bars[1][go_bar]+bars[2][go_bar]), Scalar(23,255,223), 2);
        }

        // cout<<"go_bar = "<<go_bar<<" , x = "<<bars[0][go_bar]<<" , y = "<<bars[1][go_bar]<<" , length = "<<bars[2][go_bar]<<" , left = "<<bars_dir[go_bar]<<" , right = "<<bars_dir[go_bar] << endl;
        imshow("bars", debug_img);
        waitKey(0);  // 一條一條慢慢看
    }

    imshow("watch_bars()", debug_img);
    waitKey(0);
}
