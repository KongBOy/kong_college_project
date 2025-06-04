/*
這個程式的功能是：
找出符號的符桿
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_0_array_tools.h"
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include "recognition_3_a_find_vertical_bar.h"

#define DOWNTOTOP 0
#define TOPTODOWN 1

using namespace cv;
using namespace std;


void find_vertical_bar( Mat reduce_line , int left , int right , int& top , int& down , int test_depth , bool direction ,Mat & debug)
{
    int white_coda = 0;
    int white_ok_coda = 3;
    bool have_black = false;
///找右邊的線  START
    if(direction == DOWNTOTOP)
    {
        for(int go_depth = 0 ; go_depth < test_depth ; go_depth++)
        {
            //int right_count = 0;
            have_black = false; /// 不能省這個喔！！因為我宣告丟到外面了！！

            for(int go_width = left ; go_width <= right ; go_width++)
            {
                if( (down - go_depth < 0) || (go_width > reduce_line.cols-1) ) break;///防呆，別犯傻拉請記得用用固定的點防呆呀.....

                if(reduce_line.at<uchar>(down - go_depth , go_width) == 0)
                {
                    have_black = true;
                    top--;
                    line( debug , Point(go_width,top), Point(go_width,top), Scalar(78,91,123), 2, CV_AA);
                    white_coda = 0;
                    break;
                }
            }
            //cout<<top_count;
            if( have_black == false )
            {
            ///    cout<<"now top_white_coda = "<<top_white_coda<<endl;
            ///    line( debug , Point(top_l,top_t - go_depth), Point(top_l,top_t - go_depth), Scalar(0,0,255), 2, CV_AA);
            ///    imshow("white_coda++",debug);
                white_coda++;
            ///    waitKey(0);
            }
            if(white_coda > white_ok_coda) break; /// 連續出現white_ok_coda個全白，就代表找到頭了
        }
        for(int i = 0 ; i < white_ok_coda ; i++) /// 修正 被雜訊影響怕跑過頭了
        {
            if(reduce_line.at<uchar>( top ,(right + left)/2) != 0  && have_black == true) top++;
        }
        line( debug , Point(right,top), Point(right,top), Scalar(0,0,255), 5, CV_AA);
        ///找右邊的線  END 每條線的 線頭(top_t)都應該要被找到囉！
    }
    else if(direction == TOPTODOWN)
    {
        ///找左邊的線 START
        for(int go_depth = 0 ; go_depth < test_depth ; go_depth++)
        {
            //int down_count = 0;
            have_black = false; /// 不能省這個喔！！因為我宣告丟到外面了！！
            for(int go_width = left ; go_width <= right ;go_width++)
            {
                if( (top + go_depth > reduce_line.rows-1) || (go_width > reduce_line.cols-1) ) break;///防呆，別犯傻拉請記得用用固定的點防呆呀.....
                if(reduce_line.at<uchar>(top + go_depth ,go_width) == 0)
                {
                    have_black = true;
                    down++;
                    line( debug , Point(go_width,down), Point(go_width,down), Scalar(210,91,123), 2, CV_AA);
                    white_coda = 0;
                    break;
                }
            }
            if(have_black == false ) white_coda++;
            if(white_coda > 6) break;
        }
        for(int i = 0 ; i < white_ok_coda ; i++) /// 修正 被雜訊影響怕跑過頭了
        {
            if(reduce_line.at<uchar>( down ,(right + left)/2) != 0 && have_black == true) down--;
        }
        line( debug , Point(left,down), Point(left,down), Scalar(0,0,255), 5, CV_AA);
        ///找左邊的線 END 每條線的 線頭(down_d)都應該要被找到囉！
    }

}


void recognition_3_a_find_vertical_bar(Mat template_img ,Mat reduce_line,
                                       int& maybe_head_count,float maybe_head[][200],
                                       int& lines_count,short lines[][200],bool lines_dir[][200])
{
    Mat debug_img = reduce_line.clone();
    cvtColor(reduce_line,debug_img,CV_GRAY2BGR);

    ///check line
    int test_depth = 12*template_img.rows; ///大概三個八度左右囉~~~
    int test_width = 11;
    int test_area = test_depth * test_width * 0.1;
    float test_ok_depth = 1.5*template_img.rows;


    for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
    {
        /// 定出要找的範圍 START
        int i_feel_top_r_shift = -2;
        int top_r_r = maybe_head[0][go_head] + template_img.cols + test_width/2 + i_feel_top_r_shift;//框框的右邊
        int top_r_l = top_r_r - (test_width) ;//總共想側三格
        int top_r_d = maybe_head[1][go_head] -1; //框框的上一格
        int top_r_t = top_r_d;
        ///cout<<"top_r_distance = "<<top_r_r - top_r_l<<endl;
        rectangle( debug_img, Point(top_r_l,top_r_t-test_depth), Point( top_r_r ,top_r_d ), Scalar(200,155,255), 1, 8, 0 );

        int i_feel_top_l_shift = +2;
        int top_l_l = maybe_head[0][go_head] - test_width/2 + i_feel_top_l_shift;//框框的右邊
        int top_l_r = top_l_l + (test_width) ;//總共想側三格
        int top_l_d = maybe_head[1][go_head] -1; //框框的上一格
        int top_l_t = top_r_d;
        ///cout<<"top_l_distance = "<<top_l_r - top_l_l<<endl;
        rectangle( debug_img, Point(top_l_l,top_l_t-test_depth), Point( top_l_r ,top_l_d ), Scalar(200,155,255), 1, 8, 0 );

        int i_feel_down_l_shift = +2;
        int down_l_l = maybe_head[0][go_head] - test_width/2 + i_feel_down_l_shift; //框框的左邊
        int down_l_r = down_l_l + (test_width) ; // 總共想測三格
        int down_l_t = maybe_head[1][go_head] + template_img.rows +1;//框框的下
        int down_l_d = down_l_t;
        ///cout<<"down_l_distance = "<<down_l_r - down_l_l<<endl;
        rectangle( debug_img, Point(down_l_l,down_l_t), Point( down_l_r ,down_l_d+test_depth ), Scalar(200,155,255), 1, 8, 0 );

        int i_feel_down_r_shift = -2;
        int down_r_l = maybe_head[0][go_head] + template_img.cols - test_width/2 + i_feel_down_r_shift; //框框的左邊
        int down_r_r = down_r_l + (test_width) ; // 總共想測三格
        int down_r_t = maybe_head[1][go_head] + template_img.rows +1;//框框的下
        int down_r_d = down_l_t;
        ///cout<<"down_r_distance = "<<down_r_r - down_r_l<<endl;
        rectangle( debug_img, Point(down_r_l,down_r_t), Point( down_r_r ,down_r_d+test_depth ), Scalar(200,155,255), 1, 8, 0 );

        /// 定出要找的範圍 END


        find_vertical_bar(reduce_line,top_r_l,top_r_r,top_r_t,top_r_d,test_depth,DOWNTOTOP,debug_img);
        find_vertical_bar(reduce_line,top_l_l,top_l_r,top_l_t,top_l_d,test_depth,DOWNTOTOP,debug_img);
        find_vertical_bar(reduce_line, down_r_l, down_r_r, down_r_t, down_r_d,test_depth,TOPTODOWN,debug_img);
        find_vertical_bar(reduce_line, down_l_l, down_l_r, down_l_t, down_l_d,test_depth,TOPTODOWN,debug_img);


        int top_r_length = top_r_d - top_r_t;
        int top_l_length = top_l_d - top_l_t;
        int down_r_length = down_r_d - down_r_t;
        int down_l_length = down_l_d - down_l_t;
        ///cout註解 看一下找完的結果
        ///cout<<   "head.x = "<<maybe_head[0][go_head]
        ///    <<" , head.y = "<<maybe_head[1][go_head]
        ///    <<" , top_line = " << top_l_length <<","<<top_r_length
        ///    <<" , down_line = "<<down_l_length<<","<<down_r_length
        ///    <<endl;

///重要！！不一定所有的線都要存喔！！！存右上+左下的就好了其實！！  多測左上+右下只是不想讓一些特殊的頭被刪掉而已！！
        /// 檢查線的長度，夠長就存起來
        if( down_l_length >= test_ok_depth)
        {
            /// 左下的頂點
            lines[0][lines_count] = maybe_head[0][go_head];///down_l_l + 2;///(maybe_head[0][go_head]+down_l_l)/2 +1;///down_l_l + 2;///+2是看就知道的誤差修正
            lines[1][lines_count] = down_l_t;
            lines[2][lines_count] = down_l_d - down_l_t;
            lines_dir[0][lines_count] = true;
            lines_count++;
        }
        if( top_r_length >= test_ok_depth)
        {
            /// 右上的頂點
            lines[0][lines_count] = maybe_head[0][go_head]+template_img.cols-2;///top_r_l;///(maybe_head[0][go_head]+template_img.cols+top_r_r)/2 -1;///top_r_r -2;///-2是看就知道的誤差修正
            lines[1][lines_count] = top_r_t;
            lines[2][lines_count] = top_r_d - top_r_t;
            lines_dir[1][lines_count] = true;
            lines_count++;
        }

        /// 如果這顆頭"四邊"都找不到線的話(多測左上+右下只是不想讓一些特殊的頭被刪掉而已！！)
        if( ( top_r_length  < test_ok_depth ) && ( top_l_length < test_ok_depth ) &&
            ( down_r_length < test_ok_depth ) && ( down_l_length < test_ok_depth ) )
        {
            position_erase(maybe_head_count,maybe_head,go_head);
            go_head--;
        }
        //imshow("debug",debug_img);
        //waitKey(0);
    }

    bubbleSort_line(lines_count,lines,lines_dir,Y_INDEX);
    bubbleSort_line(lines_count,lines,lines_dir,X_INDEX);

///    list_head_info(maybe_head_count,maybe_head);
///    watch_lines(debug_img,lines_count,lines,lines_dir);



/// 合併線長度，方向還沒合併，合併完就是最長的線
    for(int go_line = 0 ; go_line < lines_count -1 ; go_line++)
    {


        int next = go_line+1;
/*
    Mat debug_img2 = reduce_line.clone();
    cvtColor(reduce_line,debug_img2,CV_GRAY2BGR);

        cout<<"now  , x = "<<lines[0][go_line]<<" , y = "<<lines[1][go_line]<<" , length = "<<lines[2][go_line]<<" , tail = "<<lines[1][go_line]+lines[2][go_line]<<endl;

        line(debug_img2,Point(lines[0][go_line],lines[1][go_line]),
                           Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(0,255,0),2);

        line(debug_img2,Point(lines[0][next],lines[1][next]),
                        Point(lines[0][next],lines[1][next]+lines[2][next]),Scalar(0,0,255),2);

        imshow("lines",debug_img2);
        waitKey(0);
*/

        int distance_x = abs( lines[0][next] - lines[0][go_line] );
        int distance_y = abs( lines[1][next] - lines[1][go_line] );
        ///cout<<"distance_x = "<<distance_x<<endl;
        if( distance_x  <= template_img.cols/2 && distance_y <= template_img.rows*5) ///沒辦法 distance_y 還是要加進來，要不然會被雜線影響
        {
///            cout<<"case merge_line , lines_count = "<<lines_count<<endl;
            if(lines_dir[0][go_line] == lines_dir[0][next])
            {
///                cout<<"now  , x = "<<lines[0][go_line]<<" , y = "<<lines[1][go_line]<<" , length = "<<lines[2][go_line]<<" , tail = "<<lines[1][go_line]+lines[2][go_line]<<endl;
///                cout<<"next , x = "<<lines[0][next]<<" , y = "<<lines[1][next]<<" , length = "<<lines[2][next]<<" , tail = "<<lines[1][next]+lines[2][next]<<endl;

                int tail_y_now = lines[1][go_line] + lines[2][go_line];///線尾的y
                int tail_y_next = lines[1][next] + lines[2][next];
///                cout<<"tail_y_now = "<<tail_y_now<<" , tail_y_next = "<<tail_y_next<<endl;

                if( lines[0][next] < lines[0][go_line] ) lines[0][go_line] = (lines[0][next] + lines[0][go_line])/2 ; ///x存中間
                if( lines[1][next] < lines[1][go_line] ) lines[1][go_line] = lines[1][next]; ///y 存偏上的
                if( tail_y_next > tail_y_now ) tail_y_now = tail_y_next;/// 長度存~~ 最下面的那一點的y~~就線尾的y

                lines[2][go_line] = tail_y_now - lines[1][go_line]; ///長度 存能跑到叫下面的

                //if( lines_dir[0][next] ) lines_dir[0][go_line] = true;
                //if( lines_dir[1][next] ) lines_dir[1][go_line] = true;
                //if( lines[2][next] > lines[2][go_line] && lines_dir[0][next] ) lines_dir[0][go_line] = true; ///方向 存較長的線的方向 不可行，因為反方向的線用我的方法偵測時，會連頭一起算進去，長度會比正確的線還長
                //if( lines[2][next] > lines[2][go_line] && lines_dir[1][next] ) lines_dir[1][go_line] = true; ///方向 存較長的線的方向
                position_erase_line(lines_count,lines,lines_dir,next);
                go_line--;
            }
        }
    }

