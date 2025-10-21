/*
這個程式的功能是：
找出 所有符號可能出現的水平位置
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_0_vertical_map_to_speed_up.h"

#define AREA_THRESHOLD 20
#define DISTANCE_THRESHOLD 10
#define THRESH_HOLD 2
#define BLACK 0
using namespace cv;
using namespace std;

void Show_mountain(Mat staff_img_erase_line, Mat staff_img_erase_line_color, Mat vertical_map, int e_count, int l_edge[200], int r_edge[200], int distance[200], int b_count[], Scalar color, int thickness){
    for(int i = 0 ; i < e_count ; i++){
        cout<<"e_count = "<<e_count<<endl;
        cout<<"l_edge["<<i<<"]="<<l_edge[i]<<endl;
        cout<<"r_edge["<<i<<"]="<<r_edge[i]<<endl;
        cout<<"distance["<<i<<"]="<<distance[i]<<endl;

        for(int j = l_edge[i]+1 ; j < r_edge[i] ; j++) cout<<"ord_b_count["<<j<<"]= "<<b_count[j]<<endl;
        cout<<endl;

        cv::rectangle(staff_img_erase_line_color, cv::Rect(l_edge[i], 0, distance[i], staff_img_erase_line.rows), color, thickness);
        cv::rectangle(vertical_map              , cv::Rect(l_edge[i], 0, distance[i], staff_img_erase_line.rows), color, thickness);
        Mat mountain_cut_staff_img_erase_line = staff_img_erase_line(Rect(l_edge[i], 0, distance[i], staff_img_erase_line.rows ));
        Mat mountain_cut_vertical_map         = vertical_map        (Rect(l_edge[i], 0, distance[i], staff_img_erase_line.rows ));
        imshow("staff_img_erase_line_color", staff_img_erase_line_color);
        imshow("vertical_map", vertical_map);
        imshow("mountain_cut_staff_img_erase_line", mountain_cut_staff_img_erase_line);
        imshow("ord_vertical_img_cut"             , mountain_cut_vertical_map);
        waitKey(0);
        destroyWindow("mountain_cut_staff_img_erase_line");
        destroyWindow("ord_vertical_img_cut");
    }



}

void recognition_0_vertical_map_to_speed_up(Mat staff_img_erase_line, int& e_count, int l_edge[200], int r_edge[200], int distance[200], int mountain_area[200], int*& note_type, bool debuging){
    Mat staff_img_erase_line_color;  // debug用
    if(debuging){
        cv::cvtColor(staff_img_erase_line, staff_img_erase_line_color, COLOR_GRAY2BGR);
        cv::imshow("staff_img_erase_line_color", staff_img_erase_line_color);
        cvMoveWindow("staff_img_erase_line_color", 10, 10);
    }

    // 初始化
    int * b_count = new int[staff_img_erase_line.cols];  //black count
    for(int i = 0 ; i < staff_img_erase_line.cols ; i++) b_count[i] = 0;
    
    // vertical_map 是debug用, 這邊是視覺化 垂直投影長什麼樣子
    Mat vertical_map;
    if(debuging){
        // 垂直投影, 主要更新 b_count, vertical_map 是 debug用的, 視覺化來看 THRESH_HOLD 取多少比較好
        vertical_map = Mat(staff_img_erase_line.rows, staff_img_erase_line.cols, CV_8UC1, Scalar(255));
        for(int go_row = 0 ; go_row < staff_img_erase_line.rows; go_row++){
            for(int go_col = 0 ; go_col < staff_img_erase_line.cols ; go_col++){
                if(staff_img_erase_line.at<uchar>(go_row,go_col) == BLACK) vertical_map.at<uchar>(b_count[go_col]++, go_col) = 0;
            }
        }

        // 灰階轉彩色
        cv::cvtColor(vertical_map, vertical_map, COLOR_GRAY2BGR);
        // THRESH_HOLD的紅線
        cv::line(vertical_map, cv::Point(0, THRESH_HOLD), cv::Point(staff_img_erase_line.cols , THRESH_HOLD), Scalar(0, 0, 255), 1);
        // 電腦影像 起始座標是左上角, 但我們數學上常見的坐標系起點是左下角, 所以上下顛倒 比較好看
        cv::flip(vertical_map, vertical_map, 0);
        // 顯示出來
        imshow("vertical_map", vertical_map);
        cvMoveWindow("vertical_map", 10, 250);
        waitKey(0);
    }

    // 初始化 找山的相關容器, l_edge: 山的左邊位置, r_edge: 山的右邊位置
    e_count = 0;
    for(int i = 0 ; i < 200 ; i++){
        l_edge       [i] = 0;
        r_edge       [i] = 0;
        distance     [i] = 0;
        mountain_area[i] = 0;
    }

    //////////////////////
    // 找出l_edge, r_edge
    //////////////////////
    for(int go_col = 1 ; go_col < staff_img_erase_line.cols ; go_col++){
        // 如果 左低 右高, 代表找到 左邊界
        if     (b_count[go_col -1] <= THRESH_HOLD && b_count[go_col] > THRESH_HOLD){ //&& right_side_ok == true)
            l_edge[e_count] = go_col -1;
        }
        // 如果 左高 右低, 代表找到 右邊界
        else if(b_count[go_col -1] > THRESH_HOLD && b_count[go_col] <= THRESH_HOLD){
            r_edge[e_count] = go_col;

            // 找到右邊界, 就可以更新 distance 和 area 了
            distance[e_count] = r_edge[e_count] - l_edge[e_count];
            // 從目前的左邊界 走道 目前的右邊界, 把 走訪的b_count都加起來 就是面積囉
            for(int i = l_edge[e_count] +1 ; i < r_edge[e_count] ; i++)
                mountain_area[e_count] += b_count[i];

            e_count++;
        }
    }
    //////////////////////// debug 看一下目前的山 ////////////////////////
    if(debuging) Show_mountain(staff_img_erase_line, staff_img_erase_line_color, vertical_map, e_count, l_edge, r_edge, distance, b_count, Scalar(255, 0, 0), 1);

    ////////////////////////
    // 距離小的合併
    ////////////////////////
    for(int go_mountain = 0 ; go_mountain < e_count-1 ; go_mountain++){
        if( (l_edge[go_mountain+1]-r_edge[go_mountain]) < DISTANCE_THRESHOLD ){
            //cout<<"merge"<<endl;
            r_edge       [go_mountain]  = r_edge[go_mountain + 1];
            distance     [go_mountain]  = r_edge[go_mountain] - l_edge[go_mountain];
            mountain_area[go_mountain] += mountain_area[go_mountain+1];
            for(int go_erase = go_mountain+1 ; go_erase < e_count-1 ; go_erase++){
                l_edge       [go_erase] = l_edge       [go_erase+1];
                r_edge       [go_erase] = r_edge       [go_erase+1];
                distance     [go_erase] = distance     [go_erase+1];
                mountain_area[go_erase] = mountain_area[go_erase+1];
            }
            e_count--;
            go_mountain--;
        }
    }

    ////////////////////////
    // 連附點都不是的山刪除
    ////////////////////////
    for(int go_mountain = 0 ; go_mountain < e_count-1 ; go_mountain++){
        if( mountain_area[go_mountain] < AREA_THRESHOLD ){
            //cout<<"merge"<<endl;
            for(int go_erase = go_mountain ; go_erase < e_count-1 ; go_erase++){
                l_edge       [go_erase] = l_edge       [go_erase+1];
                r_edge       [go_erase] = r_edge       [go_erase+1];
                distance     [go_erase] = distance     [go_erase+1];
                mountain_area[go_erase] = mountain_area[go_erase+1];
            }
            e_count--;
            go_mountain--;
        }
    }

    //////////////////////// debug 看一下目前的山 ////////////////////////
    if(debuging) Show_mountain(staff_img_erase_line, staff_img_erase_line_color, vertical_map, e_count, l_edge, r_edge, distance, b_count, Scalar(255, 255, 0), 2);

}
