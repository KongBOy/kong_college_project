#pragma once
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// 辨識出來 音符 放這邊
class Note_infos {
    public:
        /*
        note[0]: x
        note[1]: y
        note[2]: type
        note[3]: time
        note[4]: pitch
        */
        int note_count;
        int note[5][1000];
        int row_note_count_array[40];

        int go_note;
        int go_row_note;

        Note_infos();
};

using namespace std;
using namespace cv;

void get_note_color_and_img(int head_type,int time_bar,Scalar & color , Mat & template_img);


void list_note_info(int note_count,int note[][1000]);
void watch_note(Mat debug_img , int note_count, int note[][1000]);
void Draw_note(string WINDOW_NAME, Mat base_img, int note_count, int note[][1000], int x_shift, int y_shift);

void list_row_note_info(int row_note_count,int row_note[][1000]);
void watch_row_note(Mat debug_img , int row_note_count, int row_note[][1000]);
void draw_row_note(Mat debug_img , int row_note_count, int row_note[][1000]);


