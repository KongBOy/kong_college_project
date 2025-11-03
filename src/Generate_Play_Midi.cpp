#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <windows.h>

#include "Bar_tool.h"
#include "UserEnterFile.h"


#include "Generate_Play_Midi.h"

#define SNDQUE 10000

using namespace std;
using namespace cv;

int speed  = 60;
int volume = 80;
bool MusicPlayback=false;
Mat Output;
int row_index = 0;
Mat row_proc_img[40];

int MinValue=863;
int MaxValue=1244;
int speed_row=390;
int volume_row=275;


double changespeed=1;


Mat background            = imread("Resource/UI_all_picture/UI PIC/UI/Background_kong.png", 1);
Mat Speed_Volume_Bar      = background(Rect(750, 245, 545, 233)).clone();
Mat Speed_Volume_Bar_roi;
Mat volume_bar_roi;
Mat speed_bar_roi ;
Mat bar                   = imread("Resource/UI_all_picture/UI PIC/UI/Bar.png", 1);
//800 245 495 233

HANDLE     gSThread = NULL;  // 播放聲音的執行緒


typedef struct _soundtype{
  double  Freq;   // 頻率（Hz），代表音高
  int     Dura;   // 持續時間（毫秒）
  int     Vol;    // 音量
  int     Voice;  // 音色或聲部種類
  double  Tempo;  // 節奏速度
  int     sndTid; // 內部使用的識別 ID
} soundtype, *LPSOUNDTYPE;

static soundtype  SndPmtr[SNDQUE + 1];
static int        gTenter;  // 佇列寫入位置（enter index）
static int        gTwait;   // 等待狀態位置
static int        gTexit;   // 佇列取出位置（exit index）
static int        gTarray;  // 當前可用的陣列索引
static BOOL       gTsig;    // 控制旗標，用於同步/中斷播放


int Sound (float Freq, int Dura , int Vol , int Voice , float Tempo){
    // cout << "gTexit:" << gTexit << endl;
    // cout << "Sound" << Freq << " " << Dura << " " << Vol << " " << Voice << " " << Tempo << endl;
    DWORD  dwThreadId;

    if (Freq == 0 && Dura < 1) return gTenter - gTexit;  // 回傳排隊中的數量
    // silence
    if (Freq == 0) Vol  = 0;  // 無聲音時音量歸零
    if (Dura <  5) Dura = 5;  // 時間太短的話強制最小值
    gTenter++;
    gTsig = FALSE;
    if (gTenter >= SNDQUE){
        gTarray = gTenter % SNDQUE + 1;
    }
    else{
        gTarray = gTenter;
    }
    SndPmtr[gTarray].Freq   = Freq;
    SndPmtr[gTarray].Dura   = Dura;
    SndPmtr[gTarray].Vol    = Vol;
    SndPmtr[gTarray].Voice  = Voice;
    SndPmtr[gTarray].Tempo  = Tempo;
    SndPmtr[gTarray].sndTid = gTenter;

    // if (gSThread == NULL && (Freq == Abs(Freq) || Freq == 0)){
    //     // "PlaySnd" needs casting (void *)
    //     gSThread = CreateThread(NULL, 0, PlaySnd, (void *)"PlaySnd", 0, &dwThreadId);
    //     //Sleep(1);
    //     cout << "thread---------------" << gSThread << endl;
    //     return 0;
    // }

    // 目前的程式Freq沒有設負的過用不到
    // if (Freq != Abs(Freq)){
    //     cout << "here~~~~~~~~~~~~~~~~~~" << endl;
    //     if (Freq == -1){
    //         Freq = 0;
    //         SndPmtr[gTarray].Vol = 0;
    //     }
    //     SndPmtr[gTarray].Freq = Abs(Freq);
    //     gTsig=TRUE;
    //     while(gSThread != NULL){
    //         Sleep(10);
    //     }
    //     gTexit = gTenter-1;
    //     gTwait = gTenter-1;
    //     gTsig = FALSE;
    //     return PlaySnd(NULL);  // needs some kind of argument
    // }
    return 0;
}



