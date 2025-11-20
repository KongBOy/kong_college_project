#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

void recognition_3_a_find_vertical_bar(Mat template_img ,
                                     Mat reduce_line,
                                     int& maybe_head_count , float maybe_head[][200],
                                     int& bars_count , short bars[][200] , bool bars_dir[200],
                                     bool debuging=false);
