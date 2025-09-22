#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void MaybeHead_list_infos(int maybe_head_count, float maybe_head[][200]);
void MaybeHead_draw(Mat& debug_img, Mat template_img, int maybe_head_count, float maybe_head[][200], float th1 = 0.7, float th2 = 0.49, float th3 = 0.43);