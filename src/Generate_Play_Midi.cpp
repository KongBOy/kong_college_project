#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <windows.h>

#include "recognition_0_debug_tools.h"
#include "UserEnterFile.h"


#include "Generate_Play_Midi.h"

#define SNDQUE 10000

using namespace std;
using namespace cv;

int speed=60;
int volume=80;
bool MusicPlayback=false;
Mat Output;
int row_index = 0;
Mat row_proc_img[40];

int MinValue=713;
int MaxValue=989;
int speed_row=400;
int volume_row=290;

RNG rng( 0xFFFFFFFF );

double changespeed=1;


Mat Speed_Volume_Bar=imread("Resource/UI_all_picture/UI PIC/UI/Speed_Volume_Bar.png",1);
Mat background      =imread("Resource/UI_all_picture/UI PIC/UI/Background.png",1);
Mat bar             =imread("Resource/UI_all_picture/UI PIC/UI/Bar.png",1);


HANDLE     gSThread = NULL;


typedef struct _soundtype
{
  double  Freq;
  int     Dura;
  int     Vol;
  int     Voice;
  double  Tempo;
  int     sndTid;
} soundtype, *LPSOUNDTYPE;

static soundtype  SndPmtr[SNDQUE+1];
static int        gTenter;
static int        gTwait;
static int        gTexit;
static int        gTarray;
static BOOL       gTsig;





double Round (double n, int d){
    return (floor((n)*pow(10.0,(d))+0.5)/pow(10.0,(d)));
}
double Abs (double a){
    if (a < 0)  return -a;
    return  a;
}