double Round (double n, int d){
    return (floor((n)*pow(10.0, (d))+0.5)/pow(10.0, (d)));
}
// 都被註解掉了用不到
// double Abs (double a){
//     if (a < 0)  return -a;
//     return  a;
// }



int GenerateMidiFile(Note_infos* note_infos, Mat staff_img[]){
    static float freqTable[7][12]={ {32.7  , 34.6  , 36.7  , 38.9  , 41.2  , 43.7  , 46.2  , 49.0  , 51.9  , 55.0  , 58.3  , 61.7  }, \
                                    {65.4  , 69.3  , 73.4  , 77.8  , 82.4  , 87.3  , 92.5  , 98.0  , 103.8 , 110.0 , 116.5 , 123.5 }, \
                                    {130.8 , 138.6 , 146.8 , 155.6 , 164.8 , 174.6 , 185.0 , 196.0 , 207.7 , 220.0 , 233.1 , 246.9 }, \
                                    {261.6 , 277.2 , 293.7 , 311.1 , 329.6 , 349.2 , 370.0 , 392.0 , 415.3 , 440.0 , 466.2 , 493.9 }, \
                                    {523.3 , 554.4 , 587.3 , 622.3 , 659.3 , 698.5 , 740.0 , 784.0 , 830.6 , 880.0 , 932.3 , 987.8 }, \
                                    {1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5}, \
                                    {2093.0, 2217.5, 2349.3, 2489.0, 2637.0, 2793.8, 2960.0, 3136.0, 3322.4, 3520.0, 3729.3, 3951.1}};


    for(int i = 0 ; i < 40 ; i++){
        row_proc_img[i] = staff_img[i].clone();
        cvtColor(staff_img[i], row_proc_img[i], CV_GRAY2BGR);
        //       note_infos -> row_note_count_array[i] = in_row_note_count_array[i];
    }

    //   note_infos -> note_count = in_note_count;
    //   for(int i = 0 ; i < 5 ; i++)
    //     for(int j = 0 ; j < note_infos -> note_count ; j++)
    //         note_infos -> note[i][j] = in_note[i][j];


    // *************************************************
    // list_note_info(note_infos -> note_count, note_infos -> note);

    cout << "step1" << endl;
    
    
    // **************************************************************
    // **************************************************************
    int head_type;
    int time_bar;
    int pitch;
    int base_4note_duration;
    speed = 60;  // 速度單位 bpm, 代表 每分鐘均等的打幾下, 舉例: 132bmp 代表 60秒 打 132下
    for(int go_note = 0 ; go_note < note_infos -> note_count ; go_note++){
        head_type = note_infos -> note[2][go_note];
        time_bar  = note_infos -> note[3][go_note];
        pitch     = note_infos -> note[4][go_note];

        // speed bpm 代表 60秒 打speed下, 所以打一下是 60/speed 秒, 然後因為是給 Sleep()用 單位是毫秒 所以 * 1000
        // 音樂上通常以 四分音符 的 bpm 來當基礎, 所以設定 四分音符 打一下幾秒 當基礎
        base_4note_duration = (60 / speed) * 1000;
        switch(head_type){
            // 全音符
            case 0:
                Sound(freqTable[ pitch /12 -1 ][ pitch %12 ], base_4note_duration * 4, 127, 0);
                break;
            
            // 二分音符
            case 2:
                Sound(freqTable[ pitch /12 -1 ][ pitch %12 ], base_4note_duration * 2,  127,  0);
                break;
            
            // 四 八 十六 ... 分音符(分別time_bar = 0, 1, 2, ...)
            case 4:
                Sound(freqTable[ pitch /12 -1 ][ pitch %12 ], base_4note_duration * pow(2, - 1 * time_bar ),  127,  0);
                break;

            // 四分休止符
            case 5:
                Sound(0, base_4note_duration * 1, 0, 0);
                break;

            // 全休止符
            case 1:
                Sound(0, base_4note_duration * 4, 0, 0);
                break;

            // 二分休止符
            case 3:
                Sound(0, base_4note_duration * 2, 0, 0);
                break;

            // 十六分休止符
            case 6:
                Sound(0, base_4note_duration * 0.25, 0, 0);
                break;
            
            // 三十二分休止符
            case 7:
                Sound(0, base_4note_duration * 0.125, 0, 0);
                break;
            
            // 八分休止符
            case 8:
                Sound(0, base_4note_duration * 0.5, 0, 0);
                break;
            
            // 高音譜記號
            // case 9:
            //     Sound(0, 0, 0, 0);
            //     break;
        }
    }
    return 0;
    // cout << "speed" << speed << endl;

    // Tugboat whistle sound 95 hertz, 2000ms, 127 = loud, 111 = Shanai
    // experiment with your own sounds, it's fun ...


    /*
    int a=0;
    cout << "step1" << endl;
    Sound(freqTable[3][a%7], 1000, 127, 0);  // 2 second blast
    cout << "step2" << endl;
    //Sound( 1, 500, 0, 0);  // 1 second of silence
    a+=1;
    // wait till que is empty

    while(a<10000){
        // cout << "step1" << endl;
        Sound(freqTable[3][a%12], 1000, 127, 0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12], 1000, 127, 0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12], 1000, 127, 0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12], 1000, 127, 0);  // 2 second blast
        a+=1;
        // cout << "step2" << endl;
        //Sound( 1, 1, 0, 0);

    }



    // // // // // // // // // // // // // //

    while(gSThread!=NULL){
        Sleep(1000);
        speed+=1;
        // changespeed=changespeed*0.9995;
    }
    return 0;
    */
}



