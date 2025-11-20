#include <opencv2/core/core_c.h>
#include <string>
using namespace std;
using namespace cv;

void Center_ROI(int, void * data);
void Center_ROI_by_slider(Mat & dst_img, string window_name, bool debuging=false);
