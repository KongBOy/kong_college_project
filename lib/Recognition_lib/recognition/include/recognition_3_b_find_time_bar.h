#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

void recognition_3_b_find_time_bar(Mat template_img,
                                   int bars_count,short bars[][200],bool bars_dir[200],
                                   int bars_time[200],
                                   Mat reduce_line, 
                                   bool debuging = false);
