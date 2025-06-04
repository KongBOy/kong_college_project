#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>





using namespace cv;


void list_lines_info(int lines_count,short lines[][200],bool lines_dir[][200]);
void draw_lines(Mat& debug_img,int lines_count,short lines[][200],bool lines_dir[][200]);
void watch_lines(Mat debug_img,int lines_count,short lines[][200],bool lines_dir[][200]);
