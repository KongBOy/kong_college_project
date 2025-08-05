#ifndef RECOGNITION_H_INCLUDED
#define RECOGNITION_H_INCLUDED
#endif // RECOGNITION_H_INCLUDED

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
int Recognition(Mat ord_img,int& staff_count, Mat staff_img_erase_line[],Mat staff_img[],double trans_start_point_x[],double trans_start_point_y[],
                int& note_count , int note[][1000] , int row_note_count_array[],
                Mat UI_bass,string UI_WINDOW_NAME,
                string Title,Mat UI2_5,
                bool debuging=false);
