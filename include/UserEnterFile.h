#pragma once


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#include "Generate_Play_Midi.h"

#define ORBIT_NUM 8


using namespace std;
using namespace cv;
int camera();
int HandShaking(string Title);
int HandDetection( );
extern int speed;
extern int volume;
extern bool MusicPlayback;

extern Mat UI_Output;
extern int row_index ;
extern Mat row_proc_img[40];
static string Title="小小指揮家";


class Camera_HandShaking_Detect{
    private:
        int orbit_num;
        int orbitX[ORBIT_NUM];
        int orbitY[ORBIT_NUM];

        float prey;
        float nowy;

        int  clock_cost_buffer_size;  // 實際 clock_cost_buffer_size 大小
        int  clock_cost_buffer_acc;   // 虛擬 clock_cost_buffer_size 大小, 大概就是假設 buffer無限大 總共input了幾次 clock_cost 進buffer的概念, 也可以代表 最新可以寫入 buffer的位置
        int* clock_cost_buffer;
        int  clock_cur_posi;

        bool now_handmoveup;
        bool pre_handmoveup;

        time_t pre_handmove_up_clock; 
        time_t now_handmoveup_clock;

        // Midi播放 和 手勢偵測共用的資料空間: 速度 和 音量
        Midi_shared_datas* midi_shared_datas_ptr;

    public:
        Camera_HandShaking_Detect(Midi_shared_datas* in_midi_shared_datas_ptr);
        static DWORD WINAPI HandShaking(LPVOID lpParameter);
        void SamplePicInitial();
        void Detect_Volumn(Mat frame_small, int go_orbit);
        bool Detect_Speed();

};