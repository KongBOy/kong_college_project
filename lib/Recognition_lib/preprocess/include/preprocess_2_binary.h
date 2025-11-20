#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void Binary_by_patch(Mat & dst, const int div_row, const int div_col, bool debuging=false); //切小塊，再做二值化
unsigned char Binary(Mat & dst);
//第一個參數：想要二值化的原圖
//第二個參數：二值化結果存的容器
//方法：
//步驟一：先用canny找出輪廓
//步驟二：找出note的顏色

//方法二：方法一的延伸，可以想像把方法一切的極小塊
void test_Binary_by_Canny(Mat);
