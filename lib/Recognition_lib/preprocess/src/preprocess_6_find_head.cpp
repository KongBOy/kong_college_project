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
#define HORIZONTAL_DIR "debug_preprocess/"



using namespace cv;
using namespace std;


static Mat line_g_img;
static Mat line_c_l_img;
static Mat line_c_p_img;
static string file_name;


//interface，先把東西包成對的格式，再丟到find_head找頭
void Find_Head_Interface(Mat test_bin,vector<Vec2f>staff_lines, int staff_count, int***& left_point , int***& right_point  ,Mat color_ord_img){
    // staff = new vector<Vec2f>[staff_count];
    left_point  = new int**[staff_count];
    right_point = new int**[staff_count];

    for(int i = 0 ; i < staff_count ; i++){
        left_point[i] = new int*[STAFF_LINE_COUNT];
        right_point[i] = new int*[STAFF_LINE_COUNT];


        for(int j = 0 ; j < STAFF_LINE_COUNT ; j++){
            left_point[i][j] = new int[X_Y_COUNT];
            right_point[i][j] = new int[X_Y_COUNT];

            for(int k = 0 ; k < X_Y_COUNT ; k++){
                left_point[i][j][k] = 1000; /// 隨便很大的數
                right_point[i][j][k] = -1000; /// 隨便很小的數
            }
        }
    }


    // 要注意格式喔~~~包成五個五個為一組！！！
    for(int i = 0 ; i < staff_count ; i++){
        int first_line = 5*i;
        int fifth_line = 5*(i+1)-1;

        vector<Vec2f> staff;
        for(int j = first_line ; j <= fifth_line ; j++){
            staff.push_back(staff_lines[j]);
            // cout<<"j = "<<j<<" data = "<<staff_lines[j][0]<<" "<<staff_lines[j][1]<<endl;
        }
        Find_Head( staff, color_ord_img ,(string)HORIZONTAL_DIR + "find_head",test_bin,left_point[i],right_point[i]);
    }
}







int Check_shift(int,int,int,int);

/*
void Watch_Hough_Line2 (vector<Vec2f> lines , Mat drew_img , string window_name){
	for(size_t i = 0; i < lines.size(); i++ ){
		cout<<"i = "<<i<<" , ";
		float rho = lines[i][0], theta = lines[i][1];
		double angle_value = (theta/PI)*180;
		cout<<"rho = "<<rho<<" , theta = "<<angle_value;

		//////////////// 起點走多少
		int width = drew_img.cols;
		int height = drew_img.rows;
		// cout<<" , width = "<<width;
		double one_step_height = 1 / sin(theta);
		int width_step = ( width / 2 ) - (width/100) * (15/2); //15是col的百分比

		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = 0, y0 = b*rho;//rho / (sin(theta)*sin(theta));
		cout<<" , x0  = "<<x0 <<" , y0 = "<<y0;
		cout<<endl;
		x0 += width_step; //y0 += width_step * one_step_height;
		// double x0 = a*rho, y0 = b*rho;


		pt1.x = cvRound(x0 + 10*(-b));
		pt1.y = cvRound(y0 + 10*(a));
		pt2.x = cvRound(x0 - 10*(-b));
		pt2.y = cvRound(y0 - 10*(a));
		line( drew_img , pt1, pt2, Scalar(0,i,255), 1, CV_AA);

		// cout<<"pt1.x = "<<pt1.x<<",pt1.y = "<<pt1.y<<" , pt2.x = "<<pt2.x<<"pt2.y = "<<pt2.y<<endl;

		// imshow( "Hough", color_dst );

	}
	// waitKey(0);
	cout<<endl;
	imshow(window_name,drew_img);
	imwrite(window_name + ".bmp",drew_img);
}
*/

