#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void FadeInOut(string Title, Mat Inp,Mat Out,int delay);

int Drawing_Random_Circles(Mat& image);

// Input圖去白色背景(200以上) 貼進 Output圖
int DrawTalk2(Mat Input, Mat& Output, int row, int col);