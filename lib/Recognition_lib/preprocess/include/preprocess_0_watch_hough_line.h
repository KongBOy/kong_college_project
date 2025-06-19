#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


#ifndef WATCH_HOUGH_LINE_H_INCLUDED
#define WATCH_HOUGH_LINE_H_INCLUDED



#endif // WATCH_HOUGH_LINE_H_INCLUDED


void Watch_Hough_Line_Shift(vector<Vec4f>, Mat,string,string);
//void Watch_Hough_Line_Shift_Staff(vector<Vec5f>, Mat,string,string);

void Watch_Hough_Line(vector<Vec2f>, const Mat & = Mat(), string="", string=""); //把 do_HoughLine()後得到的 line容器丟進去 和 找該line容器的圖片進去，可以顯示找到的線長什麼樣子
void Watch_Hough_roh(vector<Vec2f>, Mat&);
void Watch_Hough_P_Line(vector<Vec4i>, Mat,string,string);//2015/11/01 更新
