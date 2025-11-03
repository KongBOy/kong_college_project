#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED



#endif // HEADER_H_INCLUDED

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>


using namespace std;
using namespace cv;
int camera();
bool HandShaking(string Title);
int HandDetection( );
extern int speed;
extern int volume;
extern bool MusicPlayback;

extern Mat UI_Output;
extern int row_index ;
extern Mat row_proc_img[40];
static string Title="小小指揮家";