// 用來測試某個 點 可能是否在線上
// 門檻值為 CHECK_LINE_LENGTH * CODA_RATE
// 回傳值：
//   如果可能  是  在線上，回傳可能程度
//   如果可能 不是 在線上，回傳失敗值
int Check_shift(int x0,int y0,int one_step,int one_step_height){
    int count = 0;
    for(int i = 0 ; i < CHECK_LINE_LENGTH ; i++)
    {
        if     ( line_g_img.at<uchar>(y0 + i * one_step_height * one_step    ,x0 + i * one_step) == 0 ) count++;
        else if( line_g_img.at<uchar>(y0 + i * one_step_height * one_step -1 ,x0 + i * one_step) == 0 ) count++;
        else if( line_g_img.at<uchar>(y0 + i * one_step_height * one_step +1 ,x0 + i * one_step) == 0 ) count++;

    }
    if( (float)count >= (float)CHECK_LINE_LENGTH * CODA_RATE ) return count;
    else return CHECK_FAILED;

}
/*
void Erase_line(int x0 , int y0, int one_step, int one_step_height,int go_range){
    for(int go = 0 ; go < go_range ; go++){
        line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
        // line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
        // line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;
    }
}
*/
//已經把蕭線分開寫了，所以把所有 Mat & 拿掉囉~~
void Find_Head(vector<Vec2f> lines , Mat  drew_img , string window_name ,Mat bin_src_img,int**& left_point,int**& right_point){
    line_g_img = bin_src_img.clone();
	const int line_width = 2;
	for(size_t i = 0; i < lines.size(); i++ ){
		cout<<"go_staff_line = "<<i<<" ,";
		float rho = lines[i][0], theta = lines[i][1];
		double angle_value = (theta/PI)*180;
		//cout<<"rho = "<<rho<<" ,theta = "<<angle_value;

		//////////////// 起點走多少
		int width = drew_img.cols;
		int height = drew_img.rows;
		//cout<<" ,width = "<<width;

		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		////////******/////// 正確打法 -1 * cos(theta) !!!!!! ////////******////////
		double one_step_height = -1 * a;//1 / b * -1;
		//因為我們是用 "擷取正中間左右10%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
		int width_step = ( (double)width / 2 ) - ((double)width/100) * ((double)ROI_WIDTH/2); //ROI_WIDTH是col的百分比


        for(int direction = 0 ; direction < 2 ; direction++){
            ////////******/////// 正確打法 ////////******////////
            ////////******/////////////////////////******////////
            double x0 = a*rho, y0 = b*rho;  //這是一定在線上的某個點

            //因為我們是用 "擷取正中間左右 ROI_WIDTH%的圖來做hough" 所得到的線，所以一開始的x要位移到正確的位置才行
            x0 += width_step;

            double one_step = (-1 * rho * a ) * b; //往左走了 -rho * cos 個 sin，就可以把點走回原點
            x0 += one_step;
            y0 += one_step * one_step_height;
            ////////******/////////////////////////******////////
            ////////******/////////////////////////******////////
            ///////*******//////// test ///////////******/////////

            switch(direction){
                case 0:
                    one_step = 1;
                    break;
                case 1:
                    one_step = -1;
                    break;
            }

            for(int i = 0 ; i < width/2 + width * ((float)ROI_WIDTH/(float)100); i++){
    			// one_step = 1;//(i * 1 / b ) * b; //往左走了 -rho * cos 個 sin
                // Erase_line(x0,y0,one_step,one_step_height,1);
                x0 += one_step;
                y0 += one_step * one_step_height;

                // 1/28 ~ 2/2

                ////////////////////還算ok的打法
                /////////////////////////////////////
                ////////////////// Range防呆 ///////////////////
                if(y0 >= 0 && y0 < height && x0 >= 0 && x0 < width);//do nothing
                else break;
                ////////////////// Range防呆 ///////////////////

                // 一、測試一下現在在線上的線，是否真的在線上
                bool flag = false;



                //如果經過這個for，flag仍然沒有被更新成true的話，
                //一、就代表你右邊 JUMP_SPACE_LENGTH 格都可能不是在線上！！
                //二、X0 Y0 不會被更新喔喔喔喔喔喔！
                for(int i = 0 ; i < JUMP_SPACE_LENGTH ; i++){
                    if( Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0 ,one_step,one_step_height) != CHECK_FAILED ){
                        //如果真的在線上，用flag標記一下為true，等等就不用做 彎曲測試直接continue做下一個點了
                        flag = true;

                        //就算測試完的結果顯示 "很像線上的點" ， 但因為畫質關係二值化後也不一定會真的在線上(誤刪到線)，所以現在希望調整到在線上這樣子
                        //希望把點移到黑色的地方，就是二值化怕不小心誤刪線所以如果有空格先偵測是不是誤刪
                        //(就那個空格來測測看像不像線)，同時也希望跳躍空洞
                        //往右10格
                        if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0){
                            for(int i = 1 ; i < CHECK_LINE_LENGTH * CODA_RATE ; i++){
                                //先 丟下一格要走的點 來預測他是不是線~~~ 如果是才移動喔！！
                                if(   Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0,one_step,one_step_height ) != CHECK_FAILED
                                   && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +0 ,x0+i*one_step) == 0 ){

                                    // Erase_line(x0,y0,one_step,one_step_height,i);
                                    for(int go = 0 ; go < i ; go++){
                                        /*
                                        line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                        */
                                        pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                                        pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                                        pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                                        pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                                        line( drew_img , pt1, pt2, Scalar(255,0,0), 2, CV_AA);
                                    }

                                    x0 += i*one_step;
                                    y0 += i*one_step*one_step_height ;
                                    break; //這個break是因為這裡測10格所以必須寫for迴圈，一測到要需要跳出去才加的所以不能少喔！
                                }
                            }
                        }
                        //到這裡為止，如果上面有順利移動成功，那麼現在的點 就會在線上囉！ 所以下面的 "看上下哪一格比較好" 就不會執行了，不要再搞混了！
                        //如果沒有順利移動成功，x0,y0也會在原地不動呦要注意別再搞混了~~~

                        //反正就是不會有出現 往右跳完以後，還會在往上下跳的狀況！因為往右跳，代表已經在線上，在線上下面的if就會擋掉了！

                        //往上下各一格看哪個比較好
                        //如果有需要做到這個，就一定代表右邊 CHECK_LINE_LENGTH 個格子 都可能不是線！！
                        if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0){
                            //多加這個是希望他往比較 "像線"的地方跑
                            int cmp_up   = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height -1 ,one_step,one_step_height);
                            int cmp_down = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height +1 ,one_step,one_step_height);

                            //會多加 == 0 ，是希望他跑得更嚴謹、精確不要亂亂跑
                            if(cmp_up >= cmp_down && cmp_up != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height -1 ,x0+i*one_step) == 0){
                            //if( cmp_up == CHECK_FAILED && cmp_down != CHECK_FAILED){
                            
                                // Erase_line(x0,y0,one_step,one_step_height,i);
                                for(int go = 0 ; go < i ; go++){
                                    /*
                                    line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    */

                                    pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                                    pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                                    pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                                    pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                                    line( drew_img , pt1, pt2, Scalar(0,255,0), 2, CV_AA);
                                }
                                x0 += i*one_step;
                                y0 += i*one_step*one_step_height ;
                                y0--;
                            }
                            else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +1 ,x0+i*one_step) == 0 ){ //if( cmp_up != CHECK_FAILED && cmp_down == CHECK_FAILED)
                                // Erase_line(x0,y0,one_step,one_step_height,i);
                                for(int go = 0 ; go < i ; go++){
                                    /*
                                    line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                    */

                                    pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                                    pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                                    pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                                    pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                                    line( drew_img , pt1, pt2, Scalar(0,0,255), 2, CV_AA);

                                }
                                x0 += i*one_step;
                                y0 += i*one_step*one_step_height ;
                                y0++;
                            }
                        }
                        // Erase_line(x0,y0,one_step,one_step_height,1);
                        /*
                        line_g_img.at<uchar>(y0  ,x0) = 255;
                        line_g_img.at<uchar>(y0-1,x0) = 255;
                        line_g_img.at<uchar>(y0+1,x0) = 255;
                        line_g_img.at<uchar>(y0-2,x0) = 255;
                        line_g_img.at<uchar>(y0+2,x0) = 255;
                        line_g_img.at<uchar>(y0-3,x0) = 255;
                        line_g_img.at<uchar>(y0+3,x0) = 255;
                        */
                        /*
                        pt1.x = cvRound(x0); //+ 10*(-b));
                        pt1.y = cvRound(y0); //+ 10*(a));
                        pt2.x = cvRound(x0); //- 10*(-b));
                        pt2.y = cvRound(y0); //- 10*(a));
                        line( drew_img , pt1, pt2, Scalar(50,100,150), 2, CV_AA);
                        */
                    }
                    else ; //do nothing 繼續往下測試
            }
            if(flag == true) continue;


            ////彎曲測試：
            /////////////已經排除了誤刪的情況，那麼現在如果還不是在黑點上就代表很可能是線彎曲了！往上、下偵測ERROR_CODA格
			if(line_g_img.at<uchar>(y0,x0) != 0){
			    //如果線字的點不是黑色的，就代表還是有些偏差，希望可以位移到比較正確的位置
			    //(不一定要移動到一定黑色的點才行，只要移到 "被認可是線的地方即可")

                ////第一種寫法：移動相同距離，來比較往up還往down移動比較好
                int cmp_up   = -1;
                int cmp_down = -1;
                for(int i = 0 ; i <= ERROR_CODA ; i++){
                    ////////////////// Range防呆 ///////////////////
                    if(y0 >= 0+i && y0 < height-i && x0 >= 0+i && x0 < width-i);//do nothing
                    //if(y0 >= 0+5+ok_error && y0 < height-5-ok_error && x0 >= 0+5+ok_error && x0 < width-5-ok_error);//do nothing
                    else break;
                    ////////////////// Range防呆 ///////////////////
                    //多加這個是希望他往比較 "像線"的地方跑
                    int cmp_up   = Check_shift(x0,y0-i,one_step,one_step_height);
                    int cmp_down = Check_shift(x0,y0+i,one_step,one_step_height);
                    if( cmp_up >= cmp_down && cmp_up != CHECK_FAILED ){
                        y0 -= i;

                        for(int go = 0 ; go <= i ; go++){
                            pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                            pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                            pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                            pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                            line( drew_img , pt1, pt2, Scalar(50,255,50), 2, CV_AA);
                            cout<<"bend detected"<<endl;
                        }
                        break;
                    }
                    else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED ){
                        y0 += i;

                        for(int go = 0 ; go <= i ; go++){
                            pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                            pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                            pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                            pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                            line( drew_img , pt1, pt2, Scalar(50,50,255), 2, CV_AA);
                            cout<<"bend detected"<<endl;
                        }
                        break;
                    }
                }

                /////////////////////// 如果上下測完仍然還是沒辦法~~~就print出來，表示也許不是線maybe已經找到頭囉！///////////////////
				if(line_g_img.at<uchar>(y0,x0) != 0 && Check_shift(x0,y0,one_step,one_step_height) ==  CHECK_FAILED){// && error >= ok_error2)
					break;
				}
			}	////////////////////////////////////
		}
        pt1.x = cvRound(x0); //+ 10*(-b));
        pt1.y = cvRound(y0); //+ 10*(a));
        pt2.x = cvRound(x0); //- 10*(-b));
        pt2.y = cvRound(y0); //- 10*(a));
		line( drew_img , pt1, pt2, Scalar(0,0,255), 20, CV_AA);

		if(direction == 0){
            right_point[i][0] = x0;
            right_point[i][1] = y0;
        }
        else if(direction == 1){
            left_point[i][0] = x0;
            left_point[i][1] = y0;
        }

		cout<<"test "<<i<<" = "<<"x0="<<x0 <<" , y0="<<y0<<endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////下面完全是複製貼上，只有兩個地方不一樣！/**/的地方喔/////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }


    /*
    //只有這裡不一樣，上面宣告過了//
    x0 = a*rho, y0 = b*rho;
    x0 += width_step;

    one_step = (-1 * rho * a ) * b; //往左走了 -rho * cos 個 sin
    x0 += one_step;
    y0 += one_step * one_step_height;



    // 只有這裡不一樣，range不一樣而已 //
    for(int i = 0 ; i < width/2 - width * ((float)ROI_WIDTH/(float)100)/2; i++){
        // 只有這裡不一樣，變成往左走//
        one_step = -1;  //(i * 1 / b ) * b;  //往左走了 -rho * cos 個 sin
        x0 += one_step;
        y0 += one_step * one_step_height;

        // 1/28 ~ 2/2

        ////////////////////還算ok的打法
        /////////////////////////////////////
        ////////////////// Range防呆 ///////////////////
        if(y0 >= 0 && y0 < height && x0 >= 0 && x0 < width);//do nothing
        else break;
        ////////////////// Range防呆 ///////////////////

        bool flag = false;
        for(int i = 0 ; i < JUMP_SPACE_LENGTH ; i++){
            ////////////////// Range防呆 ///////////////////
            if(y0 >= 0 && y0 < height && x0 >= 0 && x0 < width);//do nothing
            else break;
            ////////////////// Range防呆 ///////////////////

            if( Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0 ,one_step,one_step_height) != CHECK_FAILED ){
                flag = true;

                if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0){
                    for(int i = 1 ; i < CHECK_LINE_LENGTH * CODA_RATE ; i++){
                        if(   Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0 ,one_step,one_step_height) != CHECK_FAILED
                            && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +0 ,x0+i*one_step) == 0 ){
                            for(int go = 0 ; go < i ; go++){
                                line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                                line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;


                                pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                                pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                                pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                                pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                                line( drew_img , pt1, pt2, Scalar(255,50,50), 2, CV_AA);

                            }
                            x0 += i*one_step;
                            y0 += i*one_step*one_step_height ;
                            break;
                        }
                    }
                }

                if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0){
                    //多加這個是希望他往比較 "像線"的地方跑
                    int cmp_up   = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height -1 ,one_step,one_step_height);
                    int cmp_down = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height +1 ,one_step,one_step_height);

                    //會多加 == 0 ，是希望他跑得更嚴謹、精確不要亂亂跑
                    if(cmp_up >= cmp_down && cmp_up != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height -1 ,x0+i*one_step) == 0)
                    //if( cmp_up == CHECK_FAILED && cmp_down != CHECK_FAILED){
                        for(int go = 0 ; go < i ; go++){
                            line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;


                            pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                            pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                            pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                            pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                            line( drew_img , pt1, pt2, Scalar(50,255,50), 2, CV_AA);

                        }
                        x0 += i*one_step;
                        y0 += i*one_step*one_step_height ;
                        y0--;
                    }
                    else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +1 ,x0+i*one_step) == 0 )//if( cmp_up != CHECK_FAILED && cmp_down == CHECK_FAILED){
                        for(int go = 0 ; go < i ; go++){
                            line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
                            line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;


                            pt1.x = cvRound(x0+go*one_step); //+ 10*(-b));
                            pt1.y = cvRound(y0+go*one_step*one_step_height); //+ 10*(a));
                            pt2.x = cvRound(x0+go*one_step); //- 10*(-b));
                            pt2.y = cvRound(y0+go*one_step*one_step_height); //- 10*(a));
                            line( drew_img , pt1, pt2, Scalar(50,50,255), 2, CV_AA);

                            }
                        x0 += i*one_step;
                        y0 += i*one_step*one_step_height ;
                        y0++;
                    }
                }

                line_g_img.at<uchar>(y0  ,x0) = 255;
                line_g_img.at<uchar>(y0-1,x0) = 255;
                line_g_img.at<uchar>(y0+1,x0) = 255;
                line_g_img.at<uchar>(y0-2,x0) = 255;
                line_g_img.at<uchar>(y0+2,x0) = 255;
                line_g_img.at<uchar>(y0-3,x0) = 255;
                line_g_img.at<uchar>(y0+3,x0) = 255;

                pt1.x = cvRound(x0); //+ 10*(-b));
                pt1.y = cvRound(y0); //+ 10*(a));
                pt2.x = cvRound(x0); //- 10*(-b));
                pt2.y = cvRound(y0); //- 10*(a));
                line( drew_img , pt1, pt2, Scalar(150,100,50), 2, CV_AA);

            }
            else ; //do nothing 繼續往下測試
        }
        if(flag == true) continue;
			if(line_g_img.at<uchar>(y0,x0) != 0){
			    for(int i = 0 ; i <= ERROR_CODA ; i++){
                    ////////////////// Range防呆 ///////////////////
                    if(y0 >= 0+i && y0 < height-i && x0 >= 0+i && x0 < width-i);//do nothing
                    //if(y0 >= 0+5+ok_error && y0 < height-5-ok_error && x0 >= 0+5+ok_error && x0 < width-5-ok_error);//do nothing
                    else break;
                    ////////////////// Range防呆 ///////////////////
                    //多加這個是希望他往比較 "像線"的地方跑
                    int cmp_up   = Check_shift(x0,y0-i,one_step,one_step_height);
                    int cmp_down = Check_shift(x0,y0+i,one_step,one_step_height);
                    if( cmp_up >= cmp_down && cmp_up != CHECK_FAILED ){
                        y0 -= i;
                        break;
                    }
                    else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED ){
                        y0 += i;
                        break;
                    }
                }

                /////////////////////// 如果上下測完仍然還是沒辦法~~~就print出來，表示也許不是線maybe已經找到頭囉！///////////////////
				if(line_g_img.at<uchar>(y0,x0) != 0 && Check_shift(x0,y0,one_step,one_step_height) ==  CHECK_FAILED)// && error >= ok_error2){
					break;
				}
			}
		}

        pt1.x = cvRound(x0); //+ 10*(-b));
        pt1.y = cvRound(y0); //+ 10*(a));
        pt2.x = cvRound(x0); //- 10*(-b));
        pt2.y = cvRound(y0); //- 10*(a));
		line( drew_img , pt1, pt2, Scalar(0,0,255), 2, CV_AA);
		cout<<"test "<<i<<" = "<<"x0="<<x0 <<" , y0="<<y0<<endl;

		left_point[i][0] = x0;
		left_point[i][1] = y0;
        */
	}

	// imshow(window_name,drew_img);
    // /******************************************************
	imwrite(window_name + "3.bmp",drew_img);
	imwrite(window_name + "_g_img.bmp",line_g_img);
    // 不用消線拉，所以這個就不用了
    // bin_src_img = line_g_img.clone();
}
