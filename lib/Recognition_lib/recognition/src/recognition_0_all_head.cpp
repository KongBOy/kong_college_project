/*
這個程式的功能是：
統整 找出所有譜上記號 的方法
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


#include "recognition_0_array_tools.h"
#include "recognition_0_debug_tools.h"

#include "recognition_0_vertical_map_to_speed_up.h"
#include "recognition_1_find_all_maybe_head.h"
///分開寫的原因：一、因為程式碼太長所以切兩半， 二、不用寫if分case，執行應該會快一咪咪
#include "recognition_2_a_head_charactaristic.h"
#include "recognition_2_b_head_recheck.h"
#include "recognition_3_a_find_vertical_bar.h"
#include "recognition_3_b_find_time_bar.h"
#include "recognition_3_c_merge_head_and_time.h"
#include "recognition_4_find_picth.h"

using namespace cv;
using namespace std;

///template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head

void recognition_0_all_head( int head_type,
                             Mat staff_img_erase_line,    /// 消掉五線譜線的圖
                             Mat staff_img,
                             int e_count, int* l_edge, int* distance, ///從 recognition_0 的資料來 加速
                             int pitch_base_y,
                             int& note_count,
                             int note[][1000]){
    // 自己設資料結構 head, 0是左上角x, 1是左上角y, 2是similarity
    int maybe_head_count = 0;
    float maybe_head[3][200];
    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            maybe_head[i][j] = 0;


    // 自己設資料結構 line
    short bars    [3][200];  //[0]頂點x [1]頂點y [2]長度
    bool  bars_dir[2][200];  //[0]左(下, TOPTODOWN) [1]右(上, DOWNTOTOP), 需要方向的原因是需要找 8, 16, 32, 64, ... 分音符的橫線, 所以 標記往哪個方向走 來找 會輕鬆很多

    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            bars[i][j] = 0;

    for(int i = 0 ; i < 2 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            bars_dir[i][j] = false;

    int bars_count = 0;

    int bars_time[200];
    for(int i = 0 ; i < 200 ; i++) bars_time[i] = 0;

    /*
    // 自己設資料結構 note
    int note[5][300];  // 0 = x , 1 = y , 2 = type , 3 = time_bar , 4 = 音高
    int note_count = 0;
    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j < 300 ; j++)
            note[i][j] = 0;
    */

    // ● 0 全音符
    // 1 全休止符
    // ● 2 二分 音符
    // 3 二分 休止符
    // ● 4 四分 音符
    // ● 5 四分 休止符


    /// note 可以在：
    /// recognition_2_b_recheck 存全音符，四分休止符 只有他一個，所以寫在下面~~~
    /// recognition_2_c_merge_head_and_time 存 二分音符 和 四分音符，因為兩個重複所以寫進去recognition_2_c裡面~~

    switch(head_type){
        // 0 全音
        case 0:{
            Mat template_img = imread("Resource/note/0/0.bmp",0);
            recognition_1_find_all_maybe_head(template_img, staff_img_erase_line, e_count, l_edge, distance, maybe_head_count, maybe_head, pitch_base_y, "method1");

            recognition_2_b_head_recheck(0, staff_img_erase_line, maybe_head_count, maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 0;
                note_count++;
            }
            // recognition_5_find_pitch(staff_img,template_img,note_count,note,pitch_base_y);
        }
        break;

        // 2 二分
        case 2:{
            Mat debug_img = staff_img_erase_line.clone();
            cvtColor(staff_img_erase_line,debug_img,CV_GRAY2BGR);

            Mat template_img = imread("Resource/note/2/2.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");
            // recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            // list_head_info(maybe_head_count,maybe_head);
            // list_lines_info(lines_count,lines,lines_dir);
            // watch_head(debug_img,template_img,maybe_head_count,maybe_head);


            recognition_3_a_find_vertical_bar (   template_img, staff_img_erase_line,            maybe_head_count, maybe_head, bars_count, bars, bars_dir);

            recognition_4_merge_head_and_time (2,template_img,staff_img_erase_line,maybe_head_count,maybe_head,bars_count,bars,bars_dir,bars_time,note_count,note);
            // recognition_5_find_pitch(staff_img,template_img,note_count,note,pitch_base_y);
        }
        break;

        // 4 四分
        case 4:{
            Mat template_img = imread("Resource/note/4/4.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");


            recognition_2_a_head_charactristic(4,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(4,staff_img_erase_line,maybe_head_count,maybe_head);


            recognition_3_a_find_vertical_bar(template_img,staff_img_erase_line,maybe_head_count,maybe_head,bars_count,bars,bars_dir);
            recognition_3_b_find_time_bar(template_img,staff_img_erase_line,bars_count,bars,bars_dir,bars_time);

            // cout<<"end 3~~~~~~~~~~~~~~~~~~~~~~~~~~ "<<endl;
            // list_head_info(maybe_head_count,maybe_head);
            // list_bars_info(bars_count,bars,bars_dir);
            // watch_head(debug_img,template_img,maybe_head_count,maybe_head);


            recognition_4_merge_head_and_time(4,template_img,staff_img_erase_line,maybe_head_count,maybe_head,bars_count,bars,bars_dir,bars_time,note_count,note);
            // recognition_5_find_pitch(staff_img,template_img,note_count,note,pitch_base_y);
        }
        break;

        // 4-rest
        case 5:{
            Mat template_img = imread("Resource/note/4-rest/4-rest.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            recognition_2_b_head_recheck(5,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;

        // 全休止
        case 1:{
            Mat template_img = imread("Resource/note/0-rest/0-rest.bmp",0);
            recognition_1_find_all_maybe_head(template_img, staff_img_erase_line, e_count, l_edge, distance, maybe_head_count, maybe_head, pitch_base_y, "method1");

            recognition_2_a_head_charactristic(1,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(1,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;

        // 二分休止
        case 3:{
            Mat template_img = imread("Resource/note/2-rest/2-rest.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            recognition_2_a_head_charactristic(3,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(3,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;

        // 十六分休止
        case 6:{
            Mat template_img = imread("Resource/note/6-rest/6-rest-2.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            recognition_2_a_head_charactristic(6,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(6,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;

        // 三十二分休止
        case 7:{
            Mat template_img = imread("Resource/note/32-rest/7-2.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            recognition_2_a_head_charactristic(7,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(7,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;

        // 八分休止
        case 8:{
            Mat template_img = imread("Resource/note/8-rest/8-rest-3.bmp",0);
            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method1");

            recognition_2_a_head_charactristic(8,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(8,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++)
            {
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = head_type;
                note_count++;
            }
        }
        break;


        // 高音譜記號
        case 9:{
            Mat template_img = imread("Resource/note/9/9-bin.bmp",0);
            Mat template_img_4(13,17,CV_8UC1,Scalar(0)); ///隨便拉~~~只是設定range比較好用ˊ口ˋ

            recognition_1_find_all_maybe_head(template_img,staff_img_erase_line,e_count,l_edge,distance,maybe_head_count,maybe_head,pitch_base_y, "method2");

            recognition_2_b_head_recheck(9,staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 9;
                note_count++;
            }

            bubbleSort_note(note_count,note,Y_INDEX);
            bubbleSort_note(note_count,note,X_INDEX);


            Mat debug_img = staff_img_erase_line.clone();
            cvtColor(staff_img_erase_line,debug_img,CV_GRAY2BGR);

            for(int go_special_index = 0 ; go_special_index < note_count ; go_special_index++){
                // 如果距離 高低音譜記號 夠進 的頭 就消掉
                if(note[2][go_special_index] == 9)
                {
                    /// 一、先訂出高低音譜記號是哪一顆
                    int dist_error = 2;
                    int special_note_index = go_special_index;
                    int special_note_x = note[0][special_note_index];
                    int special_note_y = note[1][special_note_index];

                    /// 二、如果不是高低音譜記號的note距離 高低音譜記號 夠進 的頭 就消掉
                    for(int go_note = 0 ; go_note < note_count ; go_note++){
                        cout<<"special_go_note = "<<go_note<<endl;
                        if(note[2][go_note] != 9){
                            int distance_x = note[0][go_note] - special_note_x;
                            int distance_y = note[1][go_note] - special_note_y;

                            ///乾乾乾range要寫好呀!!!!別亂寫ˊ_>ˋ
                            if( distance_x >= 0 - dist_error && distance_x <= template_img.cols - template_img_4.cols + dist_error &&
                                distance_y >= 0 - dist_error && distance_y <= template_img.rows - template_img_4.cols + dist_error){
                                cout<<"head_x = "<<note[0][go_note]
                                    <<" , head_y = "<<note[1][go_note]
                                    <<" , special_remove~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                                rectangle(debug_img,Point(note[0][go_note],note[1][go_note]),
                                                    Point(note[0][go_note]+template_img_4.cols,note[1][go_note]+template_img_4.rows),Scalar(0,0,255),3);
                                position_erase_note(note_count,note,go_note);
                                go_note--;
                            }

                            ///如果距離高低音譜記號太遠了就break囉！不用全部的頭都跑完拉！
                            else if(distance_x > template_img.cols - template_img_4.cols + dist_error &&
                                    distance_y >= 0 - dist_error && distance_y <= template_img.rows - template_img_4.cols + dist_error) break;
                        }
                    }
                }
                /// debug整合
                // imshow("debug",debug_img);
                // waitKey(0);
            }
            // recognition_5_find_pitch(staff_img,template_img,note_count,note,pitch_base_y);
        }
        break;
    }
}