int GenerateMidiFile(Note_infos* note_infos, Mat staff_img[]){
    static float freqTable[7][12]={ {32.7  ,34.6  ,36.7  ,38.9  ,41.2  ,43.7  ,46.2  ,49.0  ,51.9  ,55.0  ,58.3  ,61.7  },\
                                    {65.4  ,69.3  ,73.4  ,77.8  ,82.4  ,87.3  ,92.5  ,98.0  ,103.8 ,110.0 ,116.5 ,123.5 },\
                                    {130.8 ,138.6 ,146.8 ,155.6 ,164.8 ,174.6 ,185.0 ,196.0 ,207.7 ,220.0 ,233.1 ,246.9 },\
                                    {261.6 ,277.2 ,293.7 ,311.1 ,329.6 ,349.2 ,370.0 ,392.0 ,415.3 ,440.0 ,466.2 ,493.9 },\
                                    {523.3 ,554.4 ,587.3 ,622.3 ,659.3 ,698.5 ,740.0 ,784.0 ,830.6 ,880.0 ,932.3 ,987.8 },\
                                    {1046.5,1108.7,1174.7,1244.5,1318.5,1396.9,1480.0,1568.0,1661.2,1760.0,1864.7,1975.5},\
                                    {2093.0,2217.5,2349.3,2489.0,2637.0,2793.8,2960.0,3136.0,3322.4,3520.0,3729.3,3951.1}};


    for(int i = 0 ; i < 40 ; i++){
        row_proc_img[i] = staff_img[i].clone();
        cvtColor(staff_img[i],row_proc_img[i],CV_GRAY2BGR);
        ///      note_infos -> row_note_count_array[i] = in_row_note_count_array[i];
    }

    ///  note_infos -> note_count = in_note_count;
    ///  for(int i = 0 ; i < 5 ; i++)
    ///    for(int j = 0 ; j < note_infos -> note_count ; j++)
    ///        note_infos -> note[i][j] = in_note[i][j];


    ///*************************************************
    list_note_info(note_infos -> note_count, note_infos -> note);

    speed=60;
    cout<<"step1"<<endl;
    /*
    for(int i = 0 ; i < 40 ; i++){
        row_src_img[i] = staff_img[i].clone();
        row_proc_img[i] = row_src_img[i].clone();
        cvtColor(row_src_img[i],row_proc_img[i],CV_GRAY2BGR);
        note_infos -> row_note_count_array[i] = in_row_note_count_array[i];
    }

    note_infos -> note_count = in_note_count;
    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j < note_infos -> note_count ; j++)
            note_infos -> note[i][j] = in_note[i][j];


    list_note_info(note_infos -> note_count,note_infos -> note);*/
    ///**************************************************************
    ///**************************************************************
    for(int go_note = 0 ; go_note < note_infos -> note_count ; go_note++){
        switch(note_infos -> note[2][go_note]){
            case 0:{
                Sound(freqTable[note_infos -> note[4][go_note]/12 -1][note_infos -> note[4][go_note]%12],
                    (60/speed)*1000* 4,
                    127,
                    0);
            }
            break;

            case 2:{
                Sound(freqTable[note_infos -> note[4][go_note]/12 -1][note_infos -> note[4][go_note]%12],
                    (60/speed)*1000* 2,
                    127,
                    0);
            }
            break;

            case 4:{
                Sound(freqTable[note_infos -> note[4][go_note]/12 -1][note_infos -> note[4][go_note]%12],
                    (60/speed)*1000*pow(0.5,note_infos -> note[3][go_note]),
                    127,
                    0);
            }
            break;

            case 5:{
                Sound(0,
                    (60/speed)*1000* 1,
                    0,
                    0);
            }
            break;

            case 1:{
                Sound(0,
                    (60/speed)*1000* 4,
                    0,
                    0);
            }
            break;

            case 3:{
                Sound(0,
                    (60/speed)*1000* 2,
                    0,
                    0);
            }
            break;

            case 6:{
                Sound(freqTable[note_infos -> note[4][go_note]/12 -1][note_infos -> note[4][go_note]%12],
                    (60/speed)*1000* 0.25,
                    0,
                    0);
            }
            break;

            case 7:{
                Sound(0,
                    (60/speed)*1000* 0.125,
                    0,
                    0);
            }
            break;

            case 8:{
                Sound(0,
                    (60/speed)*1000* 0.5,
                    0,
                    0);
            }
            break;
    /*
            case 9:{
                Sound(0,
                    0,
                    0,
                    0);
            }
            break;
    */
        }

    }
    return 0;
    //cout<<"speed"<<speed<<endl;

    // Tugboat whistle sound 95 hertz, 2000ms, 127 = loud, 111 = Shanai
    // experiment with your own sounds, it's fun ...


    /*
    int a=0;
    cout<<"step1"<<endl;
    Sound(freqTable[3][a%7],1000,127,0);  // 2 second blast
    cout<<"step2"<<endl;
    //Sound( 1,500,  0,0);  // 1 second of silence
    a+=1;
    // wait till que is empty

    while(a<10000){
        //cout<<"step1"<<endl;
        Sound(freqTable[3][a%12],1000,127,0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12],1000,127,0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12],1000,127,0);  // 2 second blast
        a+=1;
        Sound(freqTable[3][a%12],1000,127,0);  // 2 second blast
        a+=1;
        //cout<<"step2"<<endl;
        //Sound( 1,1,  0,0);

    }



    /////////////////////////////////////////

    while(gSThread!=NULL){
        Sleep(1000);
        speed+=1;
        //changespeed=changespeed*0.9995;
    }
    return 0;
    */
}

