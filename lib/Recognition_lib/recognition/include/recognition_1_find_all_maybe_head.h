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

void recognition_1_find_all_MaybeHead(Mat& staff_result_map,
                                       Mat template_img,   /// 要比對的樣板
                                       Mat reduce_line,    /// 消掉五線譜線的圖
                                       int e_count, int* l_edge, int* distance, ///從 recognition_0 的資料來 加速
                                       string method);

void Grab_MaybeHead_from_ResultMap(Mat staff_result_map, int& maybe_head_count,float maybe_head[][200], int pitch_base_y, Mat staff_bin_erase_line, Mat template_img, float thresh_hold = 0.38);

void debug_draw_result_map_on_staff_bin_erase_line(Mat staff_result_map, Mat staff_bin_erase_line, Mat template_img, int l, int r, int t, int d, Scalar color=Scalar(0, 0, 255), string window_name="debug");