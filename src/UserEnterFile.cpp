/* 
這個程式的功能是：
透過攝影機，擷取要的 "速度" "音量" 資訊
 */
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#include "Generate_Play_Midi.h"

using namespace std;
using namespace cv;

/*  *  Function Headers  */
Mat T1 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\1.jpg", 1);
Mat T2 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\2.jpg", 1);
Mat T3 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\3.jpg", 1);
Mat T4 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\4.jpg", 1);
Mat T5 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\5.jpg", 1);
Mat T6 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\6.jpg", 1);
Mat T7 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\7.jpg", 1);
Mat T8 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\8.jpg", 1);
Mat T9 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\9.jpg", 1);
Mat T10 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\10.jpg", 1);
Mat T11 = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\11.jpg", 1);
Mat background1 = imread("Resource\\UI_all_picture/UI PIC/UI/Background_kong.png", 1);

bool detectAndDisplay( Mat frame, float* facex, float* facey, float* facewidth, float* faceheight );
void SamplePicInitial();
// float GaussResult(int a, int b, int c);  // 沒用到
bool DetectHandShakeSpeed();
int DrawMat(Mat Input, Mat& Output, int row, int col);
int DrawTalk(Mat Input, Mat& Output, int row, int col);

/*  *  Global variables  */
// -- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
// C:\\Users\\may\\Desktop\\龔洲暐\\學校\\專題\\VC2010\\opencv

// 用不到
// String face_cascade_name = "C:\\Users\\may\\Desktop\\龔洲暐\\學校\\專題\\VC2010\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
// string window_name = "Capture - Face detection";


static int RedTreshod = 300;
static int UserGoOutWhenPlayingTime = 60000;  // 1Min
static int HandStopShankingTime = 60000;      // 10S

////////////////// 臉部偵測用
float facex;
float facey;
float facewidth;
float faceheight;

///////////////// 高斯運算用
CvScalar MeanScalar;
CvScalar StandardDeviationScalar;



//////////////// 手勢辨識用
// 搬進 Generate_Play_Midi
// int speed = 60;
// int volume = 80;
// bool MusicPlayback = false;
// Mat Output;
// int row_index = 0;
// Mat row_proc_img[40];

float prex = 0;
float prey = 0;
float nowx = 0;
float nowy = 0;
float shakhighest = 0;
float shaklowest = 0;
float shakrange = 0;


// int go_buffer = 0;
int compute_speed_M[6] = {0};
// int compute_volume_M[6] = {0};
int average_speed = 0;
int sum_handY = 0;
int average_volume = 0;
int sum_volume = 0;
int var = 0;

bool handmoveup = false;
bool pre_handmoveup = false;

CvCapture *  capture ;


// time_t LastTimeFineUser;
// time_t OverTimeUserDisapper;

time_t preHandBitClock = clock() + 10000;
time_t nowHandBitClock = clock() + 10000;


