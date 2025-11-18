/*
這個程式的功能是：
整個 辨識 的整合介面
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "Recognition.h"



#include "preprocess_0_watch_hough_line.h"

#include "preprocess_1_warp_straight_rough.h"
#include "preprocess_2_binary.h"
#include "preprocess_3_center_roi.h"
#include "preprocess_3_horizon_map_to_find_line.h"
#include "preprocess_4_distance_detect.h"
#include "preprocess_5_find_staff.h"
#include "preprocess_6_find_head_and_erase_line.h"
#include "preprocess_7_cut_according_staff.h"




#include "recognition_0_array_tools.h"
#include "Note_infos.h"

#include "recognition_0_all_head.h"

#include "recognition_0_vertical_map_to_speed_up.h"
#include "recognition_1_find_all_maybe_head.h"
#include "recognition_2_a_head_charactaristic.h"
#include "recognition_2_b_head_recheck.h"
#include "recognition_3_a_find_vertical_bar.h"
#include "recognition_3_b_find_time_bar.h"
#include "recognition_3_c_merge_head_and_time.h"
#include "recognition_4_find_picth.h"

#include "UI_loading.h"


#define PREPROCESS_DIR "preprocess_"

///////////////////////////////////////////////////////
/*
#define ROI_DIR "2-0_roi/"
#define LINE_DIR  "2-1_find_lines/"
#define STAFF_DIR "2-2_find_staff/"
#define HEAD_DIR "2-3_find_head/"
#define HORIZONTAL_DIR "horizontal_test/"
*/
#define ROI_DIR "debug_preprocess/"
#define LINE_DIR  "debug_preprocess/"
#define STAFF_DIR "debug_preprocess/"
#define HEAD_DIR "debug_preprocess/"
#define HORIZONTAL_DIR "debug_preprocess/"


///////////////////////////////////////////////////////
#define FILE_TYPE ".bmp"
///////////////////////////////////////////////////////

using namespace std;
using namespace cv;

Recognition_page::Recognition_page(const Mat input_img, bool in_debuging):
    ord_img      (input_img), 
    debuging     (in_debuging),
    roi_c_slider (10),
    roi_r_slider (100)
    {
        note_count = 0;
        for(int i = 0 ; i < 5 ; i++)
            for(int j = 0 ; j < 1000 ; j++)
                note[i][j] = 0;

        src_img = ord_img.clone();
        src_bin = src_img.clone();

        debuging_pre1  = false;
        debuging_pre2  = false;
        debuging_pre3a = false;
        debuging_pre3b = false;
        debuging_pre4  = false;
        debuging_pre5  = false;
        debuging_pre6  = false;
        debuging_pre7  = false;

    staff_img_recogs = new Recognition_staff_img*[40];
}

int Recognition_page::run_preprocess(){
    // ************************* pre1 轉正 *************************
    try{
        Find_Angle();
        Wrap_Straight();
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -1;
    }

    // ************************* pre2 二值化 *************************
    try{
        src_bin = src_img.clone();
        Binary_by_patch(src_bin, 15, 40);
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -2;
    }
    
    // ************************* pre3 取影像正中間cols 後 水平投影 來抓五線譜線 *************************
    // 水平投影 找山 和 找線
    // vector<Vec2f> lines;
    src_bin_roi = ~src_bin;
    try{
        // 取影像正中間cols
        Center_ROI_by_slider((string)ROI_DIR + "do_roi");
        // 水平投影 來抓五線譜線
        Horizon_map_to_find_line();
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -3;
    }

    // ************************* pre4 線的 距離階層 找出來 *************************
    // 把線的 距離階層 找出來, 以目前抓出來的階層有三種：
    // dist_level[0]= 3    一條粗線裡面可能有 找到多條細線 之間的距離
    // dist_level[1]= 17   五線譜內五條線大概的距離
    // dist_level[2]= 241  五線譜之間大概的距離
    
    try{
        dist_level = Distance_detect();
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -4;
    }


    // ************************* pre5 利用 dist_level 找出五線譜線 並 組成群組(把 lines 弄成 五條一組, 用5的倍數 來走訪各群, 所以只需return 最終五線譜組數即可) *************************
    try{
        staff_count = find_Staff_lines();
        if(debuging){
            Watch_Hough_Line(lines, src_bin    , "",(string)"debug_img/" + "pre5_staff_line"    , 1066);
            Watch_Hough_Line(lines, src_bin_roi, "",(string)"debug_img/" + "pre5_staff_line_roi"      );
        }
        // 如果 找到0組 五線譜群, 就 return 失敗
        if(staff_count == 0){
            // imshow(Title, UI2_5);
            // waitKey(2000);
            return -5;
        }
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -5;
    }

    // ************************* pre6 每組五線譜群組 的線找頭 存入 left_point, right_point, 並在 順著線跑到頭的過程中 也生出一張 二值化把五線譜刪除的圖 *************************
    try{
        Find_Head_and_Erase_Line_Interface();
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -6;
    }

    // ************************* pre7 每組五線譜群組 找到的頭 的四個角 來透射切出每組五線譜  *************************
    try{
        Cut_staff();
    }
    catch (exception e){
        // imshow(Title, UI2_5);
        // waitKey(2000);
        return -7;
    }
}
Mat    Recognition_page::get_src_img    (){ return src_img; }
Mat    Recognition_page::get_src_bin    (){ return src_bin; }
int    Recognition_page::get_staff_count         (){ return staff_count; }
Mat*   Recognition_page::get_staff_img           (){ return staff_img; }
Mat*   Recognition_page::get_staff_img_erase_line(){ return staff_img_erase_line; }
int*** Recognition_page::get_left_point (){ return left_point; }
int*** Recognition_page::get_right_point(){ return right_point; }
//////////////////////////////////////////////////////////////////////////////
void Recognition_page::set_note_infos_from_staff_imgs(){
    for(int go_staff = 0; go_staff < staff_count; go_staff++){
        staff_img_recogs[go_staff] -> get_note( note_infos.note, note_infos.note_count);
        note_infos.row_note_count_array[go_staff] =  staff_img_recogs[go_staff] -> get_note_count();
    }
}
Note_infos* Recognition_page::get_note_infos(){ return &note_infos; }
// Recognition_staff_img Recognition_page::get_staff_recog(int go_staff){
//     return staff_recogs[go_staff];
// }


