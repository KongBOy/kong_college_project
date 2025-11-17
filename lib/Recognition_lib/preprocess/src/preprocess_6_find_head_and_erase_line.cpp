/*
這個程式的功能是：
利用 preprocess_5 找出的 五線譜 資訊
找出各組五線譜 的端點
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <math.h>

#include "Recognition.h"
#include "preprocess_6_find_head_and_erase_line.h"

#include "preprocess_5_find_staff.h"
#include "preprocess_0_hough_tool.h"

//#include "include/find_staff.h"
#include "preprocess_0_watch_hough_line.h"


#define PI 3.14159
#define ROI_WIDTH 10
#define CHECK_SUCESS 0
#define CHECK_FAILED -1

#define CHECK_LINE_LENGTH 10
#define CODA_RATE 0.25
#define JUMP_SPACE_LENGTH 30
#define ERROR_CODA 6

#define STAFF_LINE_COUNT 5
#define X_Y_COUNT 2


///#define HORIZONTAL_DIR "horizontal_test/"
#define HORIZONTAL_DIR "debug_img/pre6_"



using namespace cv;
using namespace std;

static string file_name;

// debug 畫線用
static Mat src_bin_reduce_line_debug;
static Mat src_bin_debug;
static Mat drew_img_debug;
static Point pt1, pt2;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//interface，先把東西包成對的格式，再丟到find_head找頭
void Recognition_page::Find_Head_and_Erase_Line_Interface(){
    debuging = debuging_pre6;
    Mat color_src_img;  // debug用
    cvtColor(src_img, color_src_img, CV_GRAY2BGR);

    src_bin_erase_line = src_bin.clone();

    drew_img_debug = color_src_img.clone();
    src_bin_reduce_line_debug = src_bin.clone();
    cvtColor(src_bin, src_bin_debug, CV_GRAY2BGR);

    // staff = new vector<Vec2f>[staff_count];
    if(debuging) cout << "staff_count:" << staff_count << endl;
    left_point  = new int**[staff_count];  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    right_point = new int**[staff_count];  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]

    for(int i = 0 ; i < staff_count ; i++){
        left_point [i] = new int*[STAFF_LINE_COUNT];
        right_point[i] = new int*[STAFF_LINE_COUNT];


        for(int go_width = 0 ; go_width < STAFF_LINE_COUNT ; go_width++){
            left_point [i][go_width] = new int[X_Y_COUNT];
            right_point[i][go_width] = new int[X_Y_COUNT];

            for(int k = 0 ; k < X_Y_COUNT ; k++){
                left_point [i][go_width][k] =  10000; /// 隨便很大的數
                right_point[i][go_width][k] = -10000; /// 隨便很小的數
            }
        }
    }


    // 要注意格式喔~~~包成五個五個為一組！！！
    for(int i = 0 ; i < staff_count ; i++){
        int first_line = 5*i;
        int fifth_line = 5*(i+1)-1;

        vector<Vec2f> staff;
        for(int go_width = first_line ; go_width <= fifth_line ; go_width++){
            staff.push_back(lines[go_width]);
            // cout << "go_width = " << go_width << " data = " << lines[go_width][0] << " " << lines[go_width][1] << endl;
        }
        Find_Head_and_Erase_Line( staff, (string)HORIZONTAL_DIR + "find_head",src_bin,left_point[i], right_point[i], src_bin_erase_line, debuging);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//interface，先把東西包成對的格式，再丟到find_head找頭
void Find_Head_and_Erase_Line_Interface(Mat src_bin, vector<Vec2f>staff_lines, int staff_count, int***& left_point, int***& right_point, Mat color_ord_img, Mat& src_bin_erase_line, bool debuging){
    drew_img_debug = color_ord_img.clone();
    src_bin_reduce_line_debug = src_bin.clone();
    cvtColor(src_bin, src_bin_debug, CV_GRAY2BGR);

    // staff = new vector<Vec2f>[staff_count];
    if(debuging) cout << "staff_count:" << staff_count << endl;
    left_point  = new int**[staff_count];  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    right_point = new int**[staff_count];  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]

    for(int i = 0 ; i < staff_count ; i++){
        left_point [i] = new int*[STAFF_LINE_COUNT];
        right_point[i] = new int*[STAFF_LINE_COUNT];


        for(int go_width = 0 ; go_width < STAFF_LINE_COUNT ; go_width++){
            left_point [i][go_width] = new int[X_Y_COUNT];
            right_point[i][go_width] = new int[X_Y_COUNT];

            for(int k = 0 ; k < X_Y_COUNT ; k++){
                left_point [i][go_width][k] =  10000; /// 隨便很大的數
                right_point[i][go_width][k] = -10000; /// 隨便很小的數
            }
        }
    }


    // 要注意格式喔~~~包成五個五個為一組！！！
    for(int i = 0 ; i < staff_count ; i++){
        int first_line = 5*i;
        int fifth_line = 5*(i+1)-1;

        vector<Vec2f> staff;
        for(int go_width = first_line ; go_width <= fifth_line ; go_width++){
            staff.push_back(staff_lines[go_width]);
            // cout << "go_width = " << go_width << " data = " << staff_lines[go_width][0] << " " << staff_lines[go_width][1] << endl;
        }
        Find_Head_and_Erase_Line( staff, (string)HORIZONTAL_DIR + "find_head",src_bin,left_point[i],right_point[i], src_bin_erase_line, debuging);
    }
}



void Erase_line(Mat& src_bin, int x0, int y0, int one_step, int one_step_height, int go_range){
    for(int go = 0 ; go < go_range ; go++){
        // 這個點的上面和下面，然後測試之間的距離，如果小於 線寬 就代表是線 就消線~~
        int up = y0 + go*one_step*one_step_height;
        // 定位, 如果沒有在線上的話，往上下找五格~~~
        if( (src_bin.at<uchar>(up ,x0+go*one_step) != 0) ){
            for(int i = 0 ; i < 5 ; i++){
                if(src_bin.at<uchar>(up+i  ,x0+go*one_step) == 0){
                    up+=i;
                    break;
                }
                else if((src_bin.at<uchar>(up-i  ,x0+go*one_step) == 0)){
                    up-=i;
                    break;
                }
            }
        }

        //如果up沒有定位成功，就不做拉~~~因為可能是在 空白處~~~
        if(src_bin.at<uchar>(up  ,x0 + go*one_step) != 0){
            // cout << "erase line white do nothing" << endl;
            // do nothing~~ 以下是debug訊息
            // cout << "something wrong~~" << endl;
            // imshow("where", src_bin( Rect(x0 - 80, up - 80, 240, 240) ));
            // waitKey(0);
        }
        else{
            // cout << "erase line black" << endl;
            // imshow("where", src_bin( Rect(x0 - 30, up - 30,  60,  60) ));
            // waitKey(0);
            int down = up;

            while(src_bin.at<uchar>(up  , x0 + go*one_step) == 0 && up   > 0            + 1) up--;
            while(src_bin.at<uchar>(down, x0 + go*one_step) == 0 && down < src_bin.rows - 1) down++;
            down--;

            int distance = down - up;
            if(distance <= 7){
                for(int i = 0 ; i <= distance ; i++ ) src_bin.at<uchar>(up + i, x0 + go*one_step) = 255;
            }
            /*debug
            if(distance >= 10 && distance <= 20){
                cout << "y0 = " << y0 << " x0 = " << x0 + go*one_step;
                cout << " distance = " << distance << endl;
                imshow("where", src_bin( Rect(x0 + go*one_step-25, up - 25, 50, 50) ));
                waitKey(0);
            }
            */
        }
    }
    // cout << " go_range = " << go_range;
    // cout << " end_clean" << endl;
}




