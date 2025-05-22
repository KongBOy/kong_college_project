/*
這個程式的功能是：
找出符號的音高
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_4_find_picth.h"

using namespace cv;
using namespace std;

void recognition_5_find_pitch(Mat cut_ord_img,
                              Mat template_img,
                              int note_count , int note[][1000],
                              int extend)
{
    /// 音高
    Mat debug_img = cut_ord_img.clone();
    cvtColor(cut_ord_img,debug_img,CV_GRAY2BGR);

    int line_shift = 14;

    for(int i = 0 ; i < 5 ; i++)
    {
        ///line( debug_img , Point(0,43+extend + i*11 - line_shift) , Point(1156,43+extend + i*11 - line_shift) , Scalar(125,255,0) , 2 );
    }

/// //////////////////////////////      定位五線譜      //////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////////////
    int staff_shift = 0;
    int staff_base_y = extend + staff_shift;


    for(int go_note = 0 ; go_note < note_count ; go_note++)
    {
        /// staff_base_y += staff_shift; /// 下次丟進function是丟移動過的base_y的概念~~~ 移動到下面，方便我之後的程式碼看

        ///************* 以下就想像成丟進function拉 ~~ **************
        bool shift_ok = false;
        int go_staff_x = note[0][go_note]+8;
        int go_staff_y = staff_base_y;
        int go_staff_shift = 0;



        /// y 修正
        while(shift_ok == false)
        {

            if( (cut_ord_img.at<uchar>(go_staff_y -1,go_staff_x) == 255) &&(cut_ord_img.at<uchar>(go_staff_y,go_staff_x) == 0) ) shift_ok = true;///do nothing
            else
            {
                /// 因為怕只看第一條線會有符桿影響的問題，所以多跑幾條，只要找到 shift 就break就好囉！
                /// 我自己的想像是：一定會有可以測到的，要不然我們人類怎麼分辨呢 ~~
                line(debug_img,Point(go_staff_x,staff_base_y),Point(go_staff_x,staff_base_y),Scalar(200,200,30),10);
                for(int go_staff = 0 ; go_staff < 5 ; go_staff++)
                {
                    ///cout註解 看現在在哪條五線五線
                    ///cout<<"go_staff = "<<go_staff<<endl;
                    go_staff_y = staff_base_y + 11*go_staff;

                    /// 線的特性： 本格是黑色(x,y) 而 上面一格格是白色(x,y-1)
                    /// 往上下跑 測試七格
                    /// 不能測太多格喔！！因為怕會定位定到別條五線譜就gg了！(五線譜間距11~~所以測7個就好~~)
                    for(int go_shift = 0 ; go_shift < 8 ; go_shift++ )
                    {
                        ///**** 往上 ****///
                        if( (cut_ord_img.at<uchar>(go_staff_y -1 -go_shift,go_staff_x) == 255) &&(cut_ord_img.at<uchar>(go_staff_y -go_shift,go_staff_x) == 0) )
                        {
                            ///cout註解 看一下現在是在哪個case
                            ///cout<<"up   case, go_shift = "<<go_shift<<endl;
                            /// 找到go_shift了，看要不要保留go_shift
                            /// 用線寬 來判斷 看要不要保留go_shift
/// *************************************************************************************
/// 找線寬~~~可以function化 不過短短的就直接寫近來好了~~
                            int check_width = 0;
                            int check_x = go_staff_x;
                            int check_y = go_staff_y -go_shift;

                            /// 找 線寬
                            while(cut_ord_img.at<uchar>(check_y,check_x) == 0)
                            {
                                check_width++;
                                check_y++;
                            }
                            line(debug_img,Point(check_x,check_y),Point(check_x,check_y-check_width),Scalar(0,255,0),5);
                            ///cout註解 看一下check_width對不對
                            ///cout<<"check_width = "<<check_width<<endl;
/// ~~~~~~~~~~~~~~~~ debug 用來看找線寬的過程 ~~~~~~~~~~~~~~~~~~~
///                            imshow("debug",debug_img);
///                            waitKey(0);
/// *************************************************************************************
                            /// 由 線寬 來判斷現在是不是 五線譜的線 而不是 符桿
                            if(check_width < 5)
                            {
                                go_staff_shift = go_shift*-1;
                                shift_ok = true;
                                break;
                            }
                            else shift_ok = false; /// do nothing

                        }
                        ///**** 往下 ****/// 幾乎複製貼上~~上面的case，再小改即可~~
                        else if( (cut_ord_img.at<uchar>(go_staff_y -1 +go_shift,go_staff_x) == 255) &&(cut_ord_img.at<uchar>(go_staff_y +go_shift,go_staff_x) == 0) )
                        {
                            ///cout註解 看一下現在是在哪個case
                            ///cout<<"down case, go_shift = "<<go_shift<<endl;
/// *************************************************************************************
/// 找線寬~~~可以function化 不過短短的就直接寫近來好了~~
                            int check_width = 0;
                            int check_x = go_staff_x;
                            int check_y = go_staff_y +go_shift;

                            /// 找 線寬
                            while(cut_ord_img.at<uchar>(check_y,check_x) == 0)
                            {
                                check_width++;
                                check_y++;
                            }
                            line(debug_img,Point(check_x,check_y),Point(check_x,check_y-check_width),Scalar(0,255,0),5);
                            ///cout註解 看一下check_width對不對
                            ///cout<<"check_width = "<<check_width<<endl;
 /// ~~~~~~~~~~~~~~~ debug 用來看找線寬的過程 ~~~~~~~~~~~~~~
 ///                           imshow("debug",debug_img);
 ///                           waitKey(0);

/// *************************************************************************************
                            /// 由 線寬 來判斷現在是不是 五線譜的線 而不是 符桿
                            if(check_width < 5)
                            {
                                go_staff_shift = go_shift;
                                shift_ok = true;
                                break;
                            }
                            else shift_ok = false; /// do nothing

                        }
                        if(shift_ok) break;

                    }
                    if(shift_ok) break;
                }
            }
            /// 正常來說做一次即可~~但是！
            /// 有可能整條都被符桿蓋住了所以找不到~~~所以如果找不到就往右走一點點再找一次看看囉！
            if(shift_ok == false) go_staff_x +=3 ;
        }
        /// /////////////////////////////////


        staff_shift = go_staff_shift; /// return go_shift 的概念
        ///cout註解 看一下staff_shift對不對
        ///cout<<"staff_shift = "<<staff_shift<<endl;

        staff_base_y += staff_shift; /// 下次丟進function是丟移動過的base_y的概念~~~移動到這裡喔~~

        for(int i = 0 ; i < 5 ; i++)
        {
            line(debug_img,Point(go_staff_x, staff_base_y + i*11),Point(go_staff_x+ template_img.cols, staff_base_y + i*11),Scalar(0,0,255),2);
        }

