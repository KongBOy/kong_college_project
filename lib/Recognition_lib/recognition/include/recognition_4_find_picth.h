#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>


#ifndef RECOGNITION_5_FIND_PICTH_H_INCLUDED
#define RECOGNITION_5_FIND_PICTH_H_INCLUDED
#endif // RECOGNITION_5_FIND_PICTH_H_INCLUDED

using namespace cv;
using namespace std;

void recognition_5_find_pitch(Mat cut_ord_img,
                              Mat template_img,
                              int note_count , int note[][1000],
                              const int first_line_y_ord, 
                              int go_staff,
                              bool debuging=false);
