#pragma once

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <windows.h>
#include "Note_infos.h"
#include "Recognition.h"

#define SNDQUE 10000

using namespace std;
using namespace cv;

extern int speed;
extern int volume;
extern bool MusicPlayback;
extern Mat UI_Output;
extern int row_index;
extern Mat row_proc_img[40];

extern int MinValue;
extern int MaxValue;
extern int speed_row;
extern int volume_row;


extern double changespeed;



extern Mat Speed_Volume_Bar;
extern Mat background      ;
extern Mat bar             ;



typedef struct _soundtype{
  double  Freq;   // 頻率（Hz），代表音高
  int     Dura;   // 持續時間（毫秒）
  int     Vol;    // 音量
  int     Voice;  // 音色或聲部種類
  double  Tempo;  // 節奏速度
  int     sndTid; // 內部使用的識別 ID
} soundtype;

////////////////////////////////////////////////////////////
class Midi_Generate{
    private:
        soundtype  SndPmtr[SNDQUE + 1];
        int        gTenter;  // 佇列寫入位置（enter index）
        int        gTwait;   // 等待狀態位置
        int        gTexit;   // 佇列取出位置（exit index）
        int        gTarray;  // 當前可用的陣列索引
        bool       gTsig;    // 控制旗標，用於同步/中斷播放

        float freqTable[7][12];
public:
        Midi_Generate();
        int MakeSound (float Freq, int Dura = 0, int Vol = 127, int Voice = 0, float Tempo = 1);
        int GenerateMidiFile(Note_infos& note_infos);

        soundtype* get_SndPmtr();
        int get_gTenter();
        int  get_gTwait();
        int  get_gTexit();
        bool get_gTsig();

        void set_gTwait(int in_gTwait);
        void set_gTexit(int in_gTexit);
};

////////////////////////////////////////////////////////////
class Midi_ShowPlay{
    private:
        Recognition_page* recog_page_ptr;
        Midi_Generate*    mide_notes_ptr;

    public:
        Midi_ShowPlay(Recognition_page* in_recog_page_ptr, Midi_Generate* in_midi_notes_ptr);

        // PlayMidiFile 是用 CreateThread() 來把 PlaySnd 丟給thread,
        // CreateThread() 參數規定 丟的function 不可以含this 要不然參數會對不上,
        // 所以 要用 static 讓 member function 變成普通function,
        // 如果想要用 data member, 可以 第四個參數 丟 function參數時 丟this指標進去
        void PlayMidiFile();
        // LPVOID lpParameter 就會 是this指標,
        // 再強制轉換成 Recognition_page* 就可以用囉, 具體如下
        // Recognition_page* self_ptr = (Recognition_page*)lpParameter;
        // Recognition_page& self     = *self_ptr;
        static DWORD WINAPI PlaySnd (LPVOID lpParameter);
};



double  Round (double,int);

int GenerateMidiFile(Note_infos* note_infos, Mat staff_img[]);

int MakeSound (float Freq, int Dura = 0, int Vol = 127, int Voice = 0, float Tempo = 1);
DWORD WINAPI PlaySnd (LPVOID lpParameter);
void PlayMidiFile    (Note_infos* note_infos);

int Drawing_Random_Circles( Mat& image);