int Sound (float Freq,int Dura,int Vol,int Voice,float Tempo){
    //cout<<"Sound"<<Freq<<" "<<Dura<<" "<<Vol<<" "<<Voice<<" "<<Tempo<<endl;
    DWORD  dwThreadId;

    if (Freq == 0 && Dura < 1) return gTenter-gTexit;
    // silence
    if (Freq == 0) Vol  = 0;
    if (Dura <  5) Dura = 5;
    gTenter++;
    gTsig = FALSE;
    if (gTenter >= SNDQUE){
        gTarray = gTenter % SNDQUE+1;
    }
    else{
        gTarray=gTenter;
    }
    SndPmtr[gTarray].Freq   = Freq;
    SndPmtr[gTarray].Dura   = Dura;
    SndPmtr[gTarray].Tempo  = Tempo;
    SndPmtr[gTarray].Vol    = Vol;
    SndPmtr[gTarray].Voice  = Voice;
    SndPmtr[gTarray].sndTid = gTenter;
    /*if (gSThread == NULL && (Freq == Abs(Freq) || Freq == 0)){
        // "PlaySnd" needs casting (void *)
        gSThread = CreateThread(NULL,0,PlaySnd,(void *)"PlaySnd",0,&dwThreadId);
        //Sleep(1);
        cout<<"thread---------------"<<gSThread<<endl;
        return 0;
    }*/
    if (Freq != Abs(Freq)){
        if (Freq == -1){
            Freq = 0;
            SndPmtr[gTarray].Vol=0;
        }
        SndPmtr[gTarray].Freq=Abs(Freq);
        gTsig=TRUE;
        while(gSThread!=NULL){
            Sleep(10);
        }
        gTexit = gTenter-1;
        gTwait = gTenter-1;
        gTsig = FALSE;
        return PlaySnd(NULL);  // needs some kind of argument
    }
    return 0;
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
    row_index=0;
    note_infos -> go_note=0;
    HMIDIOUT hMidi;
    midiOutOpen(&hMidi,(UINT)-1,0,0,CALLBACK_NULL);
    midiOutShortMsg(hMidi,(256*LocSndPar.Voice)+192);
    while(gTenter > gTexit && gTsig == FALSE){
        if(MusicPlayback){
            gTwait++;
            if (gTwait >= SNDQUE)
                lTarray = gTwait % SNDQUE+1;
            else
                lTarray = gTwait;

            LocSndPar = SndPmtr[lTarray];
            //cout<<"lTarray "<<lTarray<<endl;
            //cout<<"gTenter "<<gTenter<<endl;
            //cout<<"gTexit "<<gTexit<<endl;
            Note=0;
            Phrase=0;

            // convert frequency to midi note
            Note = (int)Round((log(LocSndPar.Freq)-log(440.0))/log(2.0)*12+69,0);
            //Phrase = (LocSndPar.Vol*256+Note)*256+144;//Noteon.
            Phrase = (volume*256+Note)*256+144;//Noteon
            midiOutShortMsg(hMidi,Phrase);

            /// ~~~~~~~~~~~加這邊邊邊邊邊~~~~~~~~~~~~
            /// ******************************************************************************************************
            /// ******************************************************************************************************
            /// ******************************************************************************************************
            if(note_infos -> note[2][note_infos -> go_note] == 9){
                note_infos -> go_note++;
                note_infos -> go_row_note++;
            }
            Mat template_img;
            Scalar color;
            set_formate(note_infos -> note[2][note_infos -> go_note],note_infos -> note[3][note_infos -> go_note],color,template_img);
            rectangle(row_proc_img[row_index],Point(note_infos -> note[0][note_infos -> go_note]                  , note_infos -> note[1][note_infos -> go_note]),
                                              Point(note_infos -> note[0][note_infos -> go_note]+template_img.cols, note_infos -> note[1][note_infos -> go_note]+template_img.rows),color,2);

            Point pt2;
            pt2.x = note_infos -> note[0][note_infos -> go_note]+8;
            pt2.y = 20;
            Scalar circle_background(0,0,0);

            switch( (note_infos -> note[4][note_infos -> go_note]/12) ){
                case 4:/// 低音c
                    circle_background = Scalar(100,120,125);///灰色
                    break;

                case 5:/// 中央C
                    circle_background = Scalar(255,143,218);///粉紅色
                    break;

                case 6:/// 高音C
                    circle_background = Scalar(255,180,100);///蛋藍色
                    break;

                case 7:/// 高高音c
                    circle_background = Scalar(100,120,125);///灰色
                    break;

            }
            circle(row_proc_img[row_index],pt2,15,circle_background,-1,1,0);
            ///circle(row_proc_img[row_index],pt2,15,randomColor(rng),-1,1,0);
            int movetocenter=10;

            pt2.x = pt2.x-movetocenter;
            pt2.y = pt2.y+movetocenter;

            int nodePitch=note_infos -> note[4][note_infos -> go_note]%12;
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
            ///Int2str(nodePitch)
            string s;
            stringstream ss;
            ss<<nodePitch;
            putText(row_proc_img[row_index],ss.str(),pt2,FONT_HERSHEY_PLAIN,2,Scalar(255,255,255),2,1,false);
            /// dst.rowRange(height_acc,height_acc_next+1)
            ///    .colRange( width_acc, width_acc_next+1) = temp_bin.clone();
            ///    Mat temp = UI4_3.clone();
            ///    cvtColor(temp,UI4_3,CV_GRAY2BGR);
            ///    UI4_3.rowRange(0,row_proc_img[row_index].rows+1)
            ///         .colRange(0,row_proc_img[row_index].cols+1) = row_proc_img[row_index].clone();


            //for(int go_row = 500 ;go_row <500+ row_proc_img[row_index].rows ; go_row++)
            //   for(int go_col = 0 ; go_col <row_proc_img[row_index].cols ; go_col++)
            //      Output.at<Vec3b>(go_row,go_col) = row_proc_img[row_index].at<Vec3b>(go_row-500,go_col);

            ///UI4_3(Rect(0,0,row_proc_img[row_index].cols,row_proc_img[row_index].rows)) = row_proc_img[row_index].clone();



            Drawing_Random_Circles(Output,rng);
            DrawMat(Speed_Volume_Bar,Output,260,598);
            DrawMat(bar,Output,speed_row,(speed-20)*(MaxValue-MinValue)/(300-20)+MinValue);
            DrawMat(bar,Output,volume_row,(volume-80)*(MaxValue-MinValue)/(127-80)+MinValue);
            ///    imshow(Title,Output);
            ///    imshow("debug3",row_proc_img[row_index]);
            /*
            Mat temp = UI4_3.clone();
            cvtColor(UI4_3,temp,CV_BGR2GRAY);
            temp.rowRange(0,row_proc_img[row_index].rows+1)
                .colRange(0,row_proc_img[row_index].cols+1) = staff_img[row_index].clone();
            imshow("debug4",temp);
            */

            waitKey(1);
            note_infos -> go_note++;
            note_infos -> go_row_note++;
            if( note_infos -> go_row_note >= note_infos -> row_note_count_array[row_index]){
                note_infos -> go_row_note = 0;
                row_index++;
            }

            /// ******************************************************************************************************
            /// ******************************************************************************************************


            /// ******************************************************************************************************



            //cout<<"Noteon ON "<<LocSndPar.Freq<<endl;
            changespeed=(60/(float)speed);

            //cout<<"changespeed"<<changespeed<<endl;
            cout<<"LocSndPar.Dura "<<LocSndPar.Dura<<endl;
            cout<<"LocSndPar.Tempo "<<LocSndPar.Tempo<<endl;
            cout<<"changespeed "<<changespeed<<endl;
            Sleep((int)(LocSndPar.Dura*(1/LocSndPar.Tempo+0.0001)*changespeed));
            //cout<<"Noteoff END "<<LocSndPar.Freq<<endl;
            //Sleep((int)LocSndPar.Dura);
            Phrase = (LocSndPar.Vol*256+Note)*256+128;//Noteoff
            midiOutShortMsg(hMidi,Phrase);

            gTexit++;
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
    cout<<"thread---------------"<<gSThread<<endl;

    ////////////////////////////////////////
}

int Drawing_Random_Circles( Mat& image, RNG rng ){
    cout<<"Drawing_Random_Circles"<<endl;
    int lineType = 8;
    Point pt1, pt2;

    for( int i = 0; i < 1; i++ ){
        pt1.x = rand()%background.cols;
        pt1.y = rand()%background.rows;
        pt2.x = rand()%background.cols;
        pt2.y = rand()%background.rows;
        //pt1.x = rng.uniform( 0, Background.cols );
        //pt1.y = rng.uniform( 0, Background.rows );
        //pt2.x = rng.uniform( 0, Background.cols );
        //pt2.y = rng.uniform( 0, Background.rows );
        cout<<"pt1.x "<<pt1.x<<endl;
        cout<<"pt1.y "<<pt1.y<<endl;
        cout<<"pt2.x "<<pt2.x<<endl;
        cout<<"pt2.y "<<pt2.y<<endl;
        int radian=2+rand()%3;
        circle(image,pt2,radian,Scalar(255,255,255),-1,1,0);
        circle(image,pt1,radian,Scalar(255,255,255),-1,1,0);
        //imshow("image",image);
        //line( image, pt1, pt2, randomColor(rng), rng.uniform(1, 10), 8 );
    }
    return 0;
}
static Scalar randomColor( RNG& rng ){
  return Scalar( rand()%255, rand()%255, rand()%255 );
}