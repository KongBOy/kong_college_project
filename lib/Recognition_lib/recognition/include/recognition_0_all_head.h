#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>



#ifndef RECOGNITION_0_ALL_HEAD_H_INCLUDED
#define RECOGNITION_0_ALL_HEAD_H_INCLUDED
#endif // RECOGNITION_0_ALL_HEAD_H_INCLUDED

using namespace cv;
using namespace std;

void recognition_0_all_head( int head_type,
                             Mat staff_img_erase_line,    /// 消掉五線譜線的圖
                             Mat staff_img,
                             int e_count, int* l_edge, int* distance, ///從 recognition_0 的資料來 加速
                             int pitch_base_y,
                             int& note_count,
                             int note[][1000],
                             bool debuging);
