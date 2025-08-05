#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Note_infos.h"

using namespace cv;

extern int speed;
extern int volume;
extern bool MusicPlayback;
extern Mat Output;
extern int row_index;
extern Mat row_proc_img[40];

extern int MinValue;
extern int MaxValue;
extern int speed_row;
extern int volume_row;

extern RNG rng;

extern double changespeed;



extern Mat Speed_Volume_Bar;
extern Mat background      ;
extern Mat bar             ;






double  Round (double,int);
double  Abs (double);

int  GenerateMidiFile(Note_infos* note_infos, Mat staff_img[]);

int     Sound (float,int=0,int=127,int=0,float=1);
DWORD WINAPI PlaySnd (LPVOID lpParameter);
void PlayMidiFile    (Note_infos* note_infos);

int Drawing_Random_Circles( Mat& image, RNG rng);
static Scalar randomColor( RNG& rng );