/*
這個程式的功能是：
找出 輸入譜的圖片 其可能是五線譜的線
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "preprocess_3_horizon_map_to_find_line.h"
#include "preprocess_0_watch_hough_line.h"

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



void Horizon_map_to_find_line(Mat src_img ,vector<Vec2f> & mountain_lines2 , Mat & containor , string file_name,Mat ord_img,Mat test_bin){
    const int width = src_img.cols - THRESH_HOLD;
    const int distance_range_min = src_img.rows / (STAFF_COUNT_MAX*2);
    const int distance_range_max = src_img.rows / (STAFF_COUNT_MIN*2);
    const int mountain_area_min = STAFF_LINE_WIDTH_MIN * ERROR_RATE * width; //一條五線譜線 占的面積
    const int mountain_area_max = STAFF_LINE_WIDTH_MAX * ERROR_RATE * width + (TAIL + NOTE + CIRCLE_LINE + WORD + ELSE) * width; //一組五線譜 + 16分的尾巴 + 一堆NOTE + 圓滑線 + 字 + 其他雜訊
    cout<<"src_img.cols = "<<src_img.cols<<endl;
    cout<<"width = "<<width<<endl;
    cout<<"distance_range_min = "<<distance_range_min<<endl;
    cout<<"mountain_area_min = "<<mountain_area_min<<endl;
    cout<<"mountain_area_max = "<<mountain_area_max<<endl;
 //   bool right_side_ok = true;

    int * w_count = new int[src_img.rows];
    for(int i = 0 ; i < src_img.rows ; i++) w_count[i] = 0;

    int l_edge[100];
    int r_edge[100];
    int distance[100];
    int mountain_area[100];
    for(int i = 0 ; i < 100 ; i++){
        l_edge[i] = 0;
        r_edge[i] = 0;
        distance[i] = 0;
        mountain_area[i] = 0;
    }
    int e_count = 0;
    for(int go_row = 0 ; go_row < src_img.rows; go_row++){
        for(int go_col = 0 ; go_col < src_img.cols ; go_col++){
            if(src_img.at<uchar>(go_row,go_col) == 255) containor.at<uchar>(go_row,w_count[go_row]++) = 255;
        }

        if(go_row > 0){
            if(w_count[go_row] > THRESH_HOLD && w_count[go_row -1] <= THRESH_HOLD){ //&& right_side_ok == true)
                l_edge[e_count] = go_row -1;
                // right_side_ok = false;
            }
            else if(w_count[go_row] <= THRESH_HOLD && w_count[go_row -1] > THRESH_HOLD){
                // if(go_row - l_edge[e_count] < distance_range_min) continue;


                r_edge[e_count] = go_row;
                distance[e_count] = r_edge[e_count] - l_edge[e_count];

                for(int i = l_edge[e_count] +1 ; i < r_edge[e_count] ; i++) mountain_area[e_count] += w_count[i];

                // right_side_ok = true;
                // Mat proc_img = src_img(Rect(THRESH_HOLD,l_edge[e_count], width, distance[e_count]));
                // imshow("test",proc_img);
                // waitKey(0);
                e_count++;
            }
        }
        // if(go_row < 650 && go_row > 600) cout<<"go_row = "<<go_row<<" , w_count = "<<w_count[go_row] <<endl;
    }


// step 3 start
/////////////////////////////////////////////////////////////////////////////////////////
// 把 可能不是五線譜的山 刪掉囉~~
/////////////////////////////////////////////////////////////////////////////////////////
    for(int i = 0 ; i < e_count ; i++){
         //cout<<"i = "<<i<<" , l_edge = "<<l_edge[i]<<" , r_edge = "<<r_edge[i]<<" , distance = "<<distance[i]<<" , mountain = "<<mountain_area[i]<<endl;
        if(mountain_area[i] > mountain_area_min && mountain_area[i] < mountain_area_max) ; //do nothing
        else{
            //刪掉自己，右邊的東西往左平移
            for(int j = i ; j < e_count-1 ; j++ ){
                l_edge[j] = l_edge[j+1];
                r_edge[j] = r_edge[j+1];
                distance[j] = distance[j+1];
                mountain_area[j] = mountain_area[j+1];
            }
            i--;
            e_count--;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    // step 3 end




    ///////////////// debug ////////////////////
    /*
    for(int i = 0 ; i < e_count ; i++)
    {
            cout<<"i = "<<i<<" , l_edge = "<<l_edge[i]<<" , r_edge = "<<r_edge[i]<<" , distance = "<<distance[i]<<" , mountain = "<<mountain_area[i]<<endl;
            Mat proc_img = src_img(Rect(0,l_edge[i], width, distance[i]));

            imshow("test",proc_img);
            // waitKey(0);
    }
    cout<<endl;
    */
    ///////////////// debug ////////////////////


    ///////////////// debug ////////////////////

    // vector<Vec2f> mountain_lines2;
    // vector<Vec5f> staff_lines;
    // int staff_count = 0;

    for(int i = 0 ; i < e_count ; i++){
        cout<<"i = "<<i<<" , l_edge = "<<l_edge[i]<<" , r_edge = "<<r_edge[i]<<" , distance = "<<distance[i]<<" , mountain = "<<mountain_area[i]<<endl;
        Mat proc_img = src_img(Rect(0,l_edge[i], width, distance[i]));

        vector<Vec2f> lines;

        string file_name;
        lines.clear();
        int hough_w_r_slider = 85;
        // imshow("test",proc_img);
        // waitKey(0);
        while(lines.size() < 5 && hough_w_r_slider >= 80){///90改88
            HoughLines(proc_img, lines  , 1 , CV_PI/180, src_img.cols * hough_w_r_slider / 100 , 0  , 0 );
            hough_w_r_slider -= 3;
            // Watch_Hough_Line(lines,proc_img,"test","test");
            // waitKey(0);
        }



        if(lines.size() > 0){
            // cout<<"find_line!!!"<<endl;
            for(int j = 0 ; j < lines.size() ; j++){
                Vec2f temp_data2;
                temp_data2[0] = (int)(l_edge[i] * sin(lines[j][1]) + lines[j][0]);
                temp_data2[1] = lines[j][1];
                mountain_lines2.push_back(temp_data2);
            }
        }
        else{ // 如果這座山找不到線，就刪掉自己，右邊的東西往左平移
        
            // 刪掉自己，右邊的東西往左平移
            for(int j = i ; j < e_count-1 ; j++ ){
                l_edge[j] = l_edge[j+1];
                r_edge[j] = r_edge[j+1];
                distance[j] = distance[j+1];
                mountain_area[j] = mountain_area[j+1];
            }
            i--;
            e_count--;
        }
        // imshow("test",proc_img);
        // waitKey(0);
        // ***********************************************************************
        // Watch_Hough_Line(lines,proc_img,(string)HORIZONTAL_DIR + "mountain_hough_line",(string)HORIZONTAL_DIR + "mountain_hough_line");
        // waitKey(0);
    }

}
