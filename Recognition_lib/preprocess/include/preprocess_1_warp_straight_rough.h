#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#ifndef WARP_STRAIGHT_ROUGH_H_INCLUDED
#define WARP_STRAIGHT_ROUGH_H_INCLUDED



#endif // WARP_STRAIGHT_ROUGH_H_INCLUDED


void bubbleSort(int,vector<Vec2f> &);  //找角度的時候會用到
double Find_Angle(Mat);     //找需要轉正的角度，參數放"要轉正的圖片"，回傳要"轉正的角度(度度量)"
void do_HoughLine(Mat,vector<Vec2f> &, float); //做霍夫線轉換，第一個參數放 圖片，第二個參數丟 "放找到的線的容器"，第三個參數丟 一個自己找到適當的常數
void Wrap_Straight(Mat &, double); //轉正，第一個參數放"要轉正的圖片"，第二個放 Find_Angle() 找到的角度
