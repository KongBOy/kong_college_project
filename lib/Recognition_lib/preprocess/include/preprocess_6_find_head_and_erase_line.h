#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

using namespace cv;

#ifndef FIND_HEAD_H_INCLUDED
#define FIND_HEAD_H_INCLUDED



#endif // FIND_HEAD_H_INCLUDED





void Watch_Hough_Line2 (vector<Vec2f> lines , Mat  drew_img  ,string window_name);
void Find_Head_and_Erase_Line (vector<Vec2f> lines, string window_name, Mat test_bin,int**& left_point, int**& right_point, Mat& src_bin_erase_line, bool debuging);

//不要用vector<Vec2f>*& staff，原因寫在find_staff.cpp
void Find_Head_and_Erase_Line_Interface(Mat test_bin,vector<Vec2f> staff_lines , int staff_count, int***& left_point , int***& right_point , Mat color_ord_img, Mat& src_bin_erase_line, bool debuging);