void Detect_Volumn(Mat ui_screen, int orbitX[], int orbitY[], int go_orbit, int orbit_num){
    float len_max = sqrt( pow(ui_screen.rows, 2) + pow(ui_screen.cols, 2) );
    // 看三條線
    float orbit_len_acc = 0;
    float orbit_len_avg = 0;
    float orbit_len     = 0;
    int buffer_size = 7;
    int acc_amount = 0;
    for(int i = 0; i < buffer_size ; i++ ){
        int cur_i = go_orbit - i;
        int bef_i = go_orbit - i - 1;
        if(cur_i < 0) cur_i = orbit_num + cur_i;
        if(bef_i < 0) bef_i = orbit_num + bef_i;
        orbit_len  = pow(orbitX[cur_i] - orbitX[bef_i], 2);
        orbit_len += pow(orbitY[cur_i] - orbitY[bef_i], 2);
        orbit_len  = sqrt(orbit_len);
        if(orbit_len > 20)
            orbit_len_acc += orbit_len;
            acc_amount++;
        // cout << "orbit_len:" << orbit_len << endl;
        // cout << "go_orbit:" << go_orbit <<  ", cur_i:" << cur_i << ", bef_i:" << bef_i << endl;
        // line( ui_screen, Point(orbitX[cur_i], orbitY[cur_i]), Point(orbitX[bef_i], orbitY[bef_i]), Scalar(44, 250, 255), 1, 8 );
    }
    // cv::imshow("ui_screen", ui_screen);
    if(acc_amount > 0){
        orbit_len_avg = orbit_len_acc / acc_amount;
        float len_ratio = orbit_len_avg / (len_max * 0.060) * 100;
        // cout << "    len_ratio:" << len_ratio << endl;
        float temp_volume = len_ratio;       
        if(temp_volume <   5) temp_volume =   5;  // 最小音量用 10%
        if(temp_volume > 100) temp_volume = 100;  // 如果超過 100% 就clip掉回100%
        volume = temp_volume / 100 * 127;         // 0 ~ 100 range 縮放成 MIDI音量 0 ~ 127
    }
    
    // cout << "orbit_len_avg:" << orbit_len_avg << ", len_max:" << len_max << ", temp_volume:" << temp_volume << ", volume:" << volume << endl;
}

