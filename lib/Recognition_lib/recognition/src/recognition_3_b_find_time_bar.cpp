/*
這個程式的功能是：
找出符桿的時間長度
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_3_b_find_time_bar.h"

#define DOWNTOTOP 0
#define TOPTODOWN 1

using namespace cv;
using namespace std;

int find_lines_time( Mat reduce_line ,int line_x,int line_y,int left , int right ,int test_depth , bool direction ,Mat & debug)
{
    const int time_bar_type_count = 5; /// 0 1 2 3  ， 4 是垃圾桶，所以共五種
    int white_coda = 0;
    int white_ok_coda = 5;
    bool have_black = false;
    int error_shift = 13;

    int stand_x = left; /// 不會動、固定的x
    int stand_y = line_y;    /// 不會動、固定的y
///    cout<<"stand_x = "<<stand_x <<" , stand_y = "<<stand_y<<endl;

    int time_bar[time_bar_type_count];
    for(int i = 0 ; i < time_bar_type_count ; i++) time_bar[i] = 0;
    if(direction == DOWNTOTOP)
    {
        for(int go_x = stand_x ; go_x <= right ; go_x++)
        {
            /// 怕 time_bar 是斜的~~所以往右看的時候一開始就是白連續的一堆白色~~所以鮮位移到正確的位置
            white_coda = 0; ///因為我現在是要做很多次，所以white_coda要設回initial值！要不然上個的結果會被繼續用!!!
            stand_y = line_y;
            for(int i = 0 ; i < error_shift ; i++)
            {
            /// *********** 防呆 ***********
                if( (stand_y +i +1 > reduce_line.rows -1) ||
                    (stand_y +i    > reduce_line.rows -1) ||
                    (stand_y -i +1 < 0) ||
                    (stand_y -i    < 0)   )break;
            /// *********** 防呆 ***********

                if( (reduce_line.at<uchar>(stand_y +i +1 ,go_x) == 255) && (reduce_line.at<uchar>(stand_y +i ,go_x) == 0) )
                {
                    stand_y += i;
                    break;
                }
                if( (reduce_line.at<uchar>(stand_y -i +1 ,go_x) == 255) && (reduce_line.at<uchar>(stand_y -i ,go_x) == 0) )
                {
                    stand_y -= i;
                    break;
                }
            }

            /// 開始移動 go_y
            int go_y = stand_y;
            ///**************************
            ///line(debug,Point(go_x,stand_y),Point(go_x,stand_y - test_depth),Scalar(123,245,210),1);

            for(int go_depth = 0 ; go_depth < test_depth ; go_depth++)
            {
                if( stand_y - go_depth < 0) break; /// 防呆
                if( reduce_line.at<uchar>( stand_y - go_depth ,go_x) == 0 )
                {
                    go_y -= white_coda; /// 如果之前有測到white，white_coda要減回來，如果沒有測到white，反正white_coda會是0所以減了也沒差，所以不用寫if分case囉！
                    go_y--;
                    white_coda = 0;

                    line(debug,Point(go_x,stand_y - go_depth),Point(go_x,stand_y - go_depth),Scalar(255,0,0),2);
                    line(debug,Point(go_x,go_y),Point(go_x,go_y),Scalar(230,100,150),2);
                    ///imshow("time",debug);
                    ///waitKey(0);
                }
                else
                {
                    white_coda++;
                }
                if(white_coda > white_ok_coda) break; /// 連續出現white_ok_coda個全白，就代表找到頭了
            }

            int time_bar_length = abs(go_y - stand_y);
            if(time_bar_length > 0) time_bar_length = time_bar_length / 13 +1;

            if(time_bar_length < 4 ) time_bar[time_bar_length]++;
            else time_bar[4]++;

            ///cout註解 看找玩time_bar的狀況
            ///cout<<"left = "<<left<<" , right = "<<right<<" , go_x = "<<go_x<<" , bar = "<<abs(go_y - stand_y)<<endl;
            line( debug , Point(go_x,go_y), Point(go_x,go_y), Scalar(0,0,255), 3, CV_AA);
            ///imshow("debug",debug);
            ///waitKey(0);
            ///找右邊的線  END 每條線的 線頭(right_line_t)都應該要被找到囉！
        }

    }
    else if(direction == TOPTODOWN)
    {
        for(int go_x = stand_x ; go_x <= right ; go_x++)
        {
             ///cout<<"go_x = "<<go_x<<endl;
            /// 怕 time_bar 是斜的~~所以往右看的時候一開始就是白連續的一堆白色~~所以鮮位移到正確的位置
            white_coda = 0; ///因為我現在是要做很多次，所以white_coda要設回initial值！要不然上個的結果會被繼續用!!!
            stand_y = line_y;
            for(int i = 0 ; i < error_shift ; i++)
            {

            ///    cout<<"i = "<<i<<endl;
            /// *********** 防呆 ***********
               if(  (stand_y -i +1 > reduce_line.rows -1) ||
                    (stand_y +i    > reduce_line.rows -1) ||
                    (stand_y -i -1 < 0) ||
                    (stand_y -i    < 0)   ) break;
            /// *********** 防呆 ***********
                if( (reduce_line.at<uchar>(stand_y +i -1 ,go_x) == 255) && (reduce_line.at<uchar>(stand_y +i ,go_x) == 0) )
                {
                    stand_y += i;
                    break;
                }
                if( (reduce_line.at<uchar>(stand_y -i -1 ,go_x) == 255) && (reduce_line.at<uchar>(stand_y -i ,go_x) == 0) )
                {
                    stand_y -= i;
                    break;
                }
            }

            /// 開始移動 go_y
            int go_y = stand_y;
            ///**************************
///            line(debug,Point(go_x,stand_y),Point(go_x,stand_y + test_depth),Scalar(123,245,210),1);

            for(int go_depth = 0 ; go_depth < test_depth ; go_depth++)
            {
///                cout<<"go_depth = "<<go_depth<<endl;
                if( stand_y + go_depth > reduce_line.rows -1) break; /// 防呆
                if( reduce_line.at<uchar>( stand_y + go_depth ,go_x) == 0 )
                {
                    go_y += white_coda; /// 如果之前有測到white，white_coda要減回來，如果沒有測到white，反正white_coda會是0所以減了也沒差，所以不用寫if分case囉！
                    go_y ++;
                    ///cout<<"white_coda = "<<white_coda<<endl;
                    white_coda = 0;

                    line(debug,Point(go_x,stand_y + go_depth),Point(go_x,stand_y + go_depth),Scalar(255,0,0),2);
                    line(debug,Point(go_x,go_y),Point(go_x,go_y),Scalar(230,100,150),2);
                    ///imshow("time",debug);
                    ///waitKey(0);
                }
                else
                {
                    white_coda++;
                }
                if(white_coda > white_ok_coda) break; /// 連續出現white_ok_coda個全白，就代表找到頭了
            }

            int time_bar_length = abs(go_y - stand_y);
            if(time_bar_length > 0) time_bar_length = time_bar_length / 13 +1;

            if(time_bar_length < 4 ) time_bar[time_bar_length]++;
            else time_bar[4]++;

            line( debug , Point(go_x,go_y), Point(go_x,go_y), Scalar(0,0,255), 3, CV_AA);
            ///找右邊的線  END 每條線的 線頭(right_line_t)都應該要被找到囉！
        }


/// for(int i = 0 ; i < time_bar_type_count ; i++)    cout<<time_bar[i]<<' ';
/// cout<<endl;

    }

    int max_place = 0;
    for(int i = 1 ; i < (time_bar_type_count -1);i++) ///垃圾桶不用比，所以-1
    {
        if(time_bar[max_place] < time_bar[i]) max_place = i;
    }
    return max_place;

}


void recognition_3_b_find_time_bar(Mat template_img,Mat reduce_line,
                                   int lines_count,short lines[][200],bool lines_dir[][200],
                                   int lines_time[200])
{
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line,debug_img,CV_GRAY2BGR);


/// 開始判斷時間長度囉
    /// tr = top-right, tl = top-left , dr = down-right , dl = down_left
    for(int go_line = 0 ; go_line < lines_count ; go_line++)
    {
        ///cout註解 看現在在處理哪條vertical bar
        ///cout<<"go_line = "<<go_line;
        int test_width = 7;
        /// check方格的x方向都相同所以寫出來~~~
        /// ******** 上右 *********
        int shift = 2;
        int time_tr_l = lines[0][go_line] + shift;
        int time_tr_r = time_tr_l +test_width;
        /// 防呆 start
        if(time_tr_l < 0 ) time_tr_l = 0;
        if(time_tr_r > reduce_line.cols-1) time_tr_r = reduce_line.cols -1;
        /// 防呆 end



        /// ******** 上左 ********
        int time_tl_r = lines[0][go_line] - shift;
        int time_tl_l = time_tl_r -test_width;
        /// 防呆 start
        if(time_tl_l < 0 ) time_tl_l = 0;
        if(time_tl_r > reduce_line.cols-1) time_tl_r = reduce_line.cols -1;
        /// 防呆 end
///        cout<<"time_tl_l = "<<time_tl_l<<" , time_tl_r = "<<time_tl_r<<endl;


        /// ******** 下右 *********
        int time_dr_l = time_tr_l; /// 直接copy
        int time_dr_r = time_tr_r; /// 直接copy

        /// ******** 下左 *********
        int time_dl_r = time_tl_r; /// 直接copy
        int time_dl_l = time_tl_l; /// 直接copy


        int test_depth = template_img.rows*4.0;

        /// 上
        if(lines_dir[0][go_line] == false && lines_dir[1][go_line] == true)
        {
            int length_r = find_lines_time(reduce_line,lines[0][go_line],lines[1][go_line],time_tr_l,time_tr_r,test_depth,TOPTODOWN,debug_img);
///            cout<<"time_tl_l = "<<time_tl_l<<" , time_tl_r = "<<time_tl_r<<endl;
            int length_l = find_lines_time(reduce_line,lines[0][go_line],lines[1][go_line],time_tl_l,time_tl_r,test_depth,TOPTODOWN,debug_img);
            if(length_l > length_r) lines_time[go_line] = length_l;
            else lines_time[go_line] = length_r;

            ///cout註解 看找到的time bar有多長
            ///cout<<" , length = "<<lines_time[go_line]<<endl;

            ///lines_time[1][go_line] = time_tl_d - time_tl_t;
            ///if(lines_time[1][go_line] > 0) lines_time[1][go_line] = lines_time[1][go_line]/11 +1;
            ///cout<<"length_r = "<<lines_time[0][go_line]<<" , length_l = "<<lines_time[1][go_line]<<endl;

            ///imshow("time",debug_img);
            ///waitKey(0);


        }

        /// 下
        if(lines_dir[0][go_line] == true && lines_dir[1][go_line] == false)
        {
///            cout<<"time_td_l = "<<time_tl_l<<" , time_td_r = "<<time_tl_r<<endl;
            int length_r = find_lines_time(reduce_line,lines[0][go_line],lines[1][go_line]+lines[2][go_line],time_dr_l,time_dr_r,test_depth,DOWNTOTOP,debug_img);
            int length_l = find_lines_time(reduce_line,lines[0][go_line],lines[1][go_line]+lines[2][go_line],time_dl_l,time_dl_r,test_depth,DOWNTOTOP,debug_img);
            if(length_l > length_r) lines_time[go_line] = length_l;
            else lines_time[go_line] = length_r;

            ///cout註解 看找到的time bar 有多長
            ///cout<<" , length = "<<lines_time[go_line]<<endl;
            ///imshow("time",debug_img);
            ///waitKey(0);

        }

        ///cout<<endl;

    }

    ///***************
    ///imshow("time",debug_img);

    ///debug整合
    ///imshow("debug",debug_img);
    ///waitKey(0);
    ///waitKey(0);
}
