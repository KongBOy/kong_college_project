#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


#ifndef RECOGNITIOIN_2_B_HEAD_RECHECK_H_INCLUDED
#define RECOGNITIOIN_2_B_HEAD_RECHECK_H_INCLUDED
#endif // RECOGNITIOIN_2_B_HEAD_RECHECK_H_INCLUDED
void recognition_2_b_head_recheck(int head_type,
                                  Mat MaybeHead_final_template,
                                  Mat reduce_line,
                                  int& maybe_head_count,float maybe_head[][200],
                                  bool debuging=true);

///
///static void matchTemplate2(Mat src_img,Mat template_test,Mat& result)
