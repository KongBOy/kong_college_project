/*
這個程式的功能是：
統整 找出所有譜上記號 的方法
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


#include "recognition_0_array_tools.h"
#include "Bar_tool.h"

#include "recognition_0_vertical_map_to_speed_up.h"
#include "recognition_1_find_all_maybe_head.h"
///分開寫的原因：一、因為程式碼太長所以切兩半， 二、不用寫if分case，執行應該會快一咪咪
#include "recognition_2_a_head_charactaristic.h"
#include "recognition_2_b_head_recheck.h"
#include "recognition_3_a_find_vertical_bar.h"
#include "recognition_3_b_find_time_bar.h"
#include "recognition_3_c_merge_head_and_time.h"
#include "recognition_4_find_picth.h"

#include "Note_infos.h"
#include "Maybe_head_tool.h"

#define ERASE 0
#define ASSIGN_8_Note 1

using namespace cv;
using namespace std;

void Overlap_Erase_or_Assing8Note(const int head_type, const Mat head_template, int note[][1000], int& note_count, Mat staff_img_erase_line, int dist_error, int method, int top_extend){
    // method: 
    //     ERASW        : 會把跟 head_type 有 Overlap的head 清除
    //     ASSIGN_8_Note: 主要是給 head_type == 八分符桿用的, 可以把 跟八分符桿 往上 top_extend 的區域 有Overlap 的head 的 time_bar 指定為1 代表八分音符
    // top_extend : 找Overlap時 要往上延伸多少pixel
    bubbleSort_note(note_count, note, Y_INDEX);
    bubbleSort_note(note_count, note, X_INDEX);
    
    Mat debug_img;
    cvtColor(staff_img_erase_line, debug_img, CV_GRAY2BGR);

    Mat template_img_casual(13,17,CV_8UC1,Scalar(0));  // 常見的頭的大小
    int special_note_index;
    int special_note_x;
    int special_note_y;

    // 二、第二層走訪所有note 如果不是高低音譜記號的note距離 高低音譜記號 夠近 的頭 就消掉
    int sp_to_note_vec_x;
    int sp_to_note_vec_y;
    int l_limit = -dist_error;
    int t_limit = -dist_error - top_extend;
    int r_limit =  dist_error;
    int d_limit =  dist_error;

    // 以高音譜記號為例, 如果遇到高音譜記號, 只要距離 高低音譜記號 夠近 的頭 就消掉
    for(int go_special_index = 0 ; go_special_index < note_count ; go_special_index++){
        // 第一層走訪所有的note, 如果遇到 高音譜記號的話, 就走第二層走訪所有note 如果不是高音譜記號 距離高音譜記號夠近就刪除
        if(note[2][go_special_index] == head_type)
        {
            // 一、先訂出高低音譜記號的位置
            special_note_index = go_special_index;
            special_note_x = note[0][special_note_index];
            special_note_y = note[1][special_note_index];
            cout << "special_go_note = " << go_special_index << endl;

            // 二、第二層走訪所有note 如果不是高低音譜記號的note距離 高低音譜記號 夠近 的頭 就消掉
            for(int go_note = 0 ; go_note < note_count ; go_note++){
                // 如果不是高音譜記號 距離高音譜記號夠近就刪除
                if(note[2][go_note] != head_type){
                    sp_to_note_vec_x = note[0][go_note] - special_note_x;
                    sp_to_note_vec_y = note[1][go_note] - special_note_y;
                    
                    // 上下左右都還有留 dist_error, 以左右為例, 往高音譜記號 左邊邊緣再往左一點點(-dist_error) ~ 高音譜記號 右邊緣再往右一點點(+dist_error), 在這範圍內有重疊就要刪除
                    // 換句話說 就是 超過這個範圍 太遠就不刪, 反過來看 是 因為不想 if 裡面包太多東西, 而且 r_limit, d_limit 又有需要特別注意的東西 全包 if 很難寫
                    //   -
                    //  |𝄞|
                    //   -
                    // 左邊緣(上邊緣) 如果超出範圍 代表離太遠就 continue 不做刪除 
                    if( sp_to_note_vec_x < l_limit ) continue;
                    if( sp_to_note_vec_y < t_limit ) continue;

                    // 右邊緣(下邊緣) 如果超出範圍 代表離太遠就 continue 不做刪除,
                    // 計算右邊緣(下邊緣同理)時注意1:
                    // note 在x軸佔的位置是 note_x + note.cols, 所以右邊緣 記得 把 note_cols 減回來, 要不然會變成 高音譜記號右邊緣 在往外凸出 note.cols 喔
                    r_limit =  head_template.cols - template_img_casual.cols + dist_error;
                    d_limit =  head_template.rows - template_img_casual.cols + dist_error;
                    // 注意2: note.cols 也有可能 > special_note.cols(比如 二分音符.cols > 八分音符符桿.cols), 這樣減完會變負的 右邊緣會跑到左邊緣不對了, 這種情況 就直接指定 dist_error, 代表還是要往右找 dist_error個px重疊的話要刪除
                    if( r_limit < dist_error ) r_limit = dist_error;
                    if( d_limit < dist_error ) d_limit = dist_error;
                    if( sp_to_note_vec_x > r_limit ) break;  // 因為 note對x 已經有排過序, 如果距離高低音譜記號右邊太遠了就break囉！剩下的一定都離太遠不用全部的頭都跑完拉！
                    if( sp_to_note_vec_y > d_limit ) continue;
                    
                    cout << "head_x = "<<note[0][go_note] << ", head_y = "<<note[1][go_note] << ", special_remove~ " << endl;
                    rectangle(debug_img,Point(note[0][go_note]                           , note[1][go_note]),
                                        Point(note[0][go_note] + template_img_casual.cols, note[1][go_note] + template_img_casual.rows), Scalar(0, 0, 255), 3);
                    switch(method){
                        case ERASE:{
                            position_erase_note(note_count, note,go_note);   
                            go_note--;
                        }
                        break;

                        case ASSIGN_8_Note:{
                            note[3][go_note] = 1;
                        }
                        break;
                    }
                }
            }
        }
        /// debug整合
        // imshow("debug",debug_img);
        // waitKey(0);
    }    
}

void recognition_0_all_head( int head_type,
                             Mat staff_img_erase_line,    /// 消掉五線譜線的圖
                             Mat staff_img,
                             int e_count, int* l_edge, int* distance, ///從 recognition_0 的資料來 加速
                             int pitch_base_y,
                             int& note_count,
                             int note[][1000],
                             bool debuging){
    // 自己設資料結構 head, 0是左上角x, 1是左上角y, 2是similarity
    int maybe_head_count = 0;
    float maybe_head[3][200];
    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            maybe_head[i][j] = 0;


    // 自己設資料結構 line
    short bars    [3][200];  //[0]頂點x [1]頂點y [2]長度
    bool  bars_dir   [200];  //[0]左(下, TOPTODOWN) [1]右(上, DOWNTOTOP), 需要方向的原因是需要找 8, 16, 32, 64, ... 分音符的橫線, 所以 標記往哪個方向走 來找 會輕鬆很多

    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            bars[i][j] = 0;

    for(int i = 0 ; i < 200 ; i++)
        bars_dir[i] = false;

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
    
    Mat staff_result_map;
    switch(head_type){
        // 0 全音
        case 0:{
            Mat template_img = imread("Resource/note/0/0.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count, l_edge, distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img);

            recognition_2_b_head_recheck(0, template_img, staff_img_erase_line, maybe_head_count, maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 0;
                note_count++;
            }
        }
        break;

        // 2 二分
        case 2:{
            Mat debug_img = staff_img_erase_line.clone();
            cvtColor(staff_img_erase_line,debug_img,CV_GRAY2BGR);

            Mat template_img = imread("Resource/note/2/2_hard_to_find1.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method1", debuging);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method2", debuging);
            template_img = imread("Resource/note/2/2_hard_to_find2.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method1", debuging);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method2", debuging);
            template_img = imread("Resource/note/2/2_hard_to_find3.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method1", debuging);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method2", debuging);
            template_img = imread("Resource/note/2/2.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method1", debuging);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img_erase_line, e_count,l_edge,distance, "method2", debuging);
            staff_result_map /= 8;
    
            Grab_MaybeHead_from_ResultMap    (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img);
            // MaybeHead_list_infos(maybe_head_count,maybe_head);
            // cv::waitKey(0);

            recognition_2_a_head_charactristic(2, template_img, staff_img_erase_line, staff_img, maybe_head_count, maybe_head);
            recognition_2_b_head_recheck      (2, template_img,  staff_img_erase_line,            maybe_head_count, maybe_head);
            recognition_2_a_head_charactristic(2, template_img, staff_img_erase_line, staff_img, maybe_head_count, maybe_head);

            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 2;
                note_count++;
            }
        }
        break;

        // 4 四分
        case 4:{
            Mat template_img = imread("Resource/note/4/4.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img);

            recognition_2_a_head_charactristic(4,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(4, template_img, staff_img_erase_line,maybe_head_count,maybe_head);

            recognition_3_a_find_vertical_bar(template_img,staff_img_erase_line,maybe_head_count,maybe_head,bars_count,bars,bars_dir);
            recognition_3_b_find_time_bar(template_img,bars_count,bars,bars_dir,bars_time,staff_img_erase_line);

            // MaybeHead_list_infos(maybe_head_count,maybe_head);
            // list_Bars_infos(bars_count,bars,bars_dir);

            recognition_4_merge_head_and_time(4,template_img,staff_img_erase_line,maybe_head_count,maybe_head,bars_count,bars,bars_dir,bars_time,note_count,note);
        }
        break;

        // 4-rest
        case 5:{
            Mat template_img = imread("Resource/note/4-rest/4-rest.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img);

            recognition_2_b_head_recheck(5, template_img, staff_img_erase_line,maybe_head_count,maybe_head);
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
            Mat template_img = imread("Resource/note/0-rest/0_rest_w_line.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img, e_count, l_edge, distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img, template_img, 0.80);

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
            Mat template_img = imread("Resource/note/2-rest/2_rest_w_line.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img, staff_img, e_count, l_edge, distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img, template_img, 0.80);

            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
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
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img, 0.35);

            recognition_2_a_head_charactristic(6,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(6, template_img, staff_img_erase_line,maybe_head_count,maybe_head);
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
            Mat template_img = imread("Resource/note/32-rest/7-1-up15w.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            template_img = imread("Resource/note/32-rest/7-1-up15w-down15w.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            staff_result_map /= 2;
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img, 0.38);
            
            recognition_2_b_head_recheck(7, template_img, staff_img_erase_line,maybe_head_count,maybe_head);
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
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            template_img = imread("Resource/note/8-rest/8-rest.bmp",0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method1", debuging);
            staff_result_map /= 2;
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img, 0.15);
            
            recognition_2_a_head_charactristic(8,template_img,staff_img_erase_line,staff_img,maybe_head_count,maybe_head);
            recognition_2_b_head_recheck(8, template_img, staff_img_erase_line,maybe_head_count,maybe_head);

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


        // 高音譜記號, 最後記得 要用 Overlap_Erase_or_Assing8Note 把 高音譜記號範圍裡面找錯的 note 刪除喔
        case 9:{
            Mat template_img = imread("Resource/note/9/9-bin.bmp",0); 
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method2", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img);

            recognition_2_b_head_recheck(9, template_img, staff_img_erase_line,maybe_head_count,maybe_head);
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 9;
                note_count++;
            }

            bubbleSort_note(note_count, note, Y_INDEX);
            bubbleSort_note(note_count, note, X_INDEX);

            Overlap_Erase_or_Assing8Note(9, template_img, note, note_count, staff_img_erase_line, 2, ERASE, 0);
        }
        break;

        // 八分音符 符桿
        case 10:{
            Mat template_img = imread("Resource/note/10/10-1.bmp", 0);
            recognition_1_find_all_MaybeHead(staff_result_map, template_img,staff_img_erase_line,e_count,l_edge,distance, "method2", debuging);
            Grab_MaybeHead_from_ResultMap   (staff_result_map, maybe_head_count, maybe_head, pitch_base_y, staff_img_erase_line, template_img, 0.85);
            
            for(int go_head = 0 ; go_head < maybe_head_count ; go_head++){
                int go_note = note_count;
                note[0][note_count] = maybe_head[0][go_head];
                note[1][note_count] = maybe_head[1][go_head];
                note[2][note_count] = 10;
                note_count++;
            }
            Overlap_Erase_or_Assing8Note(10, template_img, note, note_count, staff_img_erase_line, 10, ERASE, 0);
            Overlap_Erase_or_Assing8Note(10, template_img, note, note_count, staff_img_erase_line, 10, ASSIGN_8_Note, template_img.rows / 2);
        }
        break;
    }
}