///        cout<<"staff_base_y = "<<staff_base_y<<" , staff_shift = "<<staff_shift<<endl;
/// ~~~~~~~ debug 用來看校正線的過程 ~~~~~~~~~~~~~~~~~
///        imshow("debug",debug_img);


/// //////////////////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////////////

        int pitch = note[1][go_note];
        line(debug_img,Point(note[0][go_note],pitch),Point(note[0][go_note],pitch),Scalar(255,0,0),3);

        int base_pitch = staff_base_y -3;
/// ~~~~~~ debug ~~~~~~~
/*
        for(int staff_line_index = -3 ; staff_line_index < 8 ; staff_line_index++)
        {
            for(int i = 0 ; i < 5 ; i++)
            {
                line(debug_img,Point(note[0][go_note],base_pitch+11*staff_line_index+i),
                               Point(note[0][go_note]+template_img.cols,base_pitch+11*staff_line_index+i),Scalar(0,100,100),1);
                line(debug_img,Point(note[0][go_note],base_pitch+11*staff_line_index+6+i),
                               Point(note[0][go_note]+template_img.cols,base_pitch+11*staff_line_index+6+i),Scalar(100,0,100),1);
            }
            line(debug_img,Point(note[0][go_note],base_pitch+11*staff_line_index+5),
                           Point(note[0][go_note]+template_img.cols,base_pitch+11*staff_line_index+5),Scalar(0,100,100),1);

        }
        line(debug_img,Point(note[0][go_note]+10,pitch),Point(note[0][go_note]+10,pitch),Scalar(255,255,255),1);
*/

        int count = pitch - base_pitch;
        ///cout註解 看一下pitch的運算對不對
        ///cout<<"pitch = "<<pitch<<" , count = "<<count<< " , count / 11 +1= "<<count/11 +1<<" , count%11 = "<<count%11;

        note[4][go_note] = (count/11 )*2;
        if(count < 0) note[4][go_note] -=2;


        int on_line = count % 11; /// 如果count %11 = 6,7,8,9,10 就 pitch+1
        if(on_line < 0)
        {
            on_line += 11;/// 太高音跑到負數，就要調整到正數的位置就好囉~~
            on_line %= 11;/// 別忘了這個喔！因為當on_line == -11時~~ %完為0，再加11後就==11了！但要map到0，所以就要在%11拉！
        }
        if(on_line/6) note[4][go_note]++;


        ///cout註解 看一下音高對不對
        ///cout<<" , 音高 = "<<note[4][go_note]<<endl;