int Check_shift(Mat, int,int,int,int);
// 用來測試某個 點 可能是否在線上
// 門檻值為 CHECK_LINE_LENGTH * CODA_RATE
// 回傳值：
//   如果可能  是  在線上，回傳可能程度
//   如果可能 不是 在線上，回傳失敗值
int Check_shift(Mat src_bin, int x0, int y0, int one_step, double one_step_height){
    int count = 0;
    for(int i = 0 ; i < CHECK_LINE_LENGTH ; i++){
        if     ( src_bin.at<uchar>(y0 + i * one_step_height * one_step    ,x0 + i * one_step) == 0 ) count++;
        else if( src_bin.at<uchar>(y0 + i * one_step_height * one_step -1 ,x0 + i * one_step) == 0 ) count++;
        else if( src_bin.at<uchar>(y0 + i * one_step_height * one_step +1 ,x0 + i * one_step) == 0 ) count++;

    }
    if( (float)count >= (float)CHECK_LINE_LENGTH * CODA_RATE ) return count;
    else return CHECK_FAILED;

}

void Debug_draw_dot(double x0, double y0, Scalar color, int dot_size){
    pt1.x = cvRound(x0);
    pt1.y = cvRound(y0);
    if(dot_size == 1){
        drew_img_debug.at<Vec3b>(y0, x0) = Vec3b(color[0], color[1], color[2]); 
        src_bin_debug .at<Vec3b>(y0, x0) = Vec3b(color[0], color[1], color[2]); 
    }
    else{
        line( drew_img_debug, pt1, pt1, color, dot_size, CV_AA);
        line( src_bin_debug , pt1, pt1, color, dot_size, CV_AA);
    }
}