void Recognition_page::run_recognition(){
    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        Recognition_staff_img staff_recog(go_staff, staff_img[go_staff], staff_img_erase_line[go_staff], trans_start_point_y[go_staff]);
        staff_recog.run();
        staff_recog.get_note(note, note_count);
        // staff_recogs[go_staff] = staff_recog;
        // UI_loading_recognition_one_staff(staff_recog);
    }
}

// 想要 辨識一行staff 就 UI直接顯示, 才不會要等整張譜的staff都辨識完再顯示等太久讓user覺得無聊
Recognition_staff_img* Recognition_page::run_one_staff_recognition(int staff_index){
    staff_img_recogs[staff_index] = new Recognition_staff_img(staff_index, staff_img[staff_index], staff_img_erase_line[staff_index], trans_start_point_y[staff_index]);
    staff_img_recogs[staff_index]->run();
    staff_img_recogs[staff_index]->get_note(note, note_count);  // 把 各別staff 辨識完的 note 放回 整張譜的note容器內
    return staff_img_recogs[staff_index];
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Recognition_staff_img::get_go_staff() {return go_staff; }
Mat Recognition_staff_img::get_staff_img(){ return staff_img; }
int Recognition_staff_img::get_note_count(){ return note_count; }

Recognition_staff_img::Recognition_staff_img(int in_go_staff, Mat in_staff_img, Mat in_staff_img_erase_line, int in_pitch_base_y, bool in_debuging):
    go_staff              (in_go_staff),
    staff_img             (in_staff_img),
    staff_img_erase_line  (in_staff_img_erase_line),
    pitch_base_y          (in_pitch_base_y),
    debuging              (in_debuging)
    {

    debuging_recog0  = false;
    debuging_recog1  = false;
    debuging_recog2  = false;
    debuging_recog3a = false;
    debuging_recog3b = false;
    debuging_recog4  = false;
    debuging_recog5  = false;
    e_count = 0;
    for(int i = 0 ; i < 200 ; i++){
        l_edge       [i] = 0;
        r_edge       [i] = 0;
        distance     [i] = 0;
        mountain_area[i] = 0;
    }

    note_count = 0;
    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j < 1000 ; j++)
            note[i][j] = 0;

    // 自己設資料結構 head, 0是左上角x, 1是左上角y, 2是similarity, 200是我目前還沒看過一組五線譜裡面有超過200顆音的所以隨便設一個大大的數字200
    maybe_head_count = 0;
    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            maybe_head[i][j] = 0;

    // 自己設資料結構 line, 200是我目前還沒看過一組五線譜裡面有超過200顆音的所以隨便設一個大大的數字200
    bars_count = 0;
    for(int i = 0 ; i < 3 ; i++)
        for(int j = 0 ; j < 200 ; j++)
            bars[i][j] = 0;

    for(int i = 0 ; i < 200 ; i++) bars_dir [i] = false;

    for(int i = 0 ; i < 200 ; i++) bars_time[i] = 0;

}


