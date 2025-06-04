/*
這個程式的功能是：
把 recognition_1 找到的 可能是的符號 依據 輸入符號的特徵做篩選
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
/// void recognition_1_find_all_maybe_head
/// (Mat reduce_line, int e_count, int* l_edge, int* distance,int& maybe_head_count,float maybe_head[][200])

#include "recognition_0_array_tools.h"
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include "recognition_2_a_head_charactaristic.h"

using namespace cv;
using namespace std;

float black_count_function(Mat image,int top,int down, int left , int right , Mat& temp_show2) ///都是 array index
{
    int black_count = 0;
    for(int go_row = top ; go_row <= down  ; go_row++)
    {
        for(int go_col = left ; go_col <= right ; go_col++)
        {
            if(!image.at<uchar>(go_row,go_col))
            {
                black_count++;

                line(temp_show2,Point(go_col,go_row),Point(go_col,go_row),Scalar(0,0,255),1); ///標出位置

            }
///                    if(not_solid) break; /// 兩層都要加！
        }
///                if(not_solid) break; /// 兩層都要加！
    }


    /// 小心如果要show的話需要 if(not_solid == false) 要不然頭剛被刪掉的話 go_head會 == -1，讀資料會gg
    float test_area = (down-top+1)*(right-left+1);
    float black_rate = black_count/test_area;
    ///cout註解 看全休止符和二分休止符 左右的狀況~~
    ///cout<<"test_area = "<<test_area
    ///    <<" , black_count = "<<black_count
    ///    <<" , black_rate = "<<black_rate<<endl;
    return black_rate;
}


void recognition_2_a_head_charactristic(int head_type,Mat template_img,Mat reduce_line,Mat cut_ord_img,int& maybe_head_count,float maybe_head[][200])
{
/// 要記得分case喔！！


/// CHECK SOLID using my data_structure
    Mat temp_show2 = reduce_line.clone();
    cvtColor(reduce_line,temp_show2,CV_GRAY2BGR);

    switch(head_type)
    {
        case 2:
        case 4:
        {
            int check_size = 6;
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
            ///    cout<<"x = "<<maybe_head[0][i]<<", y = "<<maybe_head[1][i]<<", value = "<<maybe_head[2][i]<<endl;
                Point check_left_top(maybe_head[0][go_head] + 0.5*template_img.cols - check_size/2 ,
                                     maybe_head[1][go_head] + 0.5*template_img.rows - check_size/2 );
                rectangle( temp_show2, Point(check_left_top.x,check_left_top.y), Point(check_left_top.x + check_size,check_left_top.y + check_size), Scalar(255,200,100), 1, 8, 0 );

                bool not_solid = false;
                for(int i = check_left_top.y ; i < check_left_top.y+check_size ; i++)
                {
                    for(int j = check_left_top.x ; j < check_left_top.x+check_size ; j++)
                    {
                        if(reduce_line.at<uchar>(i,j))
                        {
                            not_solid = true;

                            line(temp_show2,Point(j,i),Point(j,i),Scalar(0,0,255),3); ///標出位置
                            if(head_type == 4)
                            {
                                position_erase(maybe_head_count,maybe_head,go_head);
                                go_head--;
                                // maybe_head_count--; ////已經寫進去postioin_erase裡面了
                            }
                            else if(head_type == 2) ; /// do_nothing
                        }
                        if(not_solid) break; /// 兩層都要加！
                    }
                    if(not_solid) break; /// 兩層都要加！
                }
                /// 小心如果要show的話需要 if(not_solid == false) 要不然頭剛被刪掉的話 go_head會 == -1，讀資料會gg

                if(head_type == 2)
                {
                    if(not_solid == true) ; ///do_nothing
                    else
                    {
                        position_erase(maybe_head_count,maybe_head,go_head);
                        go_head--;
                    }
                }

            }
        }
        break;

        case 1:
        case 3:
        {
            int check_width = 6;
            int shift = 2;
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
            ///    cout<<"x = "<<maybe_head[0][i]<<", y = "<<maybe_head[1][i]<<", value = "<<maybe_head[2][i]<<endl;
                int l_right = maybe_head[0][go_head] -1 +shift;
                int l_left  = l_right - check_width;
                ///int l_left  = maybe_head[0][go_head] - template_img.cols +shift;
                ///int l_right = l_left + template_img.cols -1;

                int r_left  = maybe_head[0][go_head] + template_img.cols +1 -shift;
                int r_right = r_left + check_width;
                ///int r_right = maybe_head[0][go_head]+template_img.cols*2 -shift;
                ///int r_left  = r_right - template_img.cols +1;

                int l_top = 0;
                int l_down = 0;

                if(head_type == 1)
                {
                    l_top  = maybe_head[1][go_head];///-shift;
                    l_down = l_top + check_width;

                }
                else if(head_type == 3)
                {
                    l_down = maybe_head[1][go_head] + template_img.rows;/// +shift;
                    l_top = l_down - check_width;
                }
                int r_top  = l_top;
                int r_down = l_down;


                rectangle( temp_show2, Point(l_left,l_top), Point(l_right,l_down), Scalar(255,200,100), 1, 8, 0 );
                rectangle( temp_show2, Point(r_left,r_top), Point(r_right,r_down), Scalar(255,200,100), 1, 8, 0 );

                float l_black_rate = black_count_function(cut_ord_img,l_top,l_down,l_left,l_right,temp_show2);
                float r_black_rate = black_count_function(cut_ord_img,r_top,r_down,r_left,r_right,temp_show2);

                ///cout註解 看全休止符和二分休止符 左右的狀況~~
                ///cout<<"l_black_rate = "<<l_black_rate<<" , r_black_rate = "<<r_black_rate<<endl;

                if(l_black_rate < 0.1 || r_black_rate < 0.1)
                {
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
                /*
                if(black_rate > 0.25) ///24~32格~~裡面有5~9格以上都是黑色的~~太多囉!!!!
                {
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
                */
            }
        }
        break;


    /// head_type = 6,7,8
        case 6:
        case 7:
        case 8:
        {
            int check_width = 3;
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
            ///    cout<<"x = "<<maybe_head[0][i]<<", y = "<<maybe_head[1][i]<<", value = "<<maybe_head[2][i]<<endl;
                int right = maybe_head[0][go_head]+template_img.cols ;
                int left = right - template_img.cols/2;
                int down = maybe_head[1][go_head]-4;
                int top = down - check_width;

                rectangle( temp_show2, Point(left,top), Point(right,down), Scalar(255,200,100), 1, 8, 0 );

                int black_count = 0;
                for(int go_row = top ; go_row <= down  ; go_row++)
                {
                    for(int go_col = left ; go_col <= right ; go_col++)
                    {
                        if(!reduce_line.at<uchar>(go_row,go_col))
                        {
                            black_count++;

                            line(temp_show2,Point(go_col,go_row),Point(go_col,go_row),Scalar(0,0,255),1); ///標出位置

                        }
    ///                    if(not_solid) break; /// 兩層都要加！
                    }
    ///                if(not_solid) break; /// 兩層都要加！
                }
                /// 小心如果要show的話需要 if(not_solid == false) 要不然頭剛被刪掉的話 go_head會 == -1，讀資料會gg
                float test_area = (down-top+1)*(right-left+1);
                float black_rate = black_count/test_area;
                ///cout註解 看八分、十六分、三十二分休止的右上角黑色有幾格
                ///cout<<"test_area = "<<test_area
                ///    <<" , black_count = "<<black_count
                ///    <<" , black_rate = "<<black_rate<<endl;
                if(black_rate > 0.25) ///24~32格~~裡面有5~9格以上都是黑色的~~太多囉!!!!
                {
                    position_erase(maybe_head_count,maybe_head,go_head);
                    go_head--;
                }
            }
        }
        break;

    }

    /// 秀出 做完CHECK_SOLID的頭
    draw_head(temp_show2,template_img,maybe_head_count,maybe_head);
///    imshow("Check_solid",temp_show2);

/// debug整合
///    imshow("debug",temp_show2);
///    waitKey(0);
    /// destroyWindow("Check_solid");
}