void Debug_draw_line(Point pt1, Point pt2, Scalar color, int dot_size){
    if(dot_size < 2) dot_size = 2;
    line( drew_img_debug, pt1, pt2, color, dot_size, CV_AA);
    line( src_bin_debug , pt1, pt2, color, dot_size, CV_AA);

}

// 已經把消線分開寫了，所以把所有 Mat & 拿掉囉~~
void Find_Head_and_Erase_Line(vector<Vec2f> lines, string window_name, Mat src_bin, int**& left_point, int**& right_point, Mat& src_bin_erase_line, bool debuging){
    if(debuging) cout << "Find_Head_and_Erase_Line" << endl;

    // 走訪每一條線, 把線頭找出來
	for(int go_line = 0; go_line < lines.size(); go_line++ ){
        float rho = lines[go_line][0], theta = lines[go_line][1];
		double angle_value = (theta/PI)*180;
		if(debuging){
            cout << "go_line = " << go_line << " ,";
            cout << "rho     = " << rho << ", angle_value = " << angle_value << endl;
        }

		// 起點走多少
		int width  = src_bin.cols;
		int height = src_bin.rows;
		// 因為我們是用 "擷取正中間左右10%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
		int center_roi_start_x = ( (double)width / 2 ) - ((double)width) * ((double)ROI_WIDTH / 100 /2); //ROI_WIDTH是col的百分比
        
        // 計算在線上走一步的高度, 用hough裡面用 rho, theta 本身代表的線 是 垂直於 找到的那條線,
        // rho, theta 本身代表的線的斜率 是 tan(theta), 垂直兩條線 的斜率相乘 == -1,
        // 所以 找到的那條線的 斜率是 -cot(theta)
		double cos_th = cos(theta), sin_th = sin(theta);
        double cot_th = cos_th / sin_th; 
		double one_step_height = -1 * cot_th; //1 / sin_th * -1;

        // "擷取正中間左右 ROI_WIDTH%的圖來做hough" 該圖的 左上角 與 找到的那條線 畫一條線可以與其垂直 的那個點
        double x0 = cos_th * rho, y0 = sin_th * rho;
        x0 += center_roi_start_x;  // 因為我們是用 "擷取正中間左右 ROI_WIDTH%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
        // hough 找出來的線 用 rho * cos_th, rho * sin_th 好像都會有機會偏差一點點不在線上, 所以上下搜尋5個pixel先把起點移到線上
        if(src_bin.at<uchar>(y0, x0) != 0){
            for(int go_shift = 1; go_shift <= 5; go_shift++ ){
                if     (src_bin.at<uchar>(y0 + go_shift, x0) == 0){
                    y0 += go_shift;
                    break;
                }
                else if(src_bin.at<uchar>(y0 - go_shift, x0) == 0){
                    y0 -= go_shift;
                    break;
                }
            }
        }
        if(debuging){
            cout << "width  = " << width  << endl;
            cout << "height = " << height << endl;
            cout << "rho            : " << rho << endl;
            cout << "x0             : " << x0 << endl;
            cout << "y0             : " << y0 << endl;
            Debug_draw_dot(x0, y0, Scalar(0, 102, 255), 3);  // 橘色
        }
        


        double one_step;
        for(int direction = 0 ; direction < 2 ; direction++){
            // ..._go 是會隨著找頭的進行而變動的, ..._th 是初始值不變的, 在找頭的一開始 都事先指定 初始值, 之後再隨著找頭來變動
            float  theta_go = theta;
            double cos_go = cos_th, sin_go = sin_th;
            double cot_go = cos_go / (sin_go + 0.000000001); 
            double one_step_height_go = -1 * cot_go;
            // 方向
            switch(direction){
                case 0:
                    one_step = 1;
                    break;
                case 1:
                    one_step = -1;
                    break;
            }
            if(debuging){
                cout << "cos_go         : " << cos_go << endl;
                cout << "sin_go         : " << sin_go << endl;
                cout << "one_step       : " << one_step << endl;
                cout << "one_step_height_go: " << one_step_height_go << endl;
                cout << endl;
            }

            Erase_line(src_bin_erase_line, x0, y0, one_step, one_step_height, 5);

            double next_x = x0;
            double next_y = y0;
            for(int go_width = 0 ; go_width < width/2 + width * ((float)ROI_WIDTH/(float)100); go_width++){ // 做多做 width/2 + width * ((float)ROI_WIDTH/(float)100) 次
                // 順著線走下一格
                next_x += one_step;
                next_y += one_step * one_step_height_go;

                ////////////////// Range防呆 ///////////////////
                if(next_y >= 0 && next_y < height && next_x >= 0 && next_x < width);//do nothing
                else break;
                ////////////////// Range防呆 ///////////////////
                
                // 順著線走 如果現在是黑點, 代表在線上, 繼續走下一格
                if(src_bin.at<uchar>(next_y, next_x) == 0){
                    if(debuging){
                        Debug_draw_dot(next_x, next_y, Scalar(0, 242, 255), 1);  // 黃色
                    }
                    Erase_line(src_bin_erase_line, next_x, next_y, one_step, one_step_height, 5);
                    continue;
                }
                
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // 順著線走 如果現在不是黑點, 
                //   一: 有可能還在線上 但 因為畫質差 線段在二值化出現空洞
                //   二: 有可能已經走到盡頭
                // 希望排除第一點, 所以順著線走 往右探勘, 如果探不到 再 順著線走 往上下探勘
                for(int go_jump = 0; go_jump < JUMP_SPACE_LENGTH; go_jump++){
                    // Debug_draw_dot(next_x + go_jump * one_step, next_y + go_jump * one_step * one_step_height_go, Scalar(165, go_jump * 4, 96 + go_jump * 4), 1);  // 深深紫色
                    // case1 就往線方向的右邊探勘 CHECK_LINE_LENGTH * CODA_RATE 格 希望調整到在線上這樣子
                    bool go_Right_on_line_flag = false;
                    if(src_bin.at<uchar>(next_y + go_jump*one_step*one_step_height_go, next_x + go_jump*one_step) != 0){
                        //往右探勘 CHECK_LINE_LENGTH * CODA_RATE 格
                        for(int go_R = 1 ; go_R < CHECK_LINE_LENGTH * CODA_RATE ; go_R++){
                            //探勘的格子 如果高機率是線 且 該格是黑點, 就是在線上的點囉, 就移動過去這樣子
                            if(   Check_shift(src_bin, next_x + go_R*one_step , next_y + go_R*one_step*one_step_height_go +0, one_step, one_step_height_go ) != CHECK_FAILED
                                    && src_bin.at<uchar>(next_y + go_R*one_step*one_step_height_go +0 ,next_x + go_R*one_step) == 0 ){
                                for(int go_erase = 1; go_erase <= go_R; go_erase++){
                                    Erase_line(src_bin_erase_line, next_x + go_erase*one_step, next_y + go_erase*one_step*one_step_height_go, one_step, one_step_height, 5);
                                }

                                        
                                // 畫一下現在位置
                                pt1.x = next_x, pt1.y = next_y;
                                
                                next_x += go_R*one_step;
                                next_y += go_R*one_step*one_step_height_go ;
                                go_Right_on_line_flag = true;
                                
                                // 畫一下現在位置
                                pt2.x = next_x, pt2.y = next_y;
                                if(debuging){
                                    cout << "  Forward found case" << endl;
                                    Debug_draw_line(pt1, pt2, Scalar(255, 0, 0), 1);  // 藍色
                                }
                                break;
                            }
                        }
                    }
                    // 往右探勘已經順利找到點了, 就可以跳出 jump_sapce 不用做 上下探勘囉
                    if(go_Right_on_line_flag) break;
                    // 如果沒有順利探勘成功, next_x, next_y 也會在原地不動呦要注意別再搞混了~~~


                    // case2 往線方向走 的 正上下探勘各一格看哪個比較好
                    // 如果有需要做到這個，就一定代表右邊 CHECK_LINE_LENGTH 個格子 都可能不是線！！往線方向走的 上下一格 找找看
                    bool go_UpDown_on_line_flag = false;
                    if(src_bin.at<uchar>(next_y, next_x + go_jump*one_step) != 0){
                        // 往線方向走 的 正上下一格 找找看
                        int cmp_up   = Check_shift(src_bin, next_x + go_jump*one_step , next_y + go_jump*one_step*one_step_height_go - 1 ,one_step,one_step_height_go);
                        int cmp_down = Check_shift(src_bin, next_x + go_jump*one_step , next_y + go_jump*one_step*one_step_height_go + 1 ,one_step,one_step_height_go);

                        // 往線方向走 的 正下一格 如果看起來比 正上一格 更像線 且 該格是黑點, 移動到那一格(往線的方向移動 和 往正下方偏移一格)
                        if(cmp_up >= cmp_down && cmp_up != CHECK_FAILED && src_bin.at<uchar>(next_y + go_jump*one_step*one_step_height_go -1, next_x + go_jump*one_step) == 0){
                            for(int go_erase = 1; go_erase <= go_jump; go_erase++){
                                Erase_line(src_bin_erase_line, next_x + go_erase*one_step, next_y + go_erase*one_step*one_step_height_go, one_step, one_step_height, 5);
                            }
                            go_UpDown_on_line_flag = true;
                            
                            // 畫一下現在位置
                            pt1.x = next_x, pt1.y = next_y;
                            
                            next_x += go_jump*one_step;
                            next_y += go_jump*one_step*one_step_height_go ;
                            next_y--;
                            
                            // 畫一下現在位置
                            pt2.x = next_x, pt2.y = next_y;
                            if(debuging){
                                Debug_draw_line(pt1, pt2, Scalar(0, 255, 0), 1);  // 綠色
                            }

                            Erase_line(src_bin_erase_line, next_x, next_y, one_step, one_step_height, 5);
                            
                            if(debuging){
                                cout<< "  UP before" << endl;
                                cout<< "    theta_go: " << theta_go << endl;
                                cout<< "    one_step_height_go: " << one_step_height_go <<endl;
                            }
                            theta_go -= (PI / 180.0) * 0.05; // -0.05 度
                            cos_go = cos(theta_go), sin_go = sin(theta_go);
                            cot_go = cos_go / (sin_go + 0.000000001); 
                            one_step_height_go = -1 * cot_go;
                            if(debuging){
                                cout<< "  UP after" << endl;
                                cout<< "    theta_go: " << theta_go << endl;
                                cout<< "    one_step_height_go: " << one_step_height_go <<endl<<endl;
                            }
                        }
                        // 往線方向走 的 正上一格 如果看起來比 正下一格 更像線 且 該格是黑點, 移動到那一格(往線的方向移動 和 往正上方偏移一格)
                        else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED && src_bin.at<uchar>(next_y +go_jump*one_step*one_step_height_go +1 ,next_x+go_jump*one_step) == 0 ){ //if( cmp_up != CHECK_FAILED && cmp_down == CHECK_FAILED)                                
                            for(int go_erase = 1; go_erase <= go_jump; go_erase++){
                                Erase_line(src_bin_erase_line, next_x + go_erase*one_step, next_y + go_erase*one_step*one_step_height_go, one_step, one_step_height, 5);
                            }
                            go_UpDown_on_line_flag = true;

                            // 畫一下現在位置
                            pt1.x = next_x, pt1.y = next_y;
                            
                            next_x += go_jump*one_step;
                            next_y += go_jump*one_step*one_step_height_go ;
                            next_y++;
                            
                            // 畫一下現在位置
                            pt2.x = next_x, pt2.y = next_y;
                            if(debuging){
                                Debug_draw_line(pt1, pt2, Scalar(0, 0, 255), 1);  // 紅色
                            }

                            Erase_line(src_bin_erase_line, next_x, next_y, one_step, one_step_height, 5);
                            
                            if(debuging){
                                cout<< "  DOWN before" << endl;
                                cout<< "    theta_go: " << theta_go << endl;
                                cout<< "    one_step_height_go: " << one_step_height_go <<endl;
                            }
                            theta_go += (PI / 180.0) * 0.05; // +0.05 度
                            cos_go = cos(theta_go), sin_go = sin(theta_go);
                            cot_go = cos_go / (sin_go + 0.000000001); 
                            one_step_height_go = -1 * cot_go;
                            if(debuging){
                                cout<< "  DOWN after" << endl;
                                cout<< "    theta_go: " << theta_go << endl;
                                cout<< "    one_step_height_go: " << one_step_height_go <<endl<<endl;
                            }
                        }
                        
                    }
                    if(go_UpDown_on_line_flag) continue;
                }

                

                // 經過 上面的 jump space 已經排除了 線誤刪的狀況, 如果仍然沒辦法 把點移動到線上的話
                //     一: 代表線很可能是彎曲了, 或者線微幅彎曲但被符號誤導了以為還沒彎曲後累積放大後就變彎曲了
                //     二: 有可能就是已經到盡頭了
                //     補充一下: 走到這裡 next_x, next_y 都還沒被更新喔～

                // 彎曲測試： 和上面不同的是 這裡是測 原地的上下方的狀況, 跟上面不同(上面是測 順著線走的正上下方的狀況)
                // 往上、下偵測ERROR_CODA格
                if(src_bin.at<uchar>(next_y,next_x) != 0){
                    // 如果線字的點不是黑色的，就代表還是有些偏差，希望可以位移到比較正確的位置
                    // (不一定要移動到一定黑色的點才行，只要移到 "被認可是線的地方即可")

                    // 第一種寫法：移動相同距離，來比較往up還往down移動比較好
                    for(int go_UD_range = 0 ; go_UD_range <= ERROR_CODA ; go_UD_range++){

                        ////////////////// Range防呆 ///////////////////
                        if(next_y >= 0+go_UD_range && next_y < height-go_UD_range && next_x >= 0+go_UD_range && next_x < width-go_UD_range);//do nothing
                        else break;
                        ////////////////// Range防呆 ///////////////////

                        // 比較一下 上面 還是 下面 比較像線 就往哪邊走
                        int cmp_up   = Check_shift(src_bin, next_x, next_y-go_UD_range, one_step, one_step_height_go);
                        int cmp_down = Check_shift(src_bin, next_x, next_y+go_UD_range, one_step, one_step_height_go);
                        if( cmp_up >= cmp_down && cmp_up != CHECK_FAILED ){
                            for(int go_erase = 1; go_erase <= go_UD_range; go_erase++){
                                Erase_line(src_bin_erase_line, next_x + go_erase*one_step, next_y + go_erase*one_step*one_step_height_go, one_step, one_step_height, 5);
                            }
                            // 畫一下現在位置
                            pt1.x = next_x, pt1.y = next_y;

                            next_y -= go_UD_range;
                            
                            // 畫一下現在位置
                            pt2.x = next_x, pt2.y = next_y;
                            if(debuging){
                                Debug_draw_line(pt1, pt2, Scalar(  0,  22, 165), 5);  // 深灰色
                                cout << "  bend detected up" << endl;
                            }

                            Erase_line(src_bin_erase_line, next_x, next_y, one_step, one_step_height, 5);

                            break;
                        }
                        else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED ){
                            // 畫一下現在位置
                            pt1.x = next_x, pt1.y = next_y;
                            
                            next_y += go_UD_range;
                            
                            // 畫一下現在位置
                            pt2.x = next_x, pt2.y = next_y;
                            if(debuging){
                                Debug_draw_line(pt1, pt2, Scalar(127,  0, 255), 5);  // 淺灰色
                                cout << "  bend detected down" << endl;
                            }

                            Erase_line(src_bin_erase_line, next_x, next_y, one_step, one_step_height, 5);

                            break;
                        }
                    }
                }	

                // 如果 彎曲測試 測完仍然還是沒辦法, 表示已經找到頭囉! 舊 break 結束找頭這樣子
                if(src_bin.at<uchar>(next_y,next_x) != 0 && Check_shift(src_bin, next_x,next_y,one_step,one_step_height_go) ==  CHECK_FAILED){// && error >= ok_error2)
                    break;
                }
                // 做一段 show 一次
                // if(debuging){
                //     cv::imshow("src_bin_debug", src_bin_debug);
                //     cv::waitKey(0);
                // }
		    }

            // 把找到的頭畫出來
            if(debuging){
                Debug_draw_dot(next_x, next_y, Scalar(0, 0, 255), 10);  // 紅色
            }
            
            // 把找到的頭根據方向存進 right_point[][] 或 left_point[][]
            if(direction == 0){
                right_point[go_line][0] = next_x;
                right_point[go_line][1] = next_y;
            }
            else if(direction == 1){
                left_point[go_line][0] = next_x;
                left_point[go_line][1] = next_y;
            }
            if(debuging) cout << "line_ " << go_line << " find head, " << "next_x=" << next_x << " , next_y=" << next_y << endl;
        }

        // 一條線做完 show一次
        if(debuging){
            cv::imshow("src_bin_debug", src_bin_debug);
            cv::waitKey(0);
        }
	}

    // ******************************************************
    if(debuging){
        cv::destroyWindow("src_bin_debug");
        imwrite(window_name + "3.bmp"     , drew_img_debug);
        imwrite(window_name + "_g_img.bmp", src_bin_debug );
        imwrite(window_name + "_g_img_reduce_line.bmp", src_bin_erase_line );
    }
}
