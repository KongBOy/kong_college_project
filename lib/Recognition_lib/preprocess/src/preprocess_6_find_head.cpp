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


#include "preprocess_6_find_head.h"

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
static Mat src_bin_debug;
static Mat drew_img_debug;
static Point pt1;


//interface，先把東西包成對的格式，再丟到find_head找頭
void Find_Head_Interface(Mat src_bin,vector<Vec2f>staff_lines, int staff_count, int***& left_point, int***& right_point, Mat color_ord_img, bool debuging){


    drew_img_debug = color_ord_img.clone();
    cvtColor(src_bin, src_bin_debug, CV_GRAY2BGR);

    // staff = new vector<Vec2f>[staff_count];
    left_point  = new int**[staff_count];
    right_point = new int**[staff_count];

    for(int i = 0 ; i < staff_count ; i++){
        left_point[i] = new int*[STAFF_LINE_COUNT];
        right_point[i] = new int*[STAFF_LINE_COUNT];


        for(int go_width = 0 ; go_width < STAFF_LINE_COUNT ; go_width++){
            left_point[i][go_width] = new int[X_Y_COUNT];
            right_point[i][go_width] = new int[X_Y_COUNT];

            for(int k = 0 ; k < X_Y_COUNT ; k++){
                left_point[i][go_width][k] = 1000; /// 隨便很大的數
                right_point[i][go_width][k] = -1000; /// 隨便很小的數
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
        Find_Head( staff, (string)HORIZONTAL_DIR + "find_head",src_bin,left_point[i],right_point[i], debuging);
    }
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

void Debug_draw(double x0, double y0, Scalar color, int dot_size){
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

// 已經把消線分開寫了，所以把所有 Mat & 拿掉囉~~
void Find_Head(vector<Vec2f> lines, string window_name, Mat src_bin, int**& left_point, int**& right_point, bool debuging){
    if(debuging) cout << "Find_Head" << endl;


	for(int i = 0; i < lines.size(); i++ ){
        float rho = lines[i][0], theta = lines[i][1];
		double angle_value = (theta/PI)*180;
		if(debuging){
            cout << "go_staff_line = " << i << " ,";
            cout << "rho = " << rho << ", angle_value = " << angle_value << endl;
        }

		// 起點走多少
		int width  = src_bin.cols;
		int height = src_bin.rows;
        if(debuging){
            cout << "width  = " << width  << endl;
            cout << "height = " << height << endl;
        }
        // 當年 不知道怎麼推的 覺得怪怪的
		// double cos_th = cos(theta), sin_th = sin(theta);
        // double one_step_height = -1 * cos_th; //1 / sin_th * -1;
        
        // 計算在線上走一步的高度, 用hough裡面用 rho, theta 本身代表的線 是 垂直於 找到的那條線,
        // rho, theta 本身代表的線的斜率 是 tan(theta), 垂直兩條線 的斜率相乘 == -1,
        // 所以 找到的那條線的 斜率是 -cot(theta)
		double cos_th = cos(theta), sin_th = sin(theta);
        double cot_th = cos_th / sin_th; 
		double one_step_height = -1 * cot_th; //1 / sin_th * -1;

		// 因為我們是用 "擷取正中間左右10%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
		int center_roi_start_x = ( (double)width / 2 ) - ((double)width) * ((double)ROI_WIDTH / 100 /2); //ROI_WIDTH是col的百分比
        // "擷取正中間左右 ROI_WIDTH%的圖來做hough" 該圖的 左上角 與 找到的那條線 畫一條線可以與其垂直 的那個點
        double x0 = cos_th * rho, y0 = sin_th * rho;
        // 因為我們是用 "擷取正中間左右 ROI_WIDTH%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
        x0 += center_roi_start_x;
        cout << "rho            : " << rho << endl;
        cout << "x0             : " << x0 << endl;
        cout << "y0             : " << y0 << endl;
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
        Debug_draw(x0, y0, Scalar(0, 102, 255), 3);  // 橘色


        double one_step;
        for(int direction = 0 ; direction < 2 ; direction++){
            // ..._go 是會隨著找頭的進行而變動的, ..._th 是初始值不變的, 在找頭的一開始 都事先指定 初始值, 之後再隨著找頭來變動
            float  theta_go = theta;
            double cos_go = cos_th, sin_go = sin_th;
            double cot_go = cos_go / sin_go; 
            double one_step_height_go = -1 * cot_go;
            // 
            switch(direction){
                case 0:
                    one_step = 1;
                    break;
                case 1:
                    one_step = -1;
                    break;
            }
            cout << "cos_go         : " << cos_go << endl;
            cout << "sin_go         : " << sin_go << endl;
            cout << "one_step       : " << one_step << endl;
            cout << "one_step_height_go: " << one_step_height_go << endl;
            cout << endl;

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
                    Debug_draw(next_x, next_y, Scalar(0, 242, 255), 1);  // 黃色
                    continue;
                }
                
                // 順著線走 如果現在不是黑點, 
                //   有可能還在線上 但 因為畫質差 線段在二值化出現空洞
                //   有可能已經走到盡頭

                // 測試一下現在的點 是否高機率在線上

                for(int go_jump = 0; go_jump < JUMP_SPACE_LENGTH; go_jump++){
                    if( Check_shift(src_bin, next_x + go_jump * one_step, next_y + go_jump * one_step * one_step_height_go + 0, one_step, one_step_height_go) != CHECK_FAILED ){
                        // 如果真的 很可能在線上，用flag標記一下為true，等等就不用做 彎曲測試直接continue做下一個點了
                        // 也代表還沒走到盡頭 還需要 continue 繼續測試下一個點

                        
                        // 就算測試完的結果顯示 "很像線上的點" ， 但因為畫質關係二值化後也不一定會真的在線上(誤刪到線)，
                        // 所以如果目前位置如果不是黑色, 做 case1 往線方向的右邊探勘, 如果探不到再做 case2 往線方向的正上下方 探勘

                        // case1 就往線方向的右邊探勘 CHECK_LINE_LENGTH * CODA_RATE 格 希望調整到在線上這樣子
                        bool go_Right_on_line_flag = false;
                        if(src_bin.at<uchar>(next_y + go_jump*one_step*one_step_height_go, next_x + go_jump*one_step) != 0){
                            //往右探勘 CHECK_LINE_LENGTH * CODA_RATE 格
                            for(int go_R = 1 ; go_R < CHECK_LINE_LENGTH * CODA_RATE ; go_R++){
                                //探勘的格子 如果高機率是線 且 該格是黑點, 就是在線上的點囉, 就移動過去這樣子
                                if(   Check_shift(src_bin, next_x + go_R*one_step , next_y + go_R*one_step*one_step_height_go +0, one_step, one_step_height_go ) != CHECK_FAILED
                                   && src_bin.at<uchar>(next_y + go_R*one_step*one_step_height_go +0 ,next_x + go_R*one_step) == 0 ){

                                    Debug_draw(next_x, next_y, Scalar(255, 0, 0), 1);  // 藍色
                                    
                                    next_x += go_R*one_step;
                                    next_y += go_R*one_step*one_step_height_go ;
                                    go_Right_on_line_flag = true;
                                    
                                    Debug_draw(next_x, next_y, Scalar(255, 0, 0), 1);  // 藍色
                                    break;  //這個break是因為這裡測10格所以必須寫for迴圈，一測到要需要跳出去才加的所以不能少喔！
                                }
                            }
                        }
                        // 往右探勘已經順利找到點了, 就可以跳出 jump_sapce 繼續順著線往下走囉
                        if(go_Right_on_line_flag) break;

                        // 到這裡為止，如果上面有順利移動成功，那麼現在的點 就會在線上囉！ 所以下面的 "case2 往線方向走 的 正上下探勘各一格看哪個比較好" 就不會執行了，不要再搞混了！
                        // 如果沒有順利移動成功, next_x, next_y 也會在原地不動呦要注意別再搞混了~~~

                        // 反正就是不會有出現 往右跳完以後，還會在往上下跳的狀況！因為往右跳，代表已經在線上，在線上下面的 if不在線上 就會擋掉了！

                        // case2 往線方向走 的 正上下探勘各一格看哪個比較好
                        // 如果有需要做到這個，就一定代表右邊 CHECK_LINE_LENGTH 個格子 都可能不是線！！往線方向走的 上下一格 找找看
                        if(src_bin.at<uchar>(next_y, next_x + go_jump*one_step) != 0){
                            // 往線方向走 的 正上下一格 找找看
                            int cmp_up   = Check_shift(src_bin, next_x + go_jump*one_step , next_y + go_jump*one_step*one_step_height_go - 1 ,one_step,one_step_height_go);
                            int cmp_down = Check_shift(src_bin, next_x + go_jump*one_step , next_y + go_jump*one_step*one_step_height_go + 1 ,one_step,one_step_height_go);

                            // 往線方向走 的 正下一格 如果看起來比 正上一格 更像線 且 該格是黑點, 移動到那一格(往線的方向移動 和 往正下方偏移一格)
                            if(cmp_up >= cmp_down && cmp_up != CHECK_FAILED && src_bin.at<uchar>(next_y + go_jump*one_step*one_step_height_go -1, next_x + go_jump*one_step) == 0){
                                
                                Debug_draw(next_x, next_y, Scalar(0, 255, 0), 1);  // 綠色
                                
                                next_x += go_jump*one_step;
                                next_y += go_jump*one_step*one_step_height_go ;
                                next_y--;
                                
                                Debug_draw(next_x, next_y, Scalar(0, 255, 0), 1);  // 綠色

                                // cout<< "UP before" << endl;
                                // cout<< "theta_go: " << theta_go << endl;
                                // cout<< "one_step_height_go: " << one_step_height_go <<endl;
                                theta_go -= (PI / 180.0) * 0.05; // -0.05 度
                                cos_go = cos(theta_go), sin_go = sin(theta_go);
                                cot_go = cos_go / sin_go; 
                                one_step_height_go = -1 * cot_go;
                                // cout<< "UP after" << endl;
                                // cout<< "theta_go: " << theta_go << endl;
                                // cout<< "one_step_height_go: " << one_step_height_go <<endl<<endl;
                            }
                            // 往線方向走 的 正上一格 如果看起來比 正下一格 更像線 且 該格是黑點, 移動到那一格(往線的方向移動 和 往正上方偏移一格)
                            else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED && src_bin.at<uchar>(next_y +go_jump*one_step*one_step_height_go +1 ,next_x+go_jump*one_step) == 0 ){ //if( cmp_up != CHECK_FAILED && cmp_down == CHECK_FAILED)                                
                                
                                Debug_draw(next_x, next_y, Scalar(0, 0, 255), 1);  // 紅色
                                
                                next_x += go_jump*one_step;
                                next_y += go_jump*one_step*one_step_height_go ;
                                next_y++;
                                
                                Debug_draw(next_x, next_y, Scalar(0, 0, 255), 1);  // 紅色

                                // cout<< "DOWN before" << endl;
                                // cout<< "theta_go: " << theta_go << endl;
                                // cout<< "one_step_height_go: " << one_step_height_go <<endl;
                                theta_go += (PI / 180.0) * 0.05; // +0.05 度
                                cos_go = cos(theta_go), sin_go = sin(theta_go);
                                cot_go = cos_go / sin_go; 
                                one_step_height_go = -1 * cot_go;
                                // cout<< "DOWN after" << endl;
                                // cout<< "theta_go: " << theta_go << endl;
                                // cout<< "one_step_height_go: " << one_step_height_go <<endl<<endl;
                            }
                        }
                    }
                    // Check_shift false
                    // 有可能只是在 影像品質差的 間格內而已, do nothing 繼續往下測試
                    // 有可能是上下偏差太大
                    // 有可能是已經走到頭了
                    Debug_draw(next_x + go_jump * one_step, next_y + go_jump * one_step * one_step_height_go, Scalar(165, go_jump * 4, 96 + go_jump * 4), 1);  // 深深紫色
                }
                // 如果真的 很可能在線上，用flag標記一下為true，等等就不用做 彎曲測試直接continue做下一個點了
                // 也代表還沒走到盡頭 還需要 continue 繼續測試下一個點

                

                // 經過 上面的 jump space 已經排除了 線誤刪的 但 仍然沒辦法 把點移動到線上的話
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

                        // 多加這個是希望他往比較 "像線"的地方跑
                        int cmp_up   = Check_shift(src_bin, next_x, next_y-go_UD_range, one_step, one_step_height_go);
                        int cmp_down = Check_shift(src_bin, next_x, next_y+go_UD_range, one_step, one_step_height_go);
                        if( cmp_up >= cmp_down && cmp_up != CHECK_FAILED ){
                            // 畫一下現在位置
                            Debug_draw(next_x, next_y, Scalar(  0,  22, 165), 5);  // 深灰色

                            next_y -= go_UD_range;
                            
                            // 畫一下現在位置
                            Debug_draw(next_x, next_y, Scalar(  0,  22, 165), 5);  // 深灰色
                            cout << "bend detected up" << endl;

                            break;
                        }
                        else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED ){
                            // 畫一下現在位置
                            Debug_draw(next_x, next_y, Scalar(127,  0, 255), 5);  // 淺灰色
                            
                            next_y += go_UD_range;
                            
                            // 畫一下現在位置
                            Debug_draw(next_x, next_y, Scalar(127,  0, 255), 5);  // 淺灰色
                            cout << "bend detected down" << endl;

                            break;
                        }
                    }

                    /////////////////////// 如果上下測完仍然還是沒辦法~~~就print出來，表示也許不是線maybe已經找到頭囉！///////////////////
                    if(src_bin.at<uchar>(next_y,next_x) != 0 && Check_shift(src_bin, next_x,next_y,one_step,one_step_height_go) ==  CHECK_FAILED){// && error >= ok_error2)
                        break;
                    }
                }	////////////////////////////////////
		    }
            Debug_draw(next_x, next_y, Scalar(0, 0, 255), 20);  // 紅色

            if(direction == 0){
                right_point[i][0] = next_x;
                right_point[i][1] = next_y;
            }
            else if(direction == 1){
                left_point[i][0] = next_x;
                left_point[i][1] = next_y;
            }
            cout << "test " << i << " = " << "next_x=" << next_x << " , next_y=" << next_y << endl;
        }
	}

	// imshow(window_name,drew_img);
    // /******************************************************
	imwrite(window_name + "3.bmp"     , drew_img_debug);
	imwrite(window_name + "_g_img.bmp", src_bin_debug );
    cout << "停" << endl;
    // 不用消線拉，所以這個就不用了
    // src_bin = src_bin.clone();
}