///    watch_lines(debug_img,lines_count,lines,lines_dir);


/// 合併線的方向，如果還有x距離很近且不同方向的線時，要長度也夠長才有需要合併方向(就是y距離不能太長)
    for(int go_line = 0 ; go_line < lines_count -1 ; go_line++)
    {
        int next = go_line+1;
        int distance_x = abs( lines[0][next] - lines[0][go_line] );
        int distance_y = abs( lines[1][next] - lines[1][go_line] );


/// ~~~~~~ debug ~~~~~~~
/*
    Mat debug_img3 = reduce_line.clone();
    cvtColor(reduce_line,debug_img3,CV_GRAY2BGR);

    cout<<"now  , x = "<<lines[0][go_line]
        <<" , y = "<<lines[1][go_line]
        <<" , length = "<<lines[2][go_line]
        <<" , tail = "<<lines[1][go_line]+lines[2][go_line]
        <<" , left = "<<lines_dir[0][go_line]
        <<" , right = "<<lines_dir[1][go_line]
        <<endl;

    line(debug_img3,Point(lines[0][go_line],lines[1][go_line]),
                       Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(0,255,0),2);

    line(debug_img3,Point(lines[0][next],lines[1][next]),
                    Point(lines[0][next],lines[1][next]+lines[2][next]),Scalar(0,0,255),2);

    imshow("lines",debug_img3);
    waitKey(0);
*/

        if( distance_x  <= 8 && distance_y <= template_img.rows * 4.0) /// 從不同邊找到的線誤差可能會比較大所以用1.5*template_img.rows
        {
            //if(lines_dir[0][go_line] == lines_dir[0][next])
            //{
            ///cout註解 標記一下現在是 合併方向
            ///cout<<"merge_direction : "<<endl;
                int tail_y_now = lines[1][go_line] + lines[2][go_line];///線尾的y
                int tail_y_next = lines[1][next] + lines[2][next];

                if( lines[0][next] < lines[0][go_line] ) lines[0][go_line] = (lines[0][next] + lines[0][go_line])/2; ///x存中間
                if( lines[1][next] < lines[1][go_line] ) lines[1][go_line] = lines[1][next]; ///y 存偏上的
                ///if( lines[2][next] > lines[2][go_line] ) lines[2][go_line] = lines[2][next]; ///長度 存較長的
                if( tail_y_next > tail_y_now ) tail_y_now = tail_y_next;/// 長度存~~ 最下面的那一點的y~~就線尾的y

                lines[2][go_line] = tail_y_now - lines[1][go_line]; ///長度 存能跑到叫下面的

                if( lines_dir[0][next] ) lines_dir[0][go_line] = true;
                if( lines_dir[1][next] ) lines_dir[1][go_line] = true;
                //if( lines[2][next] > lines[2][go_line] && lines_dir[0][next] ) lines_dir[0][go_line] = true; ///方向 存較長的線的方向 不可行，因為反方向的線用我的方法偵測時，會連頭一起算進去，長度會比正確的線還長
                //if( lines[2][next] > lines[2][go_line] && lines_dir[1][next] ) lines_dir[1][go_line] = true; ///方向 存較長的線的方向
                position_erase_line(lines_count,lines,lines_dir,next);
                go_line--;
            //}
        }
    }



