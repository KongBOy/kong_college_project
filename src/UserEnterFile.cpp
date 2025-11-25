/* 
這個程式的功能是：
透過攝影機，擷取要的 "速度" "音量" 資訊
 */
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <ctime>

#include "UserEnterFile.h"
#include "Generate_Play_Midi.h"

using namespace std;
using namespace cv;

Camera_HandShaking_Detect::Camera_HandShaking_Detect(Midi_shared_datas* in_midi_shared_datas_ptr):
    orbit_num(ORBIT_NUM),
    go_orbit(0),

    prey(0),
    nowy(0),

    clock_cost_buffer_size(6),
    clock_cost_buffer_acc(0),
    clock_cur_posi(0),

    now_handmoveup(false),
    pre_handmoveup(false),

    pre_handmoveup_clock(clock()),
    now_handmoveup_clock(clock()),
    // 從 Midi_ShowPlay 傳進來的 Midi播放與手勢偵測thread 共用的資料空間(目前裡面有 速度, 音量, MusicPlayback)
    midi_shared_datas_ptr(in_midi_shared_datas_ptr),
    // 偵測 音量/速度 的時候怕太敏感, 設定在某時間內抓一次的clock
    detect_speed_clock_pre (clock()),
    detect_volume_clock_pre(clock())
    {
    // 初始化 self.clock_cost_buffer
    clock_cost_buffer  = new int[clock_cost_buffer_size];
    for(int i = 0; i < clock_cost_buffer_size; i++) clock_cost_buffer[i] = 0;

    for(int i = 0; i < 8; i++){
        orbitX[i] = 0;
        orbitY[i] = 0;
    }
}

void Camera_HandShaking_Detect::set_frame_ptr(Mat* in_frame_ptr){ frame_ptr = in_frame_ptr; }
Mat&  Camera_HandShaking_Detect::get_frame_small_draw_orbit() { return frame_small_draw_orbit; }

void Camera_HandShaking_Detect::Detect_Volumn(){
    float len_max = sqrt( pow(frame_small.rows, 2) + pow(frame_small.cols, 2) );
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
        // line( frame_small, Point(orbitX[cur_i], orbitY[cur_i]), Point(orbitX[bef_i], orbitY[bef_i]), Scalar(44, 250, 255), 1, 8 );
    }
    // cv::imshow("frame_small", frame_small);
    if(acc_amount > 0){
        orbit_len_avg = orbit_len_acc / acc_amount;
        float len_ratio = orbit_len_avg / (len_max * 0.060) * 100;
        // cout << "    len_ratio:" << len_ratio << endl;
        float temp_volume = len_ratio;       
        if(temp_volume <   5) temp_volume =   5;  // 最小音量用  5%
        if(temp_volume > 100) temp_volume = 100;  // 如果超過 100% 就clip掉回100%
        int volume = temp_volume / 100 * 127;         // 0 ~ 100 range 縮放成 MIDI音量 0 ~ 127
        // 手勢偵測 偵測到的速度 設定到 與 Midi播放 共用的資料空間, 抓音量太敏感了 可能需要拉到0.4秒左右抓一次
        time_t detect_volume_clock_now = clock();
        if( (detect_volume_clock_now - detect_volume_clock_pre) >  0.4 * CLOCKS_PER_SEC ){
            midi_shared_datas_ptr -> set_volume(volume);
            detect_volume_clock_pre = detect_volume_clock_now;

        }
    }
    
    // cout << "orbit_len_avg:" << orbit_len_avg << ", len_max:" << len_max << ", temp_volume:" << temp_volume << ", volume:" << volume << endl;
}

