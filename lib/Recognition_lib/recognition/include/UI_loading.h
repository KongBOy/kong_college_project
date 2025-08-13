#ifndef UI_LOADING_H_INCLUDED
#define UI_LOADING_H_INCLUDED



#endif // UI_LOADING_H_INCLUDED


#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

void UI_loading_preprocess(Mat ord_img,
                           Mat test_bin,
                           int staff_count, int*** left_point , int*** right_point,
                           Mat UI_bass,string UI_WINDOW_NAME,
                           bool debuging=false);

void UI_loading_recognition_row(int staff_count,Mat staff_img,int row_note_count,int row_note[][1000],
                                Mat UI_bass,string UI_WINDOW_NAME);


void UI_loading_recognition(int staff_count,Mat staff_img[],
                            int note_count,int note[][1000],int row_note_count_array[],
                            Mat UI_bass,string UI_WINDOW_NAME);


/*
void UI_loading(Mat ord_img,
                Mat test_bin,
                int staff_count, int*** left_point , int*** right_point,
                Mat staff_img[],
                int note_count,int note[][1000],int row_note_count_array[]);
*/