/// 把多方向的direction的修正
/// + 把錯的頭消掉(用GROUP的概念後就可以加快找頭的速度)
    Mat debug_img4 = reduce_line.clone();
    cvtColor(reduce_line,debug_img4,CV_GRAY2BGR);

    for(int go_line = 0 ; go_line < lines_count -1 ; go_line++)
    {
        if(lines_dir[0][go_line]==true && lines_dir[1][go_line]==true)
        {
            ///cout註解 標記一下現在是在處理 有多方向的case
            ///cout<<"有多個方向囉!!!!!!!!!!!!!!!!!!!!!"<<endl;

            line(debug_img4,Point(lines[0][go_line],lines[1][go_line]),
                            Point(lines[0][go_line],lines[1][go_line]+lines[2][go_line]),Scalar(0,255,0),2);

            ///lines_dir[0][go_line] = lines_dir[0][go_line +1];
            ///lines_dir[1][go_line] = lines_dir[1][go_line +1];
            int up_head_index = 0;
            int down_head_index = 0;
            int dist_error = 5;
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
                rectangle(debug_img4,Point(maybe_head[0][go_head],maybe_head[1][go_head]),
                                 Point(maybe_head[0][go_head]+template_img.cols,maybe_head[1][go_head]+template_img.rows),Scalar(0,0,255),1);

                int distance_up_x = maybe_head[0][go_head] - lines[0][go_line];
                int distance_up_y = maybe_head[1][go_head] - lines[1][go_line];
///                cout<<"distance_up_x = "<<distance_up_x<<" , distance_up_y = "<<distance_up_y<<endl;

                if( distance_up_x >= -1*(template_img.cols+dist_error) && distance_up_x <= (0 + dist_error) &&
                    distance_up_y >= -1*(template_img.rows+dist_error) && distance_up_y < template_img.rows+dist_error)
                    {
///                        cout<<"get_up_head"<<endl;
                        up_head_index = go_head;
                    }
                else if(distance_up_x > (0 + template_img.cols + dist_error)) break; ///如果頭離線太遠了就break囉！不用全部的頭都跑完拉！

                int distance_down_x = maybe_head[0][go_head] - lines[0][go_line];
                int distance_down_y = maybe_head[1][go_head] - (lines[1][go_line]+lines[2][go_line]);
                if( distance_down_x >= -1*(template_img.cols+dist_error) && distance_down_x <= (0 + dist_error) &&
                    distance_down_y >= -1*(template_img.rows+dist_error) && distance_down_y < template_img.rows+dist_error)
                    down_head_index = go_head;
                else if(distance_down_x > (0 + template_img.cols + dist_error)) break; ///如果頭離線太遠了就break囉！不用全部的頭都跑完拉！

/// ~~~~~~~~~ debug ~~~~~~~~~用來看有兩個方向的線 & 他的頭
///                imshow("lines",debug_img4);
///                waitKey(0);

            }

            rectangle(debug_img4,Point(maybe_head[0][up_head_index],maybe_head[1][up_head_index]),
                                 Point(maybe_head[0][up_head_index]+template_img.cols,maybe_head[1][up_head_index]+template_img.rows),Scalar(123,251,20),3);

            rectangle(debug_img4,Point(maybe_head[0][down_head_index],maybe_head[1][down_head_index]),
                                 Point(maybe_head[0][down_head_index]+template_img.cols,maybe_head[1][down_head_index]+template_img.rows),Scalar(251,123,20),3);
            ///cout註解 看一下造成多方向的上下頭的相似度
            ///cout<<"  up_head_similar_value = "<<maybe_head[2][up_head_index]<<endl;
            ///cout<<"down_head_similar_value = "<<maybe_head[2][down_head_index]<<endl;


            if(maybe_head[2][up_head_index] > maybe_head[2][down_head_index])
            {
                lines_dir[0][go_line] = true;
                lines_dir[1][go_line] = false;
                position_erase(maybe_head_count,maybe_head,down_head_index);
            }
            else if(maybe_head[2][up_head_index] < maybe_head[2][down_head_index])
            {
                lines_dir[0][go_line] = false;
                lines_dir[1][go_line] = true;
                position_erase(maybe_head_count,maybe_head,up_head_index);
            }
            else if(maybe_head[2][up_head_index] = maybe_head[2][down_head_index]) cout<<"something wrong~~"<<endl;
            imshow("lines",debug_img4);
            waitKey(0);

        }
    }


