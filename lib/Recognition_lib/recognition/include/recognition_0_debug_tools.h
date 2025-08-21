#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>





using namespace cv;


void list_bars_info(int bars_count,short bars[][200],bool bars_dir[200]);
void draw_bars(Mat& debug_img,int bars_count,short bars[][200],bool bars_dir[200]);
void watch_bars(Mat debug_img,int bars_count,short bars[][200],bool bars_dir[200]);
