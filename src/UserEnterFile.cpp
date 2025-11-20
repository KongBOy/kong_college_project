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
#include <ctime>

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

void SamplePicInitial();
bool Detect_Speed();
int DrawTalk(Mat Input, Mat& Output, int row, int col);




///////////////// 高斯運算用
CvScalar MeanScalar;
CvScalar StandardDeviationScalar;



//////////////// 手勢辨識用
// 搬進 Generate_Play_Midi
// int speed = 60;
// int volume = 80;
// bool MusicPlayback = false;
// Mat UI_Output;
// int row_index = 0;
// Mat row_proc_img[40];

float prex = 0;
float prey = 0;
float nowx = 0;
float nowy = 0;
float shakhighest = 0;
float shaklowest = 0;
float shakrange = 0;

int  clock_cost_buffer_size = 6;  // 實際 clock_cost_buffer_size 大小
int  clock_cost_buffer_acc   = 0;  // 虛擬 clock_cost_buffer_size 大小, 大概就是假設 buffer無限大 總共input了幾次 clock_cost 進buffer的概念, 也可以代表 最新可以寫入 buffer的位置
int  clock_cost_buffer_go    = 0;  // clock_cost_buffer_acc % clock_cost_buffer_size 為 實際可以寫進buffer的位置
int* clock_cost_buffer  = new int[clock_cost_buffer_size];
int  clock_cur_posi = 0;

int clock_cost_avg = 0;
int clock_cost_sum = 0;

int average_volume = 0;
int sum_volume = 0;

bool now_handmoveup = false;
bool pre_handmoveup = false;

CvCapture *  capture ;

time_t pre_handmove_up_clock = clock() + 10000;
time_t now_handmoveup_clock  = clock() + 10000;


void Detect_Volumn(Mat ui_screen, int orbitX[], int orbitY[], int go_orbit, int orbit_num){
    float len_max = sqrt( pow(ui_screen.rows, 2) + pow(ui_screen.cols, 2) );
    // 看 avg_buffer_size 條線
    float orbit_len_acc = 0;
    float orbit_len_avg = 0;
    float orbit_len     = 0;
    int avg_buffer_size = 7;
    int acc_amount = 0;
    for(int i = 0; i < avg_buffer_size ; i++ ){
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
        if(temp_volume <   5) temp_volume =   5;  // 最小音量用  5%
        if(temp_volume > 100) temp_volume = 100;  // 如果超過 100% 就clip掉回100%
        volume = temp_volume / 100 * 127;         // 0 ~ 100 range 縮放成 MIDI音量 0 ~ 127
    }
    
    // cout << "orbit_len_avg:" << orbit_len_avg << ", len_max:" << len_max << ", temp_volume:" << temp_volume << ", volume:" << volume << endl;
}

