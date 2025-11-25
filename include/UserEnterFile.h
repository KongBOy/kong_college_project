#pragma once


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Generate_Play_Midi.h"

#define ORBIT_NUM 8

using namespace std;
using namespace cv;

class Camera_HandShaking_Detect{
    private:
        CvScalar MeanScalar;
        CvScalar StandardDeviationScalar;

        Mat* frame_ptr;              // 接主frame的ptr, VideoCapture只能在 主thread 裡面才能正常運作, 所以這裡second frame 多設一個ptr 來接 主thread 的 frame 來和主frame共用
        Mat frame;                   // 接主frame的ptr 參考的 原始frame, 主要是想讓 -> 改成 . 好寫一點
	    Mat frame_small;             // frame 縮小處理效果差不多 但 會快很多, 然後畫好的結果也是直接放 small_frame上囉
	    Mat frame_small_draw_orbit;  // frame 縮小後 畫上 軌跡的frame放這邊, 主frame想拿結果可以用 get_frame_small_draw_orbit()

        int orbit_num;
        int orbitX[ORBIT_NUM];
        int orbitY[ORBIT_NUM];
        int go_orbit;

        float prey;
        float nowy;

        int  clock_cost_buffer_size;  // 實際 clock_cost_buffer_size 大小
        int  clock_cost_buffer_acc;   // 虛擬 clock_cost_buffer_size 大小, 大概就是假設 buffer無限大 總共input了幾次 clock_cost 進buffer的概念, 也可以代表 最新可以寫入 buffer的位置
        int* clock_cost_buffer;
        int  clock_cur_posi;

        bool now_handmoveup;
        bool pre_handmoveup;

        time_t pre_handmoveup_clock; 
        time_t now_handmoveup_clock;

        // 從 Midi_ShowPlay 傳進來的 Midi播放與手勢偵測thread 共用的資料空間(目前裡面有 速度, 音量, MusicPlayback)
        Midi_shared_datas* midi_shared_datas_ptr;
        // 偵測 音量/速度 的時候怕太敏感, 設定在某時間內抓一次的clock
        time_t detect_volume_clock_pre;
        time_t detect_speed_clock_pre;
        // 播放手勢偵測的執行緒
        HANDLE gSThread;

    public:
        Camera_HandShaking_Detect(Midi_shared_datas* in_midi_shared_datas_ptr);

        void SamplePicInitial();
        void Detect_Volumn();
        bool Detect_Speed();

        static DWORD WINAPI HandShaking(LPVOID lpParameter);
        void thread_HandShaking();

        void set_frame_ptr(Mat* in_frame_ptr);
        Mat& get_frame_small_draw_orbit();

};