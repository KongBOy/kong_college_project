/*
這個程式的功能是：
利用 preprocess_5 找出的 五線譜 資訊
把 輸入譜的圖片 的五線譜消除
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>

#include "preprocess_6_reduce_line.h"

using namespace std;
using namespace cv;


#define PI 3.14159
#define ROI_WIDTH 10
#define CHECK_SUCESS 0
#define CHECK_FAILED -1

#define CHECK_LINE_LENGTH 10
#define CODA_RATE 0.25
#define JUMP_SPACE_LENGTH 30
#define ERROR_CODA 6


#define HORIZONTAL_DIR "horizontal_test/"

static Mat line_g_img;
static Mat line_c_l_img;
static Mat line_c_p_img;
static string file_name;


static int Check_shift(int x0,int y0,int one_step,int one_step_height)
{
    int count = 0;
    for(int i = 0 ; i < CHECK_LINE_LENGTH ; i++)
    {
             if( line_g_img.at<uchar>(y0 + i * one_step_height * one_step    ,x0 + i * one_step) == 0 ) count++;
        else if( line_g_img.at<uchar>(y0 + i * one_step_height * one_step -1 ,x0 + i * one_step) == 0 ) count++;
        else if( line_g_img.at<uchar>(y0 + i * one_step_height * one_step +1 ,x0 + i * one_step) == 0 ) count++;

    }
    if( (float)count >= (float)CHECK_LINE_LENGTH * CODA_RATE ) return count;
    else return CHECK_FAILED;

}

void Erase_line(Mat& bin_src_img,int x0 , int y0, int one_step, int one_step_height,int go_range)
{
/*
    for(int go = 0 ; go < go_range ; go++)
    {
        if( (line_g_img.at<uchar>(y0-5,x0-5) == 0) || (line_g_img.at<uchar>(y0+5,x0+5) == 0)  ) ;//do nothing
        else
        {
        line_g_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
        line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;

        }
    }
*/


    for(int go = 0 ; go < go_range ; go++)
    {
/*
        if( (line_g_img.at<uchar>(y0-5,x0) == 255) || (line_g_img.at<uchar>(y0+5,x0) == 255) ||
            (line_g_img.at<uchar>(y0-4,x0) == 255) || (line_g_img.at<uchar>(y0+4,x0) == 255) ||
            (line_g_img.at<uchar>(y0-3,x0) == 255) || (line_g_img.at<uchar>(y0+3,x0) == 255) ||
            (line_g_img.at<uchar>(y0-2,x0) == 255) || (line_g_img.at<uchar>(y0+2,x0) == 255) )
*/
/*
        if( (line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) == 255) ||
            (line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) == 255) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing


        if( (line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
        }else ;//do nothing
        if( (line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
        }else ;//do nothing


        if( (line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing
        if( (line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing


        if( (line_g_img.at<uchar>(y0-4+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing
        if( (line_g_img.at<uchar>(y0+4+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing


        if( (line_g_img.at<uchar>(y0-5+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0-4+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-2+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-3+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0-4+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing

        if( (line_g_img.at<uchar>(y0+5+go*one_step*one_step_height , x0+go*one_step) == 255) && (line_g_img.at<uchar>(y0+4+go*one_step*one_step_height , x0+go*one_step) == 0) )
        {
            bin_src_img.at<uchar>(y0  +go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+1+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+2+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+3+go*one_step*one_step_height , x0+go*one_step) = 255;
            bin_src_img.at<uchar>(y0+4+go*one_step*one_step_height , x0+go*one_step) = 255;

        }else ;//do nothing
*/

//方法二：定這個點的上面和下面，然後測試之間的距離，如果小於 線寬 就代表是線 就消線~~
        int up = y0 + go*one_step*one_step_height;
//定位up~~~如果沒有在線上的話，往上下找五格~~~
        if( (line_g_img.at<uchar>(up ,x0+go*one_step) != 0) )
        {
            for(int i = 0 ; i < 5 ; i++)
            {
                if(line_g_img.at<uchar>(up+i  ,x0+go*one_step) == 0)
                {
                    up+=i;
                    break;
                }
                else if((line_g_img.at<uchar>(up-i  ,x0+go*one_step) == 0))
                {
                    up-=i;
                    break;
                }
            }
        }

//如果up沒有訂位成功，就不做拉~~~因為可能是在 空白處~~~
        if(line_g_img.at<uchar>(up  ,x0+go*one_step) != 0)
        {
//do nothing~~ 以下是debug訊息
//            cout<<"something wrong~~"<<endl;
//            imshow("where",bin_src_img( Rect(x0-80,up-80,240,240) ));
//            waitKey(0);
        }
        else
        {
//            imshow("where",bin_src_img( Rect(x0-30,up-30,60,60) ));
//            waitKey(0);
            int down = up;

            while(line_g_img.at<uchar>(up  ,x0+go*one_step) == 0) up--;
            while(line_g_img.at<uchar>(down,x0+go*one_step) == 0) down++;
            down--;

            int distance = down - up;
            if(distance <= 7)
            {
                for(int i = 0 ; i <= distance ; i++ ) bin_src_img.at<uchar>(up + i,x0+go*one_step) = 255;
            }
/*debug
            if(distance >=10 && distance <=20)
            {
                cout<<"y0 = "<<y0<<" x0 = "<<x0+go*one_step;
                cout<<" distance = "<<distance<<endl;
                imshow("where",bin_src_img( Rect(x0+go*one_step-25,up-25,50,50) ));
                waitKey(0);
            }
*/

        }
    }

