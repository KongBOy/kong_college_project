#include <opencv2/core/core_c.h>
#include <string>
using namespace std;
using namespace cv;
#ifndef BAR_ROI_H_INCLUDED
#define BAR_ROI_H_INCLUDED



#endif // BAR_ROI_H_INCLUDED

void on_Roi(int, void * data);
void bar_Roi(Mat & dst_img, string window_name, bool debuging=false);
