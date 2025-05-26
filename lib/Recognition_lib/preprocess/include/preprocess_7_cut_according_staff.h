#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


void Cut_staff(Mat test_bin,Mat reduce_line_bin,
               int& staff_count,int*** left_point, int*** right_point,
               Mat final_rl_img_roi[],Mat final_img_roi[],
               double trans_start_point_x[],double trans_start_point_y[]);

#ifndef CUT_ACCORDING_STAFF_H_INCLUDED
#define CUT_ACCORDING_STAFF_H_INCLUDED



#endif // CUT_ACCORDING_STAFF_H_INCLUDED
