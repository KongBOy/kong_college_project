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


void recognition_0_vertical_map_to_speed_up(Mat staff_img_erase_line, Mat & vertical_map, int& e_count, int l_edge[200], int r_edge[200], int distance[200], int mountain_area[200], int*& note_type){
    const int height = staff_img_erase_line.rows - THRESH_HOLD;

    int * b_count = new int[staff_img_erase_line.cols];  //black count
    for(int i = 0 ; i < staff_img_erase_line.cols ; i++) b_count[i] = 0;


    // ////////////////
    // 垂直投影
    // ////////////////
    for(int go_row = 0 ; go_row < staff_img_erase_line.rows; go_row++){
        for(int go_col = 0 ; go_col < staff_img_erase_line.cols ; go_col++){
            if(staff_img_erase_line.at<uchar>(go_row,go_col) == BLACK) vertical_map.at<uchar>(b_count[go_col]++,go_col) = 0;
        }
    }


    Mat threshold_vertical_map = vertical_map(Rect(0,THRESH_HOLD, vertical_map.cols, height ));
    // ******************************
    imshow("threshold_vertical_map",threshold_vertical_map);
    waitKey(0);
    destroyWindow("threshold_vertical_map");

    e_count = 0;
    for(int i = 0 ; i < 200 ; i++){
        l_edge       [i] = 0;
        r_edge       [i] = 0;
        distance     [i] = 0;
        mountain_area[i] = 0;
    }

    // ////////////////////
    // 找出l_edge, r_edge
    // ////////////////////

    for(int go_col = 1 ; go_col < staff_img_erase_line.cols ; go_col++){
        if(b_count[go_col] > THRESH_HOLD && b_count[go_col -1] <= THRESH_HOLD){ //&& right_side_ok == true)
            l_edge[e_count] = go_col -1;
        }
        else if(b_count[go_col] <= THRESH_HOLD && b_count[go_col -1] > THRESH_HOLD){
            r_edge[e_count] = go_col;
            distance[e_count] = r_edge[e_count] - l_edge[e_count];

            for(int i = l_edge[e_count] +1 ; i < r_edge[e_count] ; i++)
                mountain_area[e_count] += b_count[i];

            // //////////////////////// debug用~~ ///////////////////////////////
            // for(int i = l_edge[e_count]+1 ; i < r_edge[e_count] ; i++) cout<<"ord_b_count["<<i<<"]= "<<b_count[i]<<endl;
            // cout<<endl;

            // Mat threshold_final_rl_img_roi_cut = staff_img_erase_line(Rect(l_edge[e_count],THRESH_HOLD, distance[e_count], height ));
            // Mat ord_vertical_map_cut = vertical_map(Rect(l_edge[e_count],0, distance[e_count], staff_img_erase_line.rows ));
            // imshow("threshold_final_rl_img_roi_cut",threshold_final_rl_img_roi_cut);
            // imshow("ord_vertical_img_cut",ord_vertical_map_cut);
            // waitKey(0);
            // destroyWindow("threshold_final_rl_img_roi_cut");
            // destroyWindow("ord_vertical_img_cut");
            // ///////////////////////////////////////////////////////////////

            e_count++;
        }
    }

    // //////////////////////
    // 距離小的合併
    // //////////////////////
    for(int go_mountain = 0 ; go_mountain < e_count-1 ; go_mountain++){
        if( (l_edge[go_mountain+1]-r_edge[go_mountain]) < DISTANCE_THRESHOLD ){
            //cout<<"merge"<<endl;
            r_edge[go_mountain] = r_edge[go_mountain+1];
            distance[go_mountain] = r_edge[go_mountain] - l_edge[go_mountain];
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

    // //////////////////////
    // 連附點都不是的山刪除
    // //////////////////////
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

    // //////////////////////// debug用~~ ///////////////////////////////
    /*
    for(int i = 0 ; i < e_count ; i++){
        cout<<"e_count = "<<e_count<<endl;
        cout<<"l_edge["<<i<<"]="<<l_edge[i]<<endl;
        cout<<"r_edge["<<i<<"]="<<r_edge[i]<<endl;
        cout<<"distance["<<i<<"]="<<distance[i]<<endl;

        for(int j = l_edge[i]+1 ; j < r_edge[i] ; j++) cout<<"ord_b_count["<<j<<"]= "<<b_count[j]<<endl;
        cout<<endl;


        Mat threshold_final_rl_img_roi_cut = staff_img_erase_line(Rect(l_edge[i],THRESH_HOLD, distance[i], height ));
        Mat ord_vertical_map_cut = vertical_map(Rect(l_edge[i],0, distance[i], staff_img_erase_line.rows ));
        imshow("threshold_final_rl_img_roi_cut",threshold_final_rl_img_roi_cut);
        imshow("ord_vertical_img_cut",ord_vertical_map_cut);
        waitKey(0);
        destroyWindow("threshold_final_rl_img_roi_cut");

        destroyWindow("ord_vertical_img_cut");
    }
    */
    // //////////////////////////////////////////////////////////////
}
