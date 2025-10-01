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

#include "Bar_tool.h"

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

void list_Bars_infos(int bars_count,short bars[][200],bool bars_dir[200]){
    // 文字 show 資訊
    cout << "bars_count = " << bars_count<<endl;
    for(int go_bar = 0; go_bar < bars_count; go_bar++)
        cout << "go_bar="      << go_bar
             << ", x="         << bars[0][go_bar]
             << ", y="         << bars[1][go_bar]
             << ", length="    << bars[2][go_bar]
             << ", direction=" << bars_dir[go_bar] << endl;
    cout << endl;
}

void draw_bars(Mat& debug_img, int bars_count, short bars[][200], bool bars_dir[200], bool show){
    // 顯示 線
    for(int go_bar = 0 ; go_bar < bars_count ; go_bar++){
        // TOPTODOWN (通常在head左下角從head往下延伸)
        if     (bars_dir[go_bar] == true  ) line(debug_img, Point(bars[0][go_bar], bars[1][go_bar]), Point(bars[0][go_bar], bars[1][go_bar] + bars[2][go_bar]), Scalar(123, 255, 123), 2 );  // 淺綠
        // DOWNTOTOP (通常在head右上角從head往上延伸)
        else if(bars_dir[go_bar] == false ) line(debug_img, Point(bars[0][go_bar], bars[1][go_bar]), Point(bars[0][go_bar], bars[1][go_bar] - bars[2][go_bar]), Scalar( 23, 255, 223), 2 );  // 淺黃綠

        // 如果要一條一條慢慢看 可以把註解拿掉
        // if(show){
        //     imshow("bars", debug_img);
        //     waitKey(0);
        // }
    }
    if(show){
        imshow("watch bars", debug_img);
        waitKey(0);
    }
}

