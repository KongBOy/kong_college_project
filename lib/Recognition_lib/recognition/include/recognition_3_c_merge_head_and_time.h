#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#ifndef RECOGNITION_4_MERGE_HEAD_AND_TIME_H_INCLUDED
#define RECOGNITION_4_MERGE_HEAD_AND_TIME_H_INCLUDED
#endif // RECOGNITION_4_MERGE_HEAD_AND_TIME_H_INCLUDED

using namespace std;
using namespace cv;

void recognition_4_merge_head_and_time(int head_type,
                                       Mat template_img , Mat reduce_line,
                                       int maybe_head_count , float maybe_head[][200],
                                       int bars_count , short bars[][200],bool bars_dir[200],int bars_time[200],
                                       int& note_count , int note[][1000],
                                       bool debuging = false);
