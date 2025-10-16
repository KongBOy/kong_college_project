#ifndef UI_LOADING_H_INCLUDED
#define UI_LOADING_H_INCLUDED



#endif // UI_LOADING_H_INCLUDED


#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

void UI_loading_preprocess(Mat src_img,
                           Mat bin_img,
                           int staff_count, int*** left_point, int*** right_point,
                           Mat UI_bass,string UI_WINDOW_NAME,
                           bool debuging=false);

void UI_loading_recognition_row(int staff_count, Mat staff_img, int row_note_count, int row_note[][1000],
                                Mat UI_bass, string UI_WINDOW_NAME);