/// 把錯的頭消掉
/// (用GROUP的概念後就可以加快找頭的速度)
    for(int go_line = 0 ; go_line < lines_count -1 ; go_line++)
    {
        /// 右邊
        if(lines_dir[0][go_line] == false && lines_dir[1][go_line] == true)
        {
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
                /// 如果距離 線頭 夠進 的頭 就消掉 (x方向線當 中間 線左一個template，線右一個template 不能有head；
                                                ///  y方向 以線頭當底 下面個template 不能有head )
                                                /// 用絕對值是距離的概念，就不用擔心小減大正負號的問題
                int dist_error = 2;
                int distance_x = maybe_head[0][go_head] - lines[0][go_line];
                int distance_y = maybe_head[1][go_head] - lines[1][go_line];

                ///乾乾乾range要寫好呀!!!!別亂寫ˊ_>ˋ
                if( distance_x >= -1*(template_img.cols+dist_error) && distance_x <= 0 + dist_error &&
                    distance_y >= -1*(template_img.rows+dist_error) && distance_y <= 0 + template_img.rows + dist_error)
                ///if( abs(maybe_head[0][go_head] - lines[0][go_line]) <= template_img.cols/2  &&
                ///    abs(maybe_head[1][go_head] - lines[1][go_line]) <= template_img.rows*1.0)
                {
                    ///cout註解 看消掉的頭的資訊 top case
                    ///cout<<"head_x = "<<maybe_head[0][go_head]
                    ///    <<" , head_y = "<<maybe_head[1][go_head]
                    ///   <<" ,top_r_remove~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                    rectangle(debug_img,Point(maybe_head[0][go_head],maybe_head[1][go_head]),
                                        Point(maybe_head[0][go_head]+template_img.cols,maybe_head[1][go_head]+template_img.rows),Scalar(0,0,255),3);
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
                else if(distance_y > 0 + template_img.rows + dist_error)break; ///如果頭離線太遠了就break囉！不用全部的頭都跑完拉！
            }
        }

        /// 左邊
        if(lines_dir[0][go_line] == true && lines_dir[1][go_line] == false)
        {
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
                /// 如果距離 線尾 夠進 的頭 就消掉 (x方向線當 中間 線左一個template，線右一個template 不能有head；
                                                ///  y方向 以線尾當底 上面個template 不能有head)
                                                /// 用絕對值是距離的概念，就不用擔心小減大正負號的問題
                int dist_error = 2;
                int distance_x = maybe_head[0][go_head] - lines[0][go_line];
                int distance_y = (maybe_head[1][go_head]+template_img.rows) - (lines[1][go_line]+lines[2][go_line]);

                if( distance_x >= -1*(template_img.cols+dist_error) && distance_x <= 0 + dist_error &&
                    distance_y >= -1*(template_img.rows+dist_error) && distance_y <= 0 + template_img.rows + dist_error)

///                if( abs(maybe_head[0][go_head] - (lines[0][go_line] - template_img.cols/2) )<= template_img.cols/2  &&
///                    abs(maybe_head[1][go_head] - (lines[1][go_line] - template_img.rows +lines[2][go_line]) ) <= template_img.rows * 1.0)
                {
                    ///cout註解 看消掉的頭的資訊 down case
                    ///cout<<"head_x = "<<maybe_head[0][go_head]
                    ///    <<" , head_y = "<<maybe_head[1][go_head]
                    ///    <<" ,down_l_remove~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                    rectangle(debug_img,Point(maybe_head[0][go_head],maybe_head[1][go_head]),
                                        Point(maybe_head[0][go_head]+template_img.cols,maybe_head[1][go_head]+template_img.rows),Scalar(0,0,255),3);
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
                else if(distance_y > 0 + template_img.rows + dist_error)break; ///如果頭離線太遠了就break囉！不用全部的頭都跑完拉！
            }
        }
    }


/// 顯示 線


    ///**************************
    ///void draw_lines(Mat& debug_img,int lines_count,short lines[][200],bool lines_dir[][200])

    draw_lines(debug_img,lines_count,lines,lines_dir);
    ///imshow("lines",debug_img);

    ///debug整合
    ///imshow("debug",debug_img);
    ///waitKey(0);

}