DWORD WINAPI PlaySnd (LPVOID lpParameter){
    /*
    因為 要丟到 CreateThread 裡面, 所以 參數要照規定丟 只能放一個 LPVOID void *, 
    傳進來以後 在強制轉換成 自己要的 pointer
    */
    Note_infos* note_infos = (Note_infos*)lpParameter;

    soundtype  LocSndPar;
    int  lTarray;
    int Note = 0;
    int Phrase = 0;
    row_index = 0;
    note_infos -> go_note=0;
    HMIDIOUT hMidi;
    midiOutOpen    (&hMidi, (UINT)-1, 0, 0, CALLBACK_NULL);
    midiOutShortMsg(hMidi, (256 * LocSndPar.Voice ) + 192);

    int note_x;
    int note_y;
    int head_type;
    int time_bar;
    int pitch;
    Mat template_img;
    Scalar color;
    Point pitch_word_posi;
    while(gTenter > gTexit && gTsig == FALSE){
        if(MusicPlayback){
            // ******************************************************************************************************
            // 先畫圖顯示音高 再 播音樂, 才不會 每一row影像 的最後一顆音播完音樂 還沒顯示完 就 換下一row的影像了
            note_x    = note_infos -> note[0][note_infos -> go_note];
            note_y    = note_infos -> note[1][note_infos -> go_note];
            head_type = note_infos -> note[2][note_infos -> go_note];
            time_bar  = note_infos -> note[3][note_infos -> go_note];
            pitch     = note_infos -> note[4][note_infos -> go_note];
            // 遇到 高音譜記號 或 八分符桿 就跳過不顯示音高
            while(head_type == 9 || head_type == 10){
                note_infos -> go_note++;
                note_infos -> go_row_note++;
                if( note_infos -> go_row_note >= note_infos -> row_note_count_array[row_index]){
                    note_infos -> go_row_note = 0;
                    row_index++;
                }
                note_x    = note_infos -> note[0][note_infos -> go_note];
                note_y    = note_infos -> note[1][note_infos -> go_note];
                head_type = note_infos -> note[2][note_infos -> go_note];
                time_bar  = note_infos -> note[3][note_infos -> go_note];
                pitch     = note_infos -> note[4][note_infos -> go_note];
            }
            // 取出 template_img 和 head_type顏色
            get_note_color_and_img(head_type, time_bar, color, template_img);
            // 畫出 note框框
            rectangle(row_proc_img[row_index], Point(note_x                    , note_y), 
                                               Point(note_x + template_img.cols, note_y + template_img.rows), color, 2);

            // 定位 音高文字的位置
            pitch_word_posi.x = note_x + 8;
            pitch_word_posi.y = note_y - 30;
            // pitch_word_posi.y = 20;
            Scalar circle_background(0, 0, 0);
            // 定出 各八度的顏色
            switch( (pitch / 12) ){
                case 4:  // 低音c 八度音
                    circle_background = Scalar(100, 120, 125);  // 灰色
                    break;

                case 5:  // 中央C 八度音
                    circle_background = Scalar(255, 143, 218);  // 粉紅色
                    break;

                case 6:  // 高音C 八度音
                    circle_background = Scalar(255, 180, 100);  // 蛋藍色
                    break;

                case 7:  // 高高音c 八度音
                    circle_background = Scalar(100, 120, 125);  // 灰色
                    break;
                }

            // 休止符系列
            if(head_type == 1 || head_type == 3 || head_type == 5 || head_type == 6 || head_type == 7 || head_type == 8)
                circle_background = Scalar(0, 99, 185);  // 咖啡色

            // 畫出 音高文字圈圈
            circle(row_proc_img[row_index], pitch_word_posi, 15, circle_background, -1, 1, 0);
            
            // 畫出 音高文字圈圈
            int movetocenter = 10;
            pitch_word_posi.x = pitch_word_posi.x - movetocenter;
            pitch_word_posi.y = pitch_word_posi.y + movetocenter;
            
            // 計算音高
            int    nodePitch = pitch % 12;
            string nodePitch_str;
            switch(nodePitch){
                case 0:
                    nodePitch=1;
                    break;
                case 2:
                    nodePitch=2;
                    break;
                case 4:
                    nodePitch=3;
                    break;
                case 5:
                    nodePitch=4;
                    break;
                case 7:
                    nodePitch=5;
                    break;
                case 9:
                    nodePitch=6;
                    break;
                case 11:
                    nodePitch=7;
                    break;
            }
            
            // Int2str(nodePitch)
            stringstream ss;
            ss << nodePitch;
            // 休止符系列 顯示 "R"
            if(head_type == 1 || head_type == 3 || head_type == 5 || head_type == 6 || head_type == 7 || head_type == 8)
                putText(row_proc_img[row_index], "R"     , pitch_word_posi, FONT_HERSHEY_PLAIN, 2.0, Scalar(255, 255, 255), 2, 1, false);
            // 剩下顯示 簡譜數字
            else 
                putText(row_proc_img[row_index], ss.str(), pitch_word_posi, FONT_HERSHEY_PLAIN, 2.0, Scalar(255, 255, 255), 2, 1, false);



            Drawing_Random_Circles(Output);
            
            Speed_Volume_Bar_roi = Output(Rect(750, 245, 545, 233));
            Speed_Volume_Bar.copyTo(Speed_Volume_Bar_roi);
            volume_bar_roi = Output(Rect( (volume       /         127.) * (MaxValue - MinValue) + MinValue, volume_row, bar.cols, bar.rows) );
            speed_bar_roi  = Output(Rect( (speed  - 20.)/ (300. -  20.) * (MaxValue - MinValue) + MinValue, speed_row , bar.cols, bar.rows) );
            bar.copyTo(volume_bar_roi);
            bar.copyTo(speed_bar_roi );
            
            
            //  ******************************************************************************************************
            //  ******************************************************************************************************
            //  ******************************************************************************************************
            //  ******************************************************************************************************

            gTwait++;
            if (gTwait >= SNDQUE)
                lTarray = gTwait % SNDQUE+1;
            else
                lTarray = gTwait;
            LocSndPar = SndPmtr[lTarray];

            Note   = 0;
            Phrase = 0;
            // Msg 共32bit, 從 左2^31 ~ 右2^0 共分成四個區塊
            //  0 ~  7 (2^31~2^24): 未使用
            //  8 ~ 15 (2^23~2^16): 音量
            // 16 ~ 23 (2^15~2^08): 音色
            // 24 ~ 31 (2^07~2^00): 狀態
            //   狀態:
            //      0x80(128) : 關閉音符
            //      0x90(144) : 播放音符
            //      0xA0(160) : 鍵盤壓力感應
            //      0xB0(176) : 控制器訊號（像音量、踏板）
            //      0xC0(192) : 切換樂器音色
            //      0xE0(224) : 音高彎曲（滑音）
            // 頻率轉MIDI的公式: 69 + 12 * log_2(頻率 / 440), 69是 A(440hz) 在 MIDI表內面的數字, 每升高一個八度就多 12
            Note = (int)Round( ( 69 + 12 * log2(LocSndPar.Freq / 440) ) , 0);
            // cout << "Note:" << Note << endl;
            // Phrase = (LocSndPar.Vol * 256 + Note) * 256 + 144;  // Note on.
            Phrase = (volume * 256 + Note ) * 256 + 144;  //Note on
            // cout << "volume:" << volume << endl;
            // cout << "speed :" << speed << endl;
            midiOutShortMsg(hMidi, Phrase);
            //  ******************************************************************************************************
            // cout << "Noteon ON " << LocSndPar.Freq << endl;
            changespeed=(60 / (float)speed);

            // cout << "changespeed" << changespeed << endl;
            // cout << "LocSndPar.Dura  :" << LocSndPar.Dura << endl;
            // cout << "LocSndPar.Tempo :" << LocSndPar.Tempo << endl;
            // cout << "changespeed     :" << changespeed << endl;
            Sleep((int) (LocSndPar.Dura * (1 / LocSndPar.Tempo + 0.0001 ) * changespeed ) );
            // cout << "Noteoff END " << LocSndPar.Freq << endl;
            // Sleep((int)LocSndPar.Dura);
            Phrase = (0 * 256 + Note ) * 256 + 128 ;  //Note off
            midiOutShortMsg(hMidi, Phrase);

            gTexit++;



            note_infos -> go_note++;
            note_infos -> go_row_note++;
            if( note_infos -> go_row_note >= note_infos -> row_note_count_array[row_index]){
                note_infos -> go_row_note = 0;
                row_index++;
            }
        }
        else{
            break;
        }

    }
    MusicPlayback=false;

    midiOutClose(hMidi);
    CloseHandle(gSThread);
    gSThread = NULL;
    return 0;
}

void PlayMidiFile(Note_infos* note_infos){
    MusicPlayback=true;
    DWORD  dwThreadId;
    gSThread = CreateThread(NULL, 0, PlaySnd, note_infos, 0, &dwThreadId);
    if (gSThread == NULL) {
        cerr << "Failed to create thread!" << endl;
        delete note_infos; // 若 thread 沒建立成功要釋放記憶體
        return;
    }
    cout << "thread---------------" << gSThread << endl;

    //////////////////////////
}

int Drawing_Random_Circles( Mat& image ){
    // cout << "Drawing_Random_Circles" << endl;
    int lineType = 8;
    Point pt1, pt2;

    for( int i = 0; i < 1; i++ ){
        pt1.x = rand() % background.cols;
        pt1.y = rand() % background.rows;
        pt2.x = rand() % background.cols;
        pt2.y = rand() % background.rows;
        int radian = 2 + rand() % 3;
        circle(image, pt2, radian, Scalar(255, 255, 255), -1, 1, 0);
        circle(image, pt1, radian, Scalar(255, 255, 255), -1, 1, 0);
    }
    return 0;
}
