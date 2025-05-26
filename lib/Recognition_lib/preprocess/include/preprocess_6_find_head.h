#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

using namespace cv;

#ifndef FIND_HEAD_H_INCLUDED
#define FIND_HEAD_H_INCLUDED



#endif // FIND_HEAD_H_INCLUDED





void Watch_Hough_Line2 (vector<Vec2f> lines , Mat  drew_img  ,string window_name);
void Find_Head (vector<Vec2f> lines , Mat drew_img  ,string window_name,Mat bin_src_img,int**& left_point, int**& right_point);

//不要用vector<Vec2f>*& staff，原因寫在find_staff.cpp
void Find_Head_Interface(Mat test_bin,vector<Vec2f> staff_lines , int staff_count, int***& left_point , int***& right_point , Mat color_ord_img);

string Int2str(int in_num); // int Тр string