/// ~~~~~~ debug 用來看頭的左上角的點大概落在五線譜的哪裡 ~~~~~~~~~~~~~~~
///        imshow("debug",debug_img);
///        waitKey(0);
        ///cout註解
        ///cout<<endl;
    }


    for(int i = 0 ; i < note_count ; i++)
    {
        ///***********防呆呆呆呆*********** 但小心別設錯呀！！！！
        if( (note[4][i] <= 16) && (note[4][i] >= -11) )
        {
            ///cout<<"i'm here~~"<<endl;
            switch(note[4][i])
            {
                case 16:note[4][i] = 48;///3C
                    break;
                case 15:note[4][i] = 50;
                    break;
                case 14:note[4][i] = 52;
                    break;
                case 13:note[4][i] = 53;
                    break;
                case 12:note[4][i] = 55;
                    break;
                case 11:note[4][i] = 57;
                    break;
                case 10:note[4][i] = 59;
                    break;


                case 9:note[4][i] = 60; /// 中央4C
                    break;
                case 8:note[4][i] = 62;///2;
                    break;
                case 7:note[4][i] = 64;///3;
                    break;
                case 6:note[4][i] = 65;///4;
                    break;
                case 5:note[4][i] = 67;///5;
                    break;
                case 4:note[4][i] = 69;///6;
                    break;
                case 3:note[4][i] = 71;///7;
                    break;

                case 2:note[4][i] = 72; /// 高音4C
                    break;
                case 1:note[4][i] = 74;///2;
                    break;
                case 0:note[4][i] = 76;///3;
                    break;
                case -1:note[4][i] = 77;///4;
                    break;
                case -2:note[4][i] = 79;///5;
                    break;
                case -3:note[4][i] = 81;///6;
                    break;
                case -4:note[4][i] = 83;///7;
                    break;


                case -5:note[4][i] = 84;///1; 高高音4C
                    break;
                case -6:note[4][i] = 86;///2;
                    break;
                case -7:note[4][i] = 88;///3;
                    break;
                case -8:note[4][i] = 89;///4;
                    break;
                case -9:note[4][i] = 91;///5;
                    break;
                case -10:note[4][i] = 93;///6;
                    break;
                case -11:note[4][i] = 95;///7;
                    break;
            }
        }
//        else
    }





///顯示頭
    for(int go_note = 0 ; go_note < note_count ; go_note++)
    {
        rectangle( debug_img, Point(note[0][go_note],note[1][go_note]), Point( note[0][go_note] + template_img.cols ,note[1][go_note] + template_img.rows ), Scalar(255,0,0), 2, 8, 0 );
    }

/// debug整合
///    imshow("after_temp_solid_line_show",debug_img);
///    waitKey(0);


}
