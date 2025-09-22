#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void MaybeHead_list_infos(int maybe_head_count,float maybe_head[][200]){
    cout<<"maybe_head_count = "<<maybe_head_count<<endl;
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
        cout<<"go_head = "<<go_head
            <<" , x = "   <<maybe_head[0][go_head]
            <<" , y = "   <<maybe_head[1][go_head]
            <<" , similar = "<<maybe_head[2][go_head]
            <<endl;
    cout<<endl;
}

void MaybeHead_draw(Mat& debug_img , Mat template_img,int maybe_head_count, float maybe_head[][200], float th1, float th2, float th3){
    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
        /// 一、無限大 ~ th1
        /// 二、th1 ~ th2
        /// 三、th2 ~ th3
        /// 四、th3 ~ 無限小
        // 藍色
        if     (maybe_head[2][go_head] >= th1                               ) rectangle( debug_img, Point(maybe_head[0][go_head],maybe_head[1][go_head]), Point( maybe_head[0][go_head] + template_img.cols ,maybe_head[1][go_head] + template_img.rows ), Scalar(255,0,0), 1, 8, 0 );
        // 綠色
        else if(maybe_head[2][go_head] <th1 && maybe_head[2][go_head] >= th2) rectangle( debug_img, Point(maybe_head[0][go_head],maybe_head[1][go_head]), Point( maybe_head[0][go_head] + template_img.cols ,maybe_head[1][go_head] + template_img.rows ), Scalar(0,255,0), 1, 8, 0 );
        // 紅色
        else if(maybe_head[2][go_head] <th2 && maybe_head[2][go_head] >= th3) rectangle( debug_img, Point(maybe_head[0][go_head],maybe_head[1][go_head]), Point( maybe_head[0][go_head] + template_img.cols ,maybe_head[1][go_head] + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
        // 橘色粗框
        else rectangle( debug_img, Point(maybe_head[0][go_head],maybe_head[1][go_head]), Point( maybe_head[0][go_head] + template_img.cols ,maybe_head[1][go_head] + template_img.rows ), Scalar(50,150,255), 2, 8, 0 );
    }
}