void Recognition_staff_img::run(){
    // recognition0 對 一組 刪除五線譜線後的staff影像 做垂直投影, 有音符的部分就會形成一座座黑色小山, 把小山的 左邊緣, 右邊緣, 山的寬度, 山的面積 找出來
    recognition_0_vertical_map_to_speed_up();

    // recognition0 根據垂直投影小山的結果, 把五線譜上的符號抓出來 放入 自己設計的 note資料結構
    // 全休止, 二分休止
    recognition_0_all_head(1);
    recognition_0_all_head(3);

    // 四分, 二分, 全音, 四分休止
    recognition_0_all_head(4);
    recognition_0_all_head(2);
    recognition_0_all_head(0);
    recognition_0_all_head(5);

    // // 八分休止, 十六分休止
    recognition_0_all_head(8);
    recognition_0_all_head(6);
    // // 三十二分休止
    recognition_0_all_head(7);
    
    // // 高音譜記號, 八分休止符的符桿
    recognition_0_all_head( 9);
    recognition_0_all_head(10);

    // 排序
    bubbleSort_note(note_count, note, Y_INDEX);
    bubbleSort_note(note_count, note, X_INDEX);

    // 看音高
    recognition_5_find_pitch(Mat(15, 15, CV_8UC1));

        
}


void Recognition_staff_img::get_note(int in_note[][1000], int& in_note_count){
    // 把row為單位的note 存進去所有的note的array
    for(int go_note = 0 ; go_note < note_count ; go_note++){
        in_note[0][go_note + in_note_count] = note[0][go_note];
        in_note[1][go_note + in_note_count] = note[1][go_note];
        in_note[2][go_note + in_note_count] = note[2][go_note];
        in_note[3][go_note + in_note_count] = note[3][go_note];
        in_note[4][go_note + in_note_count] = note[4][go_note];
    }

    in_note_count += note_count;
}