//    cout<<" go_range = "<<go_range;
//    cout<<" end_clean"<<endl;

}


void Reduce_lines(vector<Vec2f> lines , Mat drew_img , string window_name ,Mat& bin_src_img)
{
    line_g_img = bin_src_img.clone();
	const int line_width = 2;
	for(size_t i = 0; i < lines.size(); i++ )
	{



/**///		cout<<"i = "<<i<<" ,";
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



        for(int direction = 0 ; direction < 2 ; direction++)
        {
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

            switch(direction)
            {
                case 0:
                    one_step = 1;
                    break;
                case 1:
                    one_step = -1;
                    break;
            }

            for(int i = 0 ; i < width/2 + width * ((float)ROI_WIDTH/(float)100); i++)
            {
    //			one_step = 1;//(i * 1 / b ) * b; //往左走了 -rho * cos 個 sin
                Erase_line(bin_src_img,x0,y0,one_step,one_step_height,1);
                x0 += one_step;
                y0 += one_step * one_step_height;

                //1/28 ~ 2/2

    ////////////////////還算ok的打法
    /////////////////////////////////////
    ////////////////// Range防呆 ///////////////////
    if(y0 >= 0 && y0 < height && x0 >= 0 && x0 < width);//do nothing
    else break;
    ////////////////// Range防呆 ///////////////////

    //一、測試一下現在在線上的線，是否真的在線上
                bool flag = false;



    //如果經過這個for，flag仍然沒有被更新成true的話，
    //一、就代表你右邊 JUMP_SPACE_LENGTH 格都可能不是在線上！！
    //二、X0 Y0 不會被更新喔喔喔喔喔喔！
                for(int i = 0 ; i < JUMP_SPACE_LENGTH ; i++)
                {
                        if( Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0 ,one_step,one_step_height) != CHECK_FAILED )
                        {
                            //如果真的在線上，用flag標記一下為true，等等就不用做 彎曲測試直接continue做下一個點了
                            flag = true;

    //就算測試完的結果顯示 "很像線上的點" ， 但因為畫質關係二值化後也不一定會真的在線上(誤刪到線)，所以現在希望調整到在線上這樣子
    //希望把點移到黑色的地方，就是二值化怕不小心誤刪線所以如果有空格先偵測是不是誤刪
    //(就那個空格來測測看像不像線)，同時也希望跳躍空洞
                            //往右10格
                            if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0)
                            {
                                for(int i = 1 ; i < CHECK_LINE_LENGTH * CODA_RATE ; i++)
                                {
                                    //先 丟下一格要走的點 來預測他是不是線~~~ 如果是才移動喔！！
                                    if(   Check_shift(x0+i*one_step , y0 +i*one_step*one_step_height +0,one_step,one_step_height ) != CHECK_FAILED
                                       && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +0 ,x0+i*one_step) == 0 )
                                    {

                                        Erase_line(bin_src_img,x0,y0,one_step,one_step_height,i);
                                        for(int go = 0 ; go < i ; go++)
                                        {

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
                            if(line_g_img.at<uchar>(y0,x0+i*one_step) != 0)
                            {
                                //多加這個是希望他往比較 "像線"的地方跑
                                int cmp_up   = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height -1 ,one_step,one_step_height);
                                int cmp_down = Check_shift(x0+i*one_step , y0 + i*one_step*one_step_height +1 ,one_step,one_step_height);

                                //會多加 == 0 ，是希望他跑得更嚴謹、精確不要亂亂跑
                                if(cmp_up >= cmp_down && cmp_up != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height -1 ,x0+i*one_step) == 0)
                                //if( cmp_up == CHECK_FAILED && cmp_down != CHECK_FAILED)
                                {
                                    Erase_line(bin_src_img,x0,y0,one_step,one_step_height,i);
                                    for(int go = 0 ; go < i ; go++)
                                        {

                                        }
                                     x0 += i*one_step;
                                        y0 += i*one_step*one_step_height ;
                                        y0--;
                                }
                                else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED && line_g_img.at<uchar>(y0 +i*one_step*one_step_height +1 ,x0+i*one_step) == 0 )//if( cmp_up != CHECK_FAILED && cmp_down == CHECK_FAILED)
                                {
                                    Erase_line(bin_src_img,x0,y0,one_step,one_step_height,i);
                                    for(int go = 0 ; go < i ; go++)
                                        {

                                        }
                                    x0 += i*one_step;
                                        y0 += i*one_step*one_step_height ;
                                        y0++;
                                }
                            }
                            //Erase_line(bin_src_img,x0,y0,one_step,one_step_height,1);

                        }
                        else ; //do nothing 繼續往下測試
                }
                if(flag == true) continue;

//cout<<"bend~~~"<<endl;
    ////彎曲測試：
    /////////////已經排除了誤刪的情況，那麼現在如果還不是在黑點上就代表很可能是線彎曲了！往上、下偵測ERROR_CODA格
                if(line_g_img.at<uchar>(y0,x0) != 0)
                {
                    //如果線字的點不是黑色的，就代表還是有些偏差，希望可以位移到比較正確的位置
                    //(不一定要移動到一定黑色的點才行，只要移到 "被認可是線的地方即可")

    ////第一種寫法：移動相同距離，來比較往up還往down移動比較好
                    int cmp_up   = -1;
                    int cmp_down = -1;
                    for(int i = 0 ; i <= ERROR_CODA ; i++)
                    {
    ////////////////// Range防呆 ///////////////////
    if(y0 >= 0+i && y0 < height-i && x0 >= 0+i && x0 < width-i);//do nothing
    //if(y0 >= 0+5+ok_error && y0 < height-5-ok_error && x0 >= 0+5+ok_error && x0 < width-5-ok_error);//do nothing
    else break;
    ////////////////// Range防呆 ///////////////////
                        //多加這個是希望他往比較 "像線"的地方跑
                        int cmp_up   = Check_shift(x0,y0-i,one_step,one_step_height);
                        int cmp_down = Check_shift(x0,y0+i,one_step,one_step_height);
                        if( cmp_up >= cmp_down && cmp_up != CHECK_FAILED )
                        {
                            y0 -= i;


                            cout<<"bend detected"<<endl;


                            break;
                        }
                        else if(cmp_down > cmp_up && cmp_down != CHECK_FAILED )
                        {
                            y0 += i;


    cout<<"bend detected"<<endl;


                            break;
                        }
                    }

    /////////////////////// 如果上下測完仍然還是沒辦法~~~就print出來，表示也許不是線maybe已經找到頭囉！///////////////////
                    if(line_g_img.at<uchar>(y0,x0) != 0 && Check_shift(x0,y0,one_step,one_step_height) ==  CHECK_FAILED)// && error >= ok_error2)
                    {
                        break;
                    }
                }	////////////////////////////////////

            }
            pt1.x = cvRound(x0); //+ 10*(-b));
            pt1.y = cvRound(y0); //+ 10*(a));
            pt2.x = cvRound(x0); //- 10*(-b));
            pt2.y = cvRound(y0); //- 10*(a));
            line( drew_img , pt1, pt2, Scalar(0,0,255), 2, CV_AA);

    //		cout<<"test "<<i<<" = "<<"x0="<<x0 <<" , y0="<<y0<<endl;

        }
	}
	//imshow(window_name,drew_img);
///******************************************************
///	imwrite(window_name + "3.bmp",drew_img);
///	imwrite(window_name + "_g_img.bmp",bin_src_img);
	//bin_src_img = line_g_img.clone();
}