int HandShaking(string Title){
    // time_t LastTimeFineUser     = clock();
    // time_t OverTimeUserDisapper = clock()  +  UserGoOutWhenPlayingTime;

	IplImage *  vframe ;

	Mat copyFrame;
	Mat newimg;
	Mat frame;

	double MinValue;
	double MaxValue;

	Point MinLocation;
	Point MaxLocation;


    // 臉部辨識 內建的東西~~來判斷是不是人臉
	// if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

    // 開始視訊
    SamplePicInitial();  // 初始化 MeanScalar 和 StandardDeviationScalar
    Mat sample_color;
	capture = cvCaptureFromCAM( -1 );
    if(capture){
        frame = cvQueryFrame( capture );
        sample_color = Mat( int(frame.rows / 2), int(frame.cols / 2), CV_8UC3, Scalar(MeanScalar.val[0], MeanScalar.val[1], MeanScalar.val[2]));
    }

    int go_orbit = 0;
	int orbit_num = 8;
	int orbitX[8] = {0};
	int orbitY[8] = {0};

    int talktime = 0;
    Mat talk;
    Mat talk_roi_ord = Output(Rect(700, 130, T1.cols * 0.7, T1.rows * 0.7)).clone();
    Mat talk_roi     = Output(Rect(700, 130, T1.cols * 0.7, T1.rows * 0.7));
    Mat Output2;
    Mat frame_show;

	// 偵測手環是否進入範圍內
	if(capture){
	    Mat ReducePic;
        Mat ReducePicShow;
        int returnval = 1;
        int go_frame = 0;
		while( MusicPlayback){
		    // cout << "APPNOWTIME" << LastTimeFineUser << endl;
            // cout << "ENDSTEPTIME" << OverTimeUserDisapper << endl;
			frame = cvQueryFrame( capture );

			if( !frame.empty() ){
			  // printf("OK!!!!\n");
			}
            else{
			    printf(" --(!) No captured frame -- Break!"); break;
			}

            // delay 1 毫秒 抓一次圖
			int c = waitKey(1);
			if( (char)c ==  'c' ) { break; }


			// 臉部偵測的function
			resize(frame, newimg, Size(int(frame.cols / 2), int(frame.rows / 2)), 0, 0, INTER_CUBIC);
            // cv::imshow("newimg", newimg);
            // cv::waitKey(0);

            // 偵測 顏色最相近的點 在哪裡
            int MinRow = 0;
            int MinCol = 0;
            int MinValue = 300;

            Mat distance;
            Mat sample_color_f;
            Mat newimg_f;
            newimg      .convertTo(newimg_f      , CV_32F);
            sample_color.convertTo(sample_color_f, CV_32F);
            pow(newimg_f - sample_color_f,  2.0, distance);
            vector<Mat> channels;
            split(distance, channels);
            distance = channels[0] + channels[1] + channels[2];
            sqrt(distance, distance);
            double minVal; double maxVal; Point minLoc; Point maxLoc;
            minMaxLoc( distance , &minVal, &maxVal, &minLoc, &maxLoc);
            MinCol = minLoc.x;
            MinRow = minLoc.y;


            nowx = MinCol;
            nowy = MinRow;
            if(!DetectHandShakeSpeed()){
                preHandBitClock = clock();
                cout << "DetectHandShakeSpeed" << endl;
                returnval = 3;
            }

            prex = MinCol;
            prey = MinRow;

            // 畫出 最新偵測到的 顏色位置 orbitXY, 用 藍色圈圈 表示
            circle(newimg, cvPoint(MinCol, MinRow), 4, Scalar(250, 0, 0), 2, 8, 0);

            // 軌跡buffer 存入 顏色最相近的點
            orbitX[go_orbit] = MinCol;
            orbitY[go_orbit] = MinRow;

            Detect_Volumn(newimg, orbitX, orbitY, go_orbit, orbit_num);

            // 畫出 orbit 裡面的點 連成的 線, 最後一個點不用連回頭所以-1
			for(int i = 0; i < orbit_num - 1 ; i++ ){
                int cur_i = go_orbit - i;
                int bef_i = go_orbit - i - 1;
                if(cur_i < 0) cur_i = orbit_num + cur_i;
                if(bef_i < 0) bef_i = orbit_num + bef_i;

                // cout << "go_orbit:" << go_orbit <<  ", cur_i:" << cur_i << ", bef_i:" << bef_i << endl;
                
                line( newimg, Point(orbitX[cur_i], orbitY[cur_i]), Point(orbitX[bef_i], orbitY[bef_i]), Scalar(44, 250, 3), 1, 8 );
                // line( newimg, Point(orbitX[i], orbitY[i]), Point(orbitX[(i + 1) % 8], orbitY[(i + 1) % 8]), Scalar(44, 250, 3), 1, 8 );
				// circle(newimg, cvPoint(orbitX[i], orbitY[i]), 2, Scalar(44, 250, 3), 2, 8, 0);
			}

            // 更新 軌跡buffer的index
            if(go_orbit < orbit_num-1) go_orbit++  ;
            else                       go_orbit = 0;

            // 貼到UI前 先縮小到UI指定的大小
            resize(newimg, newimg, Size(frame.cols * 0.537, frame.rows * 0.537), 0, 0, INTER_CUBIC);

            // 把東西貼上 UI Output(寫在 Generate_Play_Midi 跟 PlaySnd共用)
            if(!Output.empty()){
                
                // 把 畫完圖的frame 貼上 Output
                frame_show = Output(Rect(16, 77, newimg.cols, newimg.rows));
                cv::flip(newimg, newimg, 1);  // 左右翻轉
                newimg.copyTo(frame_show);

                // 把 五線譜組 貼上 Output                
                int roi_height = row_proc_img[row_index].rows;
                int roi_width  = row_proc_img[row_index].cols;
                if(roi_height > 227) roi_height = 227;  // 高度最多抓 227
                Mat ui_staff_roi    = Output                 (Rect(62, 530, roi_width, roi_height));
                Mat staff_staff_roi = row_proc_img[row_index](Rect( 0,   0, roi_width, roi_height));
                staff_staff_roi.copyTo(ui_staff_roi);
            }

            // 
            Output2 = Output.clone();
            if(clock() > talktime){
                talktime = clock() + 5000 + (rand() % 10 * 1000);
                switch(1 + rand() % 11){
                case 1:
                    resize(T1, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 2:
                    resize(T2, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 3:
                    resize(T3, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 4:
                    resize(T4, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 5:
                    resize(T5, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 6:
                    resize(T6, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 7:
                    resize(T7, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 8:
                    resize(T8, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 9:
                    resize(T9, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 10:
                    resize(T10, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 11:
                    resize(T11, talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                }

                // cout << "talktime" << talktime << endl;
                // cout << "clock()" << clock() << endl;
            }
            
            
            talk_roi_ord.copyTo(talk_roi);     // 先把上次的結果還原回原始UI
            DrawTalk(talk, Output, 130, 700);  // 再貼上新的Talk圖片

            imshow(Title, Output);
            go_frame++;
		}
		cvReleaseCapture(&capture);
        return returnval;
	}

}

/*  * 
  *  @function detectAndDisplay
  */
bool detectAndDisplay( Mat frame, float* facex, float* facey, float* facewidth, float* faceheight ){
	std::vector<Rect> faces;
    Mat frame_gray;
    
    // float facex = 0;
    // float facey = 0;
    // float facewidth = 0;
    // float faceheight = 0;
    
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    // -- Detect faces

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(1, 1) );
    if(faces.size() == 0)
        return false;

    int BiggestFace = 10;
    int MinFaceLengh = frame.rows + frame.cols;
    for( int i = 0; i < faces.size(); i ++  ){
        if((faces[i].width + faces[i].height) < MinFaceLengh){
            MinFaceLengh = faces[i].width + faces[i].height;
            BiggestFace = i;
        }


	     *facex = faces[i].x;
	     *facey = faces[i].y;
	     *facewidth = faces[i].width;
	     *faceheight = faces[i].height;

    }
    if(BiggestFace != 10){
        Point center( faces[BiggestFace].x  +  faces[BiggestFace].width * 0.5, faces[BiggestFace].y  +  faces[BiggestFace].height * 0.5 );
        rectangle(frame, cvPoint(faces[BiggestFace].x , faces[BiggestFace].y), cvPoint(faces[BiggestFace].x  + faces[BiggestFace].width , faces[BiggestFace].y  + faces[BiggestFace].height), 2, 3, 0);
        // cout << "face size x = " << faces[BiggestFace].width << " y = " << faces[BiggestFace].height << endl;
    }



     // -- Show what you got
     // imshow( window_name, frame );
     // imwrite( "C:\\Users\\user\\Desktop\\test\\result.jpg", frame );
     return true;
}



void SamplePicInitial(){
    // 舊版 opencv 寫法, 要搭配 cvShowImage, cvWaitKey 才可以顯示喔
    // IplImage* src = cvLoadImage("testmode4.jpg", 1);
    // IplImage* src = cvLoadImage("Resource/hand_color/testmode4.jpg", 1);
    IplImage* src = cvLoadImage("Resource/hand_color/red_pen.jpg", 1);
    // IplImage* src = cvLoadImage("Resource/hand_color/yellow.jpg", 1);
    cvAvgSdv(src, &MeanScalar, &StandardDeviationScalar);

    // cout << "src:" << src << endl;
    // cout << "Blue  Channel MeanScalar.val             :" << MeanScalar.val[0]              << endl;
    // cout << "Blue  Channel StandardDeviationScalar.val:" << StandardDeviationScalar.val[0] << endl;
    // cout << "Green Channel MeanScalar.val             :" << MeanScalar.val[1]              << endl;
    // cout << "Green Channel StandardDeviationScalar.val:" << StandardDeviationScalar.val[1] << endl;
    // cout << "Red   Channel MeanScalar.val             :" << MeanScalar.val[2]              << endl;
    // cout << "Red   Channel StandardDeviationScalar.val:" << StandardDeviationScalar.val[2] << endl;
    // cvShowImage("src", src);
    // cvWaitKey(0);

}

bool DetectHandShakeSpeed(){
   if(a > 5) a = 0;
   if((nowy - prey) > 10){
        handmoveup = false;
    }
    else if((nowy - prey) < -10){

        handmoveup = true;
    }
    if(pre_handmoveup != handmoveup && pre_handmoveup == true){   // 表示改變方向
        shakhighest = nowy;
        nowHandBitClock = clock();

        // cout << "a " << a << endl;
        compute_speed_M[a] = nowHandBitClock - preHandBitClock;
        sum_handY = 0;
        for(int i = 0;i < 6;i ++ ){
            sum_handY = sum_handY + compute_speed_M[i];
        }

        average_speed = sum_handY / 6;
        int dev[6] = {0};
        for(int i = 0;i < 6;i ++ ){
            dev[i] =  compute_speed_M[i] - average_speed;
        }


        sort(dev, dev + 6);
        // for(int i = 0;i < 6;i ++ )
        //     cout << "dev[" << i << "] " << dev[i] << endl;

        var = 0;
        for(int i = 0;i < 4;i ++ )
            var = var + pow(dev[i], 2);
        // cout << "                 ~~~~var " << var << endl;
        if(var < 60000000){
            int k = 60000/((dev[0] + dev[1] + dev[2] + dev[3]) / 4 + average_speed + 1);
            if(k < 300 && k>20)
                speed = k;
            // speed = 60000/(endclock-startclock);
            cout << "---------------------------speed      :" << speed << endl << endl ;
        }


        a += 1;
        // cout << "---------------------------during time:" << endclock-startclock << endl << endl ;

        preHandBitClock = clock();
        //////////////////////////////////// clock
    }
    // else if(pre_handmoveup != handmoveup && pre_handmoveup == false){
    //     shaklowest = nowy;
    //     shakrange = abs(shakhighest - shaklowest);
    //     // cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!Hand shakrange" << shakrange << endl;
    //     compute_volume_M[go_buffer] = shakrange;
    //     sum_volume = 0;
    //     for(int i = 0; i < 6; i ++ ){
    //         sum_volume = sum_volume + compute_volume_M[i];
    //     }

    //     average_volume = sum_volume / 6;
    //     average_volume = 60 + (2 * average_volume);
    //     // for(int i = 0; i < 6; i ++ )
    //     //     cout << "compute_volume_M[" << i << "] " << compute_volume_M[i] << endl;
    //     // cout << "average_volume " << average_volume << endl;
    //     if(average_volume < 127) volume = average_volume;
    //     // cout << "change volume:" << volume << endl;

    // }

    pre_handmoveup = handmoveup;
    int NOWTIME = clock();
    cout << "NOWTIME-preHandBitClock" << NOWTIME-preHandBitClock << endl;
    // if( abs(NOWTIME - preHandBitClock) > HandStopShankingTime ) return false;

    return true;
}


int DrawMat(Mat Input, Mat& Output, int row, int col){
    int OutputRow = Output.rows;
    int OutputCol = Output.cols;
    int InputRow = Input.rows;
    int InputCol = Input.cols;

    if(row<OutputRow && col < OutputCol){
        for(int i = row;i < (row + InputRow);i ++ ){
            for(int j = col;j < (col + InputCol);j ++ ){
                Output.at<Vec3b>(i, j)[0] = Input.at<Vec3b>(i-row, j-col)[0];
                Output.at<Vec3b>(i, j)[1] = Input.at<Vec3b>(i-row, j-col)[1];
                Output.at<Vec3b>(i, j)[2] = Input.at<Vec3b>(i-row, j-col)[2];
            }
        }

    }else{
        cout << "Draw Out of Range" << endl;
        waitKey(0);
    }
    // imshow("Output", Output);
    // waitKey(0);
    return 0;
}
int DrawTalk(Mat Input, Mat& Output, int row, int col){
    int OutputRow = Output.rows;
    int OutputCol = Output.cols;
    int InputRow = Input.rows;
    int InputCol = Input.cols;
    if(row < OutputRow && col < OutputCol){
        for(int i = row;i < (row + InputRow);i ++ ){
            for(int j = col;j < (col + InputCol);j ++ ){
                if(Input.at<Vec3b>(i-row, j-col)[0] < 200)
                    Output.at<Vec3b>(i, j) = Input.at<Vec3b>(i-row, j-col);
            }
        }

    }
    else{
        cout << "Draw Out of Range" << endl;
        waitKey(0);
    }
    // imshow("Output", Output);
    // waitKey(0);
    return 0;
}