DWORD WINAPI Camera_HandShaking_Detect::HandShaking(LPVOID lpParameter){
    Camera_HandShaking_Detect* self_ptr = (Camera_HandShaking_Detect*)lpParameter;
    Camera_HandShaking_Detect& self     = *self_ptr;

    // 初始化 指定物品的顏色 的 MeanScalar 和 StandardDeviationScalar
    self.SamplePicInitial();
    Mat sample_color;
    // 把 存取 frame 從 "->" 改成 "."
    self.frame = *self.frame_ptr;
    sample_color = Mat( int(self.frame.rows / 2), int(self.frame.cols / 2), CV_8UC3, Scalar(self.MeanScalar.val[0], self.MeanScalar.val[1], self.MeanScalar.val[2]));

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 如果正在播音樂(MusicPlayback==True) 且 主frame視訊有正常開啟並傳frame進來 就開始記錄軌跡並偵測手勢
    int status = 1;
    while( self.midi_shared_datas_ptr -> get_MusicPlayback() && !self.frame.empty()){
        // self.frame 縮小處理效果差不多 但 會快很多
        resize(self.frame, self.frame_small, Size(int(self.frame.cols / 2), int(self.frame.rows / 2)), 0, 0, INTER_CUBIC);
        // cv::imshow("self.frame_small", self.frame_small);
        // cv::waitKey(0);

        // 偵測 frame_small內顏色 與 指定物品的顏色 最相近的點 在哪裡
        int MinRow = 0;
        int MinCol = 0;

        Mat distance;
        Mat sample_color_f;
        Mat frame_small_f;
        //  (b1 - b2)^2, (g1 - g2)^2, (r1 - r2)^2
        self.frame_small .convertTo(frame_small_f , CV_32F);
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
        self.orbitX[self.go_orbit] = MinCol;
        self.orbitY[self.go_orbit] = MinRow;
        
        // 用 軌跡判斷 音量
        self.Detect_Volumn();

        // 用 y的變化來算速度
        self.nowy = MinRow;
        self.Detect_Speed();     
        self.prey = self.nowy;  // nowy用完 變成下次的 self.prey 囉


        // frame_small_draw_orbit 從 frame_small 複製一份 並 畫出 最新偵測到的 orbitXY(用藍色圈圈) 和 過去buffer裡面的 orbitXY(用綠色細線)
        // 不能用 .clone(), 應該是因為 不是真的新建一個 new Mat 而是 複製pointer, 所以在取second thread的時候 如果 frame_small已經更新的話, frame_small已經指向新圖片而原本的空間可能會被release掉, frame_small_draw_orbit又指向被release掉的空間來做事情 就會當掉
        // 所以 frame_small_draw_orbit 就手動建立一個新Mat 再把 frame_small 加進來, 就可以保證 frame_small_draw_orbit 指向的東西會存在喔
        self.frame_small_draw_orbit = Mat(self.frame_small.rows, self.frame_small.cols, CV_8UC3, Scalar(0, 0, 0)) + self.frame_small;
        // 畫出 orbit 最新的點(藍色空心點)
        circle(self.frame_small_draw_orbit, cvPoint(MinCol, MinRow), 4, Scalar(250, 0, 0), 2, 8, 0);
        // 畫出 orbit 裡面的點 連成的 線, 最後一個點不用連回頭所以-1(綠色細線)
        for(int i = 0; i < self.orbit_num - 1 ; i++ ){
            int cur_i = self.go_orbit - i;
            int bef_i = self.go_orbit - i - 1;
            if(cur_i < 0) cur_i = self.orbit_num + cur_i;
            if(bef_i < 0) bef_i = self.orbit_num + bef_i;
            // cout << "go_orbit:" << go_orbit <<  ", cur_i:" << cur_i << ", bef_i:" << bef_i << endl;
            
            line( self.frame_small_draw_orbit, Point(self.orbitX[cur_i], self.orbitY[cur_i]), Point(self.orbitX[bef_i], self.orbitY[bef_i]), Scalar(44, 250, 3), 1, 8 );
            // circle(self.frame_small_draw_orbit, cvPoint(self.orbitX[i], self.orbitY[i]), 2, Scalar(44, 250, 3), 2, 8, 0);
        }
        
        // 更新 軌跡buffer的index
        if(self.go_orbit < self.orbit_num-1) self.go_orbit++  ;
        else                                 self.go_orbit = 0;
    }

    CloseHandle(self.gSThread);
    self.gSThread = NULL;
    status = 0;
    return status;
}

void Camera_HandShaking_Detect::thread_HandShaking(){
    DWORD  dwThreadId;
    HANDLE gSThread = CreateThread(NULL, 0, Camera_HandShaking_Detect::HandShaking, this, 0, &dwThreadId);
    if (gSThread == NULL) {
        cerr << "Failed to create thread!" << endl;
        return;
    }
    cout << "thread_HandShaking ---------------" << gSThread << endl;
}


void Camera_HandShaking_Detect::SamplePicInitial(){
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

bool Camera_HandShaking_Detect::Detect_Speed(){
    // nowy: 偵測前 的 y
    // prey: 偵測後 的 y, 相當於下一次的nowy 的 前一次
    if     ( (nowy - prey) >  10) now_handmoveup = false;  // 動作往下
    else if( (nowy - prey) < -10) now_handmoveup = true;   // 動作往上
    
    // pre_handmoveup != now_handmoveup 表示改變方向, now_handmoveup == true 代表 最後的動作是往上的時候
    if(pre_handmoveup != now_handmoveup && now_handmoveup == true){
        // 紀錄 上次 到 這次 的 改變方向 且 最後動作是往上的 時間花了多久 存進buffer裡
        now_handmoveup_clock = clock();                                                    // 紀錄此時時間
        clock_cur_posi = clock_cost_buffer_acc % clock_cost_buffer_size;                   // 定位實際buffer可儲存的位置
        clock_cost_buffer[clock_cur_posi] = now_handmoveup_clock - pre_handmoveup_clock;  // 花的時間存進去
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
        int speed = midi_shared_datas_ptr -> get_speed();
        int speed_temp = 60000 / (clock_avg + 0.00000001);
        cout << "clock_cost_buffer_acc: " << clock_cost_buffer_acc << ", acc_amount:" << acc_amount << ", clock_avg:" << clock_avg << ", speed_temp:" << speed_temp << endl;
        // 把太極端的速度去除後 才設定成 我們要的速度, 補充一下不要用clip因為速度容易受到雜訊干擾超過300, 超過300就等於300的話 很容易一值被拉到300
        if(20 < speed_temp && speed_temp < 300 ) speed = speed_temp;
        // 加速時常常揮太快超過300 結果發現沒加速 又揮更快, 所以超過300 且 speed 沒超過300 就慢慢 +5
        if(speed_temp > 300 and speed < 300) speed += 5;
        // 手勢偵測 偵測到的速度 設定到 與 Midi播放 共用的資料空間, 抓速度比較不敏感不用設多長時間設定一次的限制, 不過寫都寫了就留著備用吧
        time_t detect_speed_clock_now = clock();
        midi_shared_datas_ptr -> set_speed(speed);
        detect_speed_clock_pre = detect_speed_clock_now;

        // 更新 buffer 目前可儲存位置
        clock_cost_buffer_acc += 1;
        // now_handmove_up_clock用完了 可以更新成 pre_handmoveup_clock 了
        pre_handmoveup_clock = now_handmoveup_clock;
    }
    
    // now_handmoveup用完了 可以更新成 pre_handmoveup 了
    pre_handmoveup = now_handmoveup;
    return true;
}
