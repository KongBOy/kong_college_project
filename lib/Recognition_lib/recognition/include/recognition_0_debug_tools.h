#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#ifndef RECOGNITION_0_DEBUG_TOOLS_H_INCLUDED
// 辨識出來 音符 放這邊
struct Note_infos {
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
};

#define RECOGNITION_0_DEBUG_TOOLS_H_INCLUDED
#endif // RECOGNITION_0_DEBUG_TOOLS_H_INCLUDED



using namespace cv;

void set_formate(int head_type,int time_bar,Scalar & color , Mat & template_img);


void list_note_info(int note_count,int note[][1000]);
void watch_note(Mat debug_img , int note_count, int note[][1000]);
void draw_note(Mat debug_img , int note_count, int note[][1000]);

void list_row_note_info(int row_note_count,int row_note[][1000]);
void watch_row_note(Mat debug_img , int row_note_count, int row_note[][1000]);
void draw_row_note(Mat debug_img , int row_note_count, int row_note[][1000]);


void list_head_info(int maybe_head_count,float maybe_head[][200]);
void watch_head(Mat debug_img , Mat template_img, int maybe_head_count, float maybe_head[][200]);
void draw_head(Mat& debug_img , Mat template_img, int maybe_head_count, float maybe_head[][200]);

void list_lines_info(int lines_count,short lines[][200],bool lines_dir[][200]);
void draw_lines(Mat& debug_img,int lines_count,short lines[][200],bool lines_dir[][200]);
void watch_lines(Mat debug_img,int lines_count,short lines[][200],bool lines_dir[][200]);
