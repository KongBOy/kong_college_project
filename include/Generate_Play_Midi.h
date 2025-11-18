#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <windows.h>
#include "Note_infos.h"

#define SNDQUE 10000

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
        BOOL       gTsig;    // 控制旗標，用於同步/中斷播放

        float freqTable[7][12];
public:
        Midi_Generate();
        int MakeSound (float Freq, int Dura = 0, int Vol = 127, int Voice = 0, float Tempo = 1);
        int GenerateMidiFile(Note_infos* note_infos);

        soundtype* get_SndPmtr();
        int get_gTenter();
        int get_gTwait();
        int get_gTexit();
        BOOL get_gTsig();

        void set_gTwait(int in_gTwait);
        void set_gTexit(int in_gTexit);
};




double  Round (double,int);

int GenerateMidiFile(Note_infos* note_infos, Mat staff_img[]);

int MakeSound (float Freq, int Dura = 0, int Vol = 127, int Voice = 0, float Tempo = 1);
DWORD WINAPI PlaySnd (LPVOID lpParameter);
void PlayMidiFile    (Note_infos* note_infos);

int Drawing_Random_Circles( Mat& image);