int Recognition(Mat ord_img, int& staff_count, Mat staff_img_erase_line[],Mat staff_img[],double trans_start_point_x[],double trans_start_point_y[],
                int& note_count , int note[][1000] , int row_note_count_array[],
                Mat UI_bass, string UI_WINDOW_NAME,
                string Title, Mat UI2_5,
                bool debuging){
    bool developing_debuging = false;
    Mat src_img = ord_img.clone();
    Mat src_bin = src_img.clone();

    // ************************* pre1 轉正 *************************
    try{
        double warp_angle = Find_Angle(src_img, developing_debuging);
        Wrap_Straight(src_img, warp_angle, developing_debuging);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -1;
    }
    
    // ************************* pre2 二值化 *************************
    try{
        src_bin = src_img.clone();
        Binary_by_patch(src_bin, 15, 40, developing_debuging);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -2;
    }
        
    
    
    // ************************* pre3 取影像正中間cols 後 水平投影 來抓五線譜線 *************************
    // 水平投影 找山 和 找線
    vector<Vec2f> lines;
    Mat src_bin_roi = ~src_bin;
    try{
        Center_ROI_by_slider(src_bin_roi, (string)ROI_DIR + "do_roi", developing_debuging);
        Mat horizontal_img(src_bin_roi.rows ,src_bin_roi.cols, CV_8UC1, Scalar(0));
        Horizon_map_to_find_line(src_bin_roi, lines, horizontal_img, developing_debuging);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -3;
    }
    
    // ************************* pre4 線的 距離階層 找出來 *************************
    // 把線的 距離階層 找出來, 以目前抓出來的階層有三種：
    // dist_level[0]= 3    一條粗線裡面可能有 找到多條細線 之間的距離
    // dist_level[1]= 17   五線譜內五條線大概的距離
    // dist_level[2]= 241  五線譜之間大概的距離
    int * dist_level;
    try{
        dist_level = Distance_detect(lines, developing_debuging);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -4;
    }
    
    // ************************* pre5 利用 dist_level 找出五線譜線 並 組成群組(把 lines 弄成 五條一組, 用5的倍數 來走訪各群, 所以只需return 最終五線譜組數即可) *************************
    try{
        staff_count = find_Staff_lines(lines, dist_level[0], dist_level[1], developing_debuging);
        if(developing_debuging){
            Watch_Hough_Line(lines, src_bin    , "",(string)"debug_img/" + "pre5_staff_line"    , 1066);
            Watch_Hough_Line(lines, src_bin_roi, "",(string)"debug_img/" + "pre5_staff_line_roi"      );
        }
        // 如果 找到0組 五線譜群, 就 return 失敗
        if(staff_count == 0){
            imshow(Title, UI2_5);
            waitKey(2000);
            return -5;
        }
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -5;
    }
    
    // ************************* pre6 每組五線譜群組 的線找頭 存入 left_point, right_point, 並在 順著線跑到頭的過程中 也生出一張 二值化把五線譜刪除的圖 *************************
    Mat color_src_img;  // debug用
    cvtColor(src_img, color_src_img, CV_GRAY2BGR);

    Mat src_bin_erase_line = src_bin.clone();
    
    int*** left_point ;  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    int*** right_point;  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    try{
        Find_Head_and_Erase_Line_Interface(src_bin, lines, staff_count, left_point, right_point, color_src_img, src_bin_erase_line, developing_debuging);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -6;
    }
    // ************************* 在 UI 上顯示 原始影像 -> 二值化影像 -> 畫出找完左右兩頭連成的線 *************************
    UI_loading_preprocess(src_img, src_bin, staff_count, left_point, right_point, UI_bass, UI_WINDOW_NAME, developing_debuging);


    // ************************* pre7 每組五線譜群組 找到的頭 的四個角 來透射切出每組五線譜  *************************
    try{
        Cut_staff(src_bin, src_bin_erase_line, staff_count, left_point, right_point,
                  staff_img_erase_line, staff_img,
                  trans_start_point_x, trans_start_point_y, false);
    }
    catch (exception e){
        imshow(Title, UI2_5);
        waitKey(2000);
        return -7;
    }
    // ********************************************************************************************************************************************************************
    // ********************************************************************************************************************************************************************
    // ********************************************************************************************************************************************************************
    // 走訪 每組五線譜 開始 辨識 每組五線譜
    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        // 主要用這些
        int e_count = 0;  // edge_count
        int l_edge       [200];
        int r_edge       [200];
        int distance     [200];
        int mountain_area[200];
        recognition_0_vertical_map_to_speed_up(staff_img_erase_line[go_staff],
                                               e_count, l_edge, r_edge, distance, mountain_area, 
                                               developing_debuging);

        // ~~~~~~~ 自己設的資料結構 用row為單位來存note~~~~~~~~~
        // note[0] = x , 
        // note[1] = y , 
        // note[2] = head_type , 
        // note[3] = time_bar , 
        // note[4] = 音高
        int row_note[5][1000]; 
        int row_note_count = 0;
        for(int i = 0 ; i < 5 ; i++)
            for(int j = 0 ; j < 1000 ; j++)
                row_note[i][j] = 0;

        // start_time = getTickCount();
        // end_time   = getTickCount() - start_time;
        // cout<<"maybe_head cost Time = "<<end_time<<endl;

        // head_type
        //   0:  1 全音符
        //   1:  1 全休止
        //   2:  2 分音符
        //   3:  2 休止
        //   4:  4 分音符
        //   5:  4 休止
        //   6: 16 休止
        //   7: 32 休止
        //   8:  8 休止
        //   9: 高音譜記號
        //  10: 八分休止符的符桿

        // 全休止, 二分休止
        recognition_0_all_head(1, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(3, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);

        // 四分, 二分, 全音, 四分休止
        recognition_0_all_head(4, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(2, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(0, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(5, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
    
        // 八分休止, 十六分休止
        recognition_0_all_head(8, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(6, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        // 三十二分休止
        recognition_0_all_head(7, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        
        // 高音譜記號, 八分休止符的符桿
        recognition_0_all_head( 9, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(10, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        
        // 排序
        bubbleSort_note(row_note_count, row_note, Y_INDEX);
        bubbleSort_note(row_note_count, row_note, X_INDEX);
        
        // 看音高
        recognition_5_find_pitch(staff_img[go_staff], Mat(15, 15, CV_8UC1), row_note_count, row_note, trans_start_point_y[go_staff], go_staff);

        // 把row為單位的note 存進去所有的note的array
        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            note[0][note_count+go_row_note] = row_note[0][go_row_note];
            note[1][note_count+go_row_note] = row_note[1][go_row_note];
            note[2][note_count+go_row_note] = row_note[2][go_row_note];
            note[3][note_count+go_row_note] = row_note[3][go_row_note];
            note[4][note_count+go_row_note] = row_note[4][go_row_note];
        }

        // 把row為單位的note 存進去所有的note的array
        note_count += row_note_count;
        row_note_count_array[go_staff] = row_note_count;

        // *****************************************************************
        // 看一下辨識結果
        if(developing_debuging){
            Mat debug_img2;
            cvtColor(staff_img[go_staff], debug_img2, CV_GRAY2BGR);
            list_row_note_info(            row_note_count, row_note);
            watch_row_note    (debug_img2, row_note_count, row_note);
        }
        // *****************************************************************
        // 在 UI 上顯示 辨識結果
        UI_loading_recognition_one_staff(staff_count, staff_img[go_staff], row_note_count, row_note, UI_bass, UI_WINDOW_NAME);
    }
    return 0;
}
