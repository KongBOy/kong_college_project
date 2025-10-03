#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#ifndef RECOGNITIOIN_2_A_HEAD_CHARACTARISTIC_H_INCLUDED
#define RECOGNITIOIN_2_A_HEAD_CHARACTARISTIC_H_INCLUDED

#endif // RECOGNITIOIN_2_A_HEAD_CHARACTARISTIC_H_INCLUDED


using namespace cv;

void recognition_2_a_head_charactristic(int head_type,
                                        Mat template_img,
                                        Mat staff_bin_erase_line,
                                        Mat cut_ord_img,
                                        int& maybe_head_count,float maybe_head[][200],
                                        bool debuging=false);
/// cpp 要記得分case喔！！