int HandShaking(string Title){
    // 初始化 clock_cost_buffer
    for(int i = 0; i < clock_cost_buffer_size; i++) clock_cost_buffer[i] = 0;

	Mat frame;
	Mat frame_small;
	Mat frame_small_fit_ui;

	double MinValue;
	double MaxValue;

	Point MinLocation;
	Point MaxLocation;

    // 初始化 指定物品的顏色 的 MeanScalar 和 StandardDeviationScalar
    SamplePicInitial();
    Mat sample_color;

    // 開始視訊
    // 1. 建立 VideoCapture 物件並打開攝影機
    VideoCapture cap(0);
    // 2. 檢查攝影機是否成功開啟
    if (!cap.isOpened()) {
        cout << "error, cannot open camera." << endl;
        return -1;
    }

    // 3. 先抓一張影像來取出 frame 大小, 生成 sample_color影像
    cap.read(frame);
    sample_color = Mat( int(frame.rows / 2), int(frame.cols / 2), CV_8UC3, Scalar(MeanScalar.val[0], MeanScalar.val[1], MeanScalar.val[2]));

    int go_orbit = 0;
	int orbit_num = 8;
	int orbitX[8] = {0};
	int orbitY[8] = {0};

    int talktime = 0;
    Mat talk;
    Mat talk_roi_ord = UI_Output(Rect(700, 130, T1.cols * 0.7, T1.rows * 0.7)).clone();
    Mat talk_roi     = UI_Output(Rect(700, 130, T1.cols * 0.7, T1.rows * 0.7));
    Mat frame_on_ui;

	// 如果 視訊 有正常開啟
	if(cap.isOpened()){

        int status = 1;
        int go_frame = 0;
		while( MusicPlayback){
			cap.read(frame);
			if( frame.empty() ){
                printf(" --(!) No captured frame -- Break!");
                break;
            }

            // delay 1 毫秒 抓一次圖
			int c = waitKey(1);
			if( (char)c ==  'c' ) { break; }

            // frame 縮小處理效果差不多 但 會快很多
			resize(frame, frame_small, Size(int(frame.cols / 2), int(frame.rows / 2)), 0, 0, INTER_CUBIC);
            // cv::imshow("frame_small", frame_small);
            // cv::waitKey(0);

            // 偵測 frame_small內顏色 與 指定物品的顏色 最相近的點 在哪裡
            int MinRow = 0;
            int MinCol = 0;
            int MinValue = 300;

            Mat distance;
            Mat sample_color_f;
            Mat frame_small_f;
            //  (b1 - b2)^2, (g1 - g2)^2, (r1 - r2)^2
            frame_small .convertTo(frame_small_f , CV_32F);
            sample_color.convertTo(sample_color_f, CV_32F);
            pow(frame_small_f - sample_color_f,  2.0, distance);
            //  (b1 - b2)^2 + (g1 - g2)^2 + (r1 - r2)^2
            vector<Mat> channels;
            split(distance, channels);
            distance = channels[0] + channels[1] + channels[2];
            // ( (b1 - b2)^2 + (g1 - g2)^2 + (r1 - r2)^2 ) ^0.5
            sqrt(distance, distance);
            // 找最小距離(顏色最相近)的點在哪裡
            double minVal; double maxVal; Point minLoc; Point maxLoc;
            minMaxLoc( distance , &minVal, &maxVal, &minLoc, &maxLoc);
            MinCol = minLoc.x;
            MinRow = minLoc.y;

            // 軌跡buffer 存入 顏色最相近的點
            orbitX[go_orbit] = MinCol;
            orbitY[go_orbit] = MinRow;
            
            // 用 軌跡判斷 音量
            Detect_Volumn(frame_small, orbitX, orbitY, go_orbit, orbit_num);

            // 用 y的變化來算速度
            nowy = MinRow;
            Detect_Speed();     
            prey = nowy;  // nowy用完 變成下次的 prey 囉


            // 畫出 最新偵測到的 顏色位置 orbitXY, 用 藍色圈圈 表示
            circle(frame_small, cvPoint(MinCol, MinRow), 4, Scalar(250, 0, 0), 2, 8, 0);
            // 畫出 orbit 裡面的點 連成的 線, 最後一個點不用連回頭所以-1
			for(int i = 0; i < orbit_num - 1 ; i++ ){
                int cur_i = go_orbit - i;
                int bef_i = go_orbit - i - 1;
                if(cur_i < 0) cur_i = orbit_num + cur_i;
                if(bef_i < 0) bef_i = orbit_num + bef_i;
                // cout << "go_orbit:" << go_orbit <<  ", cur_i:" << cur_i << ", bef_i:" << bef_i << endl;
                
                line( frame_small, Point(orbitX[cur_i], orbitY[cur_i]), Point(orbitX[bef_i], orbitY[bef_i]), Scalar(44, 250, 3), 1, 8 );
				// circle(frame_small, cvPoint(orbitX[i], orbitY[i]), 2, Scalar(44, 250, 3), 2, 8, 0);
			}

            // 更新 軌跡buffer的index
            if(go_orbit < orbit_num-1) go_orbit++  ;
            else                       go_orbit = 0;

            // 貼到UI前 先縮小到UI指定的大小
            resize(frame_small, frame_small_fit_ui, Size(frame.cols * 0.537, frame.rows * 0.537), 0, 0, INTER_CUBIC);

            // 把東西貼上 UI_Output(寫在 Generate_Play_Midi 跟 PlaySnd共用)
            if(!UI_Output.empty()){
                // 把 畫完圖的frame 貼上 UI_Output
                frame_on_ui = UI_Output(Rect(16, 77, frame_small_fit_ui.cols, frame_small_fit_ui.rows));
                cv::flip(frame_small_fit_ui, frame_small_fit_ui, 1);  // 左右翻轉
                frame_small_fit_ui.copyTo(frame_on_ui);

                // 把 五線譜組 貼上 UI_Output                
                int roi_height = row_proc_img[row_index].rows;
                int roi_width  = row_proc_img[row_index].cols;
                if(roi_height > 227) roi_height = 227;  // 高度最多抓 227
                Mat ui_staff_roi    = UI_Output                 (Rect(62, 530, roi_width, roi_height));
                Mat staff_staff_roi = row_proc_img[row_index](Rect( 0,   0, roi_width, roi_height));
                staff_staff_roi.copyTo(ui_staff_roi);
            }

            // UI 隨機 挑出 11段對話文字 來畫
            if(clock() > talktime){
                talktime = clock() + 5000 + (rand() % 10 * 1000);
                switch(1 + rand() % 11){
                case 1:
                    resize(T1 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 2:
                    resize(T2 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 3:
                    resize(T3 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 4:
                    resize(T4 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 5:
                    resize(T5 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 6:
                    resize(T6 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 7:
                    resize(T7 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 8:
                    resize(T8 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
                    break;
                case 9:
                    resize(T9 , talk, Size(T1.cols * 0.7, T1.rows * 0.7), 0, 0, INTER_CUBIC);
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
            DrawTalk(talk, UI_Output, 130, 700);  // 再貼上新的Talk圖片
            imshow(Title, UI_Output);
            go_frame++;
		}
		cvReleaseCapture(&capture);
        return status;
	}
}

void SamplePicInitial(){
    // 舊版 opencv 寫法, 要搭配 cvShowImage, cvWaitKey 才可以顯示喔
    // IplImage* src = cvLoadImage("testmode4.jpg", 1);
    // IplImage* src = cvLoadImage("Resource/hand_color/testmode4.jpg", 1);
    IplImage* src = cvLoadImage("Resource/hand_color/laser_fold4.jpg", 1);
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

bool Detect_Speed(){
    // nowy: 偵測前 的 y
    // prey: 偵測後 的 y, 相當於下一次的nowy 的 前一次
    if     ( (nowy - prey) >  10) now_handmoveup = false;  // 動作往下
    else if( (nowy - prey) < -10) now_handmoveup = true;   // 動作往上
    
    // pre_handmoveup != now_handmoveup 表示改變方向, now_handmoveup == true 代表 最後的動作是往上的時候
    if(pre_handmoveup != now_handmoveup && now_handmoveup == true){
        // 紀錄 上次 到 這次 的 改變方向 且 最後動作是往上的 時間花了多久 存進buffer裡
        now_handmoveup_clock = clock();                                                    // 紀錄此時時間
        clock_cur_posi = clock_cost_buffer_acc % clock_cost_buffer_size;                   // 定位實際buffer可儲存的位置
        clock_cost_buffer[clock_cur_posi] = now_handmoveup_clock - pre_handmove_up_clock;  // 花的時間存進去
        // 顯示一下目前clock_cost_buffer
        for(int i = 0; i < clock_cost_buffer_size; i++ )
            if(i <= clock_cost_buffer_acc) cout << "clock_cost_buffer[" << i << "] " << clock_cost_buffer[i] << endl;

        // 抓出 目前花多久時間 current ~ current前n個 時間點的距離算平均 來算速度, n越大越不會那麼敏感一下就變速度
        float clock_sum = 0;
        float clock_avg = 0;
        int avg_buffer_size = 2;
        int acc_amount = 0;
        for(int go_avg = 0; go_avg < avg_buffer_size ; go_avg++ ){
            int cur_avg_i = clock_cur_posi - go_avg;
            if(cur_avg_i < 0) cur_avg_i = clock_cost_buffer_size + cur_avg_i;
            clock_sum += clock_cost_buffer[cur_avg_i];
            acc_amount++;
        }
        clock_avg = clock_sum / acc_amount;

        // 60秒可以切幾分 clock_avg 就是 60秒可以打幾下, 就是 bpm 囉
        int speed_temp = 60000 / (clock_avg + 0.00000001);
        cout << "clock_cost_buffer_acc: " << clock_cost_buffer_acc << ", acc_amount:" << acc_amount << ", clock_avg:" << clock_avg << ", speed_temp:" << speed_temp << endl;
        // 把太極端的速度去除後 才設定成 我們要的速度, 補充一下不要用clip因為速度容易受到雜訊干擾超過300, 超過300就等於300的話 很容易一值被拉到300
        if(20 < speed_temp && speed_temp < 300 ) speed = speed_temp;
        // 加速時常常揮太快超過300 結果發現沒加速 又揮更快, 所以超過300 且 speed 沒超過300 就慢慢 +5
        if(speed_temp > 300 and speed < 300) speed += 5;

        // 更新 buffer 目前可儲存位置
        clock_cost_buffer_acc += 1;
        // now_handmove_up_clock用完了 可以更新成 pre_handmove_up_clock 了
        pre_handmove_up_clock = now_handmoveup_clock;
    }
    
    // now_handmoveup用完了 可以更新成 pre_handmoveup 了
    pre_handmoveup = now_handmoveup;
    return true;
}


// Input圖去白色背景(200以上) 貼進 Output圖
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


