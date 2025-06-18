/*
這個程式的功能是：
找出 輸入譜的圖片 其可能是五線譜的線
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <direct.h>
#include <cstring>
#include <sstream>
#include <iomanip>

#include "preprocess_3_horizon_map_to_find_line.h"
#include "preprocess_0_watch_hough_line.h"
#include "preprocess_0_hough_tool.h"

#define HORIZONTAL_DIR "debug_preprocess/"

//一張a4紙大概最多可以放staff的組數
#define STAFF_COUNT_MAX 15
#define STAFF_COUNT_MIN 6
//先的粗細差很多喔！
#define STAFF_LINE_WIDTH_MIN 1
#define STAFF_LINE_WIDTH_MAX 5


#define TAIL 3*2
#define NOTE 18*0.8
#define CIRCLE_LINE 2
#define WORD 10
#define ELSE 10

#define ERROR_RATE 0.9
//去掉散狀的雜訊
#define THRESH_HOLD 5

using namespace cv;
using namespace std;



// debug用, 看找到的 五線譜山 的長怎樣
void See_mountain(const Mat & src_img, int e_count, int l_edge[], int r_edge[], int distance[], int mountain_area[], string dir_path, string file_name){
    // 整張影像 用框框出候選區域 的影像  , 預設 debug_img/mountain.jpg
    // 框出的候選影像區域 各自切出來的影像 , 預設 debug_img/mountain/各個框內影像.jpg
    if(file_name == "") file_name = "mountain";
    if(dir_path  == "") dir_path  = "debug_img";
    string file_path     = dir_path + "/" + file_name + ".jpg";  // 大圖上畫框存一份
    string file_name_dir = dir_path + "/" + file_name;           // 大圖上畫框裡面的影像也存一份
    _mkdir(file_name_dir.c_str()); // 建立 框內影像 存放的資料夾

    Mat visual_img;
    cvtColor(src_img, visual_img, CV_GRAY2BGR);

    for(int go_m = 0; go_m < e_count; go_m++){
        cout << file_path + "_detail" << endl
             << "go_m = "             << setw(3) << go_m 
             << ", l_edge = "         << setw(4) << l_edge       [go_m]
             << ", r_edge = "         << setw(4) << r_edge       [go_m]
             << ", distance = "       << setw(3) << distance     [go_m]
             << ", mountain_area = "  << setw(5) << mountain_area[go_m]
             << ", ";

        // 大圖上 畫 框
        Point  left_up    (           0, l_edge[go_m]);
        Point  right_down (src_img.cols, l_edge[go_m] + distance[go_m]);
        Scalar color      ((go_m * 5 ) % 180, (go_m * 7 ) % 180, 255); // BGR
        int    thickness = 2;    // 寬度 (-1 表示填滿)
        rectangle(visual_img, left_up, right_down, color, thickness);

        // 框內的影像 存一份, 把檔名先弄好 編號-面積.jpg
        stringstream ss;
        // 編號
        ss << setw(3) << setfill('0') << go_m;
        string str_go_m = ss.str();
        // 清空 ss 給 面積用
        ss.str("");
        ss.clear();
        // 面積
        ss << mountain_area[go_m];
        string str_mountain_area = ss.str();
        // 組起來 編號-面積.jpg
        string visual_img_part_path = file_name_dir + "/" + str_go_m + "-" + str_mountain_area + ".jpg";
        cout << visual_img_part_path << endl;
        // 存圖
        Mat proc_img = src_img(Rect(0, l_edge[go_m], src_img.cols, distance[go_m]));
        imwrite(visual_img_part_path, proc_img);
    }
    // 大圖存一份
    imwrite(file_path, visual_img);
}

// 把 轉正 和 二值化後的影像 裡面的白點 全部移到左邊形成直條圖 會形成像山一樣的形狀, 找出 由五線譜形成的山 並 再 該區間的影像找出五線譜的線
void Horizon_map_to_find_line(Mat src_img, vector<Vec2f> & mountain_lines2, Mat & containor, bool debuging){
    // src_img  : 轉正 和 二值化後的影像
    // containor: src_img 一樣大小的 全黑 影像
    const int width = src_img.cols - THRESH_HOLD;
    const int distance_range_min = src_img.rows / (STAFF_COUNT_MAX*2);
    const int distance_range_max = src_img.rows / (STAFF_COUNT_MIN*2);
    const int mountain_area_min = STAFF_LINE_WIDTH_MIN * ERROR_RATE * width; //一條五線譜線 占的面積
    const int mountain_area_max = STAFF_LINE_WIDTH_MAX * ERROR_RATE * width + (TAIL + NOTE + CIRCLE_LINE + WORD + ELSE) * width; //一組五線譜 + 16分的尾巴 + 一堆NOTE + 圓滑線 + 字 + 其他雜訊
    if(debuging){
        cout<<"src_img.cols = "<<src_img.cols<<endl;
        cout<<"width = "<<width<<endl;
        cout<<"distance_range_min = "<<distance_range_min<<endl;
        cout<<"mountain_area_min = "<<mountain_area_min<<endl;
        cout<<"mountain_area_max = "<<mountain_area_max<<endl;
    }

    // 建立容器
    int * w_count = new int[src_img.rows]; // src_img 的每一個 row 有幾個 pixel 是白色的
    int e_count;             // 山的數量
    int l_edge[100];         // 山的左邊
    int r_edge[100];         // 山的右邊
    int distance[100];       // 山的row數
    int mountain_area[100];  // 山的白點總數

    // 初始化容器
    for(int i = 0 ; i < src_img.rows ; i++) w_count[i] = 0;
    for(int i = 0 ; i < 100 ; i++){
        l_edge[i] = 0;
        r_edge[i] = 0;
        distance[i] = 0;
        mountain_area[i] = 0;
    }
    e_count = 0;

    // 走訪 src_img 的 每一個row, 找出 山的 左, 右側, row, 面積
    for(int go_row = 0 ; go_row < src_img.rows; go_row++){
        for(int go_col = 0 ; go_col < src_img.cols ; go_col++){
            if(src_img.at<uchar>(go_row,go_col) == 255) containor.at<uchar>(go_row, w_count[go_row]++) = 255;
        }

        if(go_row > 0){
            // 山的左側, 本row 超過 THRESH_HOLD, 前row  沒超過 THRESH_HOLD, 大概如下兩行註解的樣子
            // **       前row
            // ******** 本row
            if(w_count[go_row] > THRESH_HOLD && w_count[go_row -1] <= THRESH_HOLD){
                l_edge[e_count] = go_row -1;
            }
            
            // 山的右側, 本row 沒超過 THRESH_HOLD, 前row  超過 THRESH_HOLD, 大概如下兩行註解的樣子
            // ******** 前row
            // **       本row
            else if(w_count[go_row] <= THRESH_HOLD && w_count[go_row -1] > THRESH_HOLD){
                r_edge[e_count] = go_row;
                distance[e_count] = r_edge[e_count] - l_edge[e_count];
                
                // 保險一點 取 l_edge + 1 和 r_edge -1 的範圍來算面積
                for(int i = l_edge[e_count] +1 ; i < r_edge[e_count] ; i++) mountain_area[e_count] += w_count[i];

                // Mat proc_img = src_img(Rect(THRESH_HOLD, l_edge[e_count],  width, distance[e_count]));
                // imshow("test",proc_img);
                // waitKey(0);
                e_count++;
            }
        }
    }
    if(debuging) See_mountain(src_img, e_count, l_edge, r_edge, distance, mountain_area, "debug_img", "pre3_HorizL_2_1_mountain_1_before");


    // 把 可能不是五線譜的山 刪掉囉~~
    for(int go_m = 0 ; go_m < e_count ; go_m++){
        if(mountain_area[go_m] > mountain_area_min && mountain_area[go_m] < mountain_area_max); //do nothing
        else{
            //刪掉自己，右邊的東西往左平移
            for(int i = go_m ; i < e_count-1 ; i++ ){
                l_edge       [i] = l_edge       [i+1];
                r_edge       [i] = r_edge       [i+1];
                distance     [i] = distance     [i+1];
                mountain_area[i] = mountain_area[i+1];
            }
            go_m--;
            e_count--;
        }
    }
    if(debuging) See_mountain(src_img, e_count, l_edge, r_edge, distance, mountain_area, "debug_img", "pre3_HorizL_2_1_mountain_2_after");


    // 找出五線譜的線
    for(int go_m = 0 ; go_m < e_count ; go_m++){
        Mat proc_img = src_img(Rect(0,l_edge[go_m], width, distance[go_m]));

        vector<Vec2f> lines;

        lines.clear();
        // 影像寬度的幾趴 來當作 HoughLines 的 threshold
        // 怕趴數太高找不到, 會從 hough_w_th_ratio 開始找, 如果找到的線不夠多(< 5條), 就會把 趴數降低一點再搜一次線, 直到 hough_w_th_ratio_low_bound 為止
        int hough_w_th_ratio           = 85;
        int hough_w_th_ratio_low_bound = 80;
        while(lines.size() < 5 && hough_w_th_ratio >= hough_w_th_ratio_low_bound){
            HoughLines(proc_img, lines, 1 , CV_PI/180, src_img.cols * hough_w_th_ratio / 100 , 0  , 0 );
            hough_w_th_ratio -= 3;
            
            if(debuging) {
                string watch_hough_process_name = "debug_img/pre3_HorizL_2_2_Part_line";
                stringstream ss1; ss1 << setw(3) << setfill('0') << go_m;
                stringstream ss2; ss2 << setw(3) << setfill('0') << hough_w_th_ratio;
                watch_hough_process_name += ss1.str() + "_" + ss2.str();
                Watch_Hough_Line(lines, proc_img, "", watch_hough_process_name);
            }
        }

        // 如果這座山有找到線, 丟進去 mountain_lines2 容器內
        if(lines.size() > 0){
            // cout<<"find_line!!!"<<endl;
            for(int i = 0 ; i < lines.size() ; i++){
                Vec2f temp_data2;
                temp_data2[0] = (int)(l_edge[go_m] * sin(lines[i][1]) + lines[i][0]);
                temp_data2[1] = lines[i][1];
                mountain_lines2.push_back(temp_data2);
            }
        }
        // 如果這座山找不到線, 就刪掉自己, 右邊的東西往左平移
        else{ 
            // 刪掉自己，右邊的東西往左平移
            for(int i = go_m ; i < e_count-1 ; i++ ){
                l_edge[i] = l_edge[i+1];
                r_edge[i] = r_edge[i+1];
                distance[i] = distance[i+1];
                mountain_area[i] = mountain_area[i+1];
            }
            go_m--;
            e_count--;
        }
        // ***********************************************************************
    }

    // 找到所有線 對 rho 做排序
    bubbleSort_H_rho(mountain_lines2);
    if(debuging) Watch_Hough_Line(mountain_lines2, src_img, "", (string)"debug_img/" + "pre3_HorizL_2_2_All_line");

}
