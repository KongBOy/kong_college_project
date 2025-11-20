#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


void Cut_staff(Mat src_bin, Mat src_bin_erase_line,
               int& staff_count, int*** left_point, int*** right_point,
               Mat staff_img_erase_line[], Mat staff_img[],
               double trans_start_point_x[], double trans_start_point_y[],
               bool debuging=false);
