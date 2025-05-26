#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#ifndef RECOGNITIOIN_3_B_FIND_TIME_BAR_H_INCLUDED
#define RECOGNITIOIN_3_B_FIND_TIME_BAR_H_INCLUDED


#endif // RECOGNITIOIN_3_B_FIND_TIME_BAR_H_INCLUDED

void recognition_3_b_find_time_bar(Mat template_img,Mat reduce_line,
                                   int lines_count,short lines[][200],bool lines_dir[][200],
                                   int lines_time[200]);
///
///int find_lines_time( Mat reduce_line ,int line_x,int line_y,int left , int right ,int test_depth , bool direction ,Mat & debug)

