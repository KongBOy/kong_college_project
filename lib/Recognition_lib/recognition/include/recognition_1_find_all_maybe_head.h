#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#ifndef RECOGNITIOIN_1_FIND_ALL_MAYBE_HEAD_H_INCLUDED
#define RECOGNITIOIN_1_FIND_ALL_MAYBE_HEAD_H_INCLUDED



#endif // RECOGNITIOIN_1_FIND_ALL_MAYBE_HEAD_H_INCLUDED


/// input：reduceline以後的圖片
/// 原本寫在cut_according_staff 的 function：
///     Vertical_map_to_recognize(staff_img_erase_line,vertical_map,staff_img,trans[0].y);

using namespace cv;

void recognition_1_find_all_maybe_head(Mat template_img,   /// 要比對的樣板
                                       Mat reduce_line,    /// 消掉五線譜線的圖
                                       int e_count, int* l_edge, int* distance, ///從 recognition_0 的資料來 加速
                                       int& maybe_head_count,float maybe_head[][200],
                                       int pitch_base_y);
