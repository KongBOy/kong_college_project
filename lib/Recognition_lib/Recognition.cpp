/*
這個程式的功能是：
整個 辨識 的整合介面
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>



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
int Recognization();


static string IntToString(int num)
{
    stringstream ss;
    ss<<num;

    string temp_string;
    ss>>temp_string;
    return temp_string;
}
int Recognition(Mat ord_img,int& staff_count, Mat staff_img_erase_line[],Mat staff_img[],double trans_start_point_x[],double trans_start_point_y[],
                int& note_count , int note[][1000] , int row_note_count_array[],
                Mat UI_bass,string UI_WINDOW_NAME,
                string Title,Mat UI2_5,
                bool debuging)
{
    bool developing_debuging = false;
    Mat src_img = ord_img.clone();
    Mat src_bin = src_img.clone();

    // ************************* pre1 轉正 *************************
    try{
        double warp_angle = Find_Angle(src_img, developing_debuging);
        Wrap_Straight(src_img, warp_angle, developing_debuging);
    }
    catch (exception e){
        imshow(Title,UI2_5);
        waitKey(2000);
        // NextStep=0;
        return -1;
    }
    
    // ************************* pre2 二值化 *************************
    src_bin = src_img.clone();
    Binary_by_patch(src_bin, 15, 40);
    // test_Binary_by_Canny(src_img);
    
    
    
    // ************************* pre3 取影像正中間 *************************
	string file_name;
    file_name = (string)"test_bin_";
    Mat src_bin_roi = ~src_bin;
    Center_ROI_by_slider(src_bin_roi, (string)ROI_DIR + "do_roi", developing_debuging);
    // imshow("test_roi",src_bin_roi);
    // waitKey(0);
    
    // 水平投影 找山 和 找線
    vector<Vec2f> lines;
	vector<Vec4i> lines_p;
    Mat horizontal_img(src_bin_roi.rows ,src_bin_roi.cols, CV_8UC1, Scalar(0));
    Horizon_map_to_find_line(src_bin_roi, lines, horizontal_img, developing_debuging);
    
    // ************************* pre4 線的 距離階層 找出來 *************************
    // 把線的 距離階層 找出來, 以目前抓出來的階層有三種：
    // dist_level[0]= 3    一條粗線裡面可能有 找到多條細線 之間的距離
    // dist_level[1]= 17   五線譜內五條線大概的距離
    // dist_level[2]= 241  五線譜之間大概的距離
    int * dist_level;
    
    dist_level = Distance_detect(lines, developing_debuging);
    // cout<<"distance_detect end"<<endl;


    // ************************* pre5 利用 dist_level 找出五線譜線組成群組 *************************
    staff_count = find_Staff2(lines, dist_level[0], dist_level[1], developing_debuging);
    if(developing_debuging){
        Watch_Hough_Line(lines, src_bin    , "",(string)"debug_img/" + "pre5_staff_line"    , 1066);
        Watch_Hough_Line(lines, src_bin_roi, "",(string)"debug_img/" + "pre5_staff_line_roi"      );
    }
    // waitKey(0);
    cout<<"find_staff_sucess"<<endl;




    // ************************* pre6 每組五線譜群組 的線找頭 *************************
    Mat color_src_img;  // debug用
    Mat src_bin_erase_line = src_bin.clone();
    cvtColor(src_img, color_src_img, CV_GRAY2BGR);

    int*** left_point ;  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    int*** right_point;  // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
    try{
        Find_Head_and_Erase_Line_Interface(src_bin, lines, staff_count, left_point, right_point, color_src_img, src_bin_erase_line, developing_debuging);
        cout<<"find_head_end~"<<endl;
    }
    catch (exception e){
        imshow(Title,UI2_5);
        waitKey(2000);
        return -2;
        // NextStep=0;
        // break;
    }
    
    // ************************* 在 UI 上顯示 *************************
    UI_loading_preprocess(src_img, src_bin, staff_count, left_point, right_point, UI_bass, UI_WINDOW_NAME, developing_debuging);


    // ************************* pre7 每組五線譜群組 找到的頭 的四個角 來透射切出每組五線譜  *************************
    // Mat staff_img_erase_line[40]; 寫出去主程式用參數傳
    // Mat staff_img   [40]; 寫出去主程式用參數傳
    // double trans_start_point_x[40]; 寫出去主程式用參數傳
    // double trans_start_point_y[40]; 寫出去主程式用參數傳
    try{
        Cut_staff(src_bin, src_bin_erase_line, staff_count, left_point, right_point,
                staff_img_erase_line, staff_img,
                trans_start_point_x, trans_start_point_y);
    }
    catch (exception e){
        imshow(Title,UI2_5);
        waitKey(2000);
        // NextStep=0;
        // break;
        return -3;
    }
    // cout<<"cut_staff_end~~"<<endl;
    // ****************************************************************************************************
    // ****************************************************************************************************
    // ****************************************************************************************************
    //  寫出去主程式用參數傳
    // 自己設的資料結構 note ~~~~~~~ 存整張譜所有的row的note~~~~~~~~~
    /*
    int note[5][1000]; /// 0 = x , 1 = y , 2 = type , 3 = time_bar , 4 = 音高
    int note_count = 0;
    for(int i = 0 ; i < 5 ; i++)
        for(int j = 0 ; j < 1000 ; j++)
            note[i][j] = 0;
    int row_note_count_array[40];
    for(int i = 0 ; i < 40 ; i++) row_note_count_array[i] = 0;
    */


    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        // 主要用這些
        int e_count = 0;  // edge_count
        int l_edge       [200];
        int r_edge       [200];
        int distance     [200];
        int mountain_area[200];
        int* note_type;
        recognition_0_vertical_map_to_speed_up(staff_img_erase_line[go_staff],
                                               e_count, l_edge, r_edge, distance, mountain_area,
                                               note_type);

        ///~~~~~~~ 自己設的資料結構 用row為單位來存note~~~~~~~~~
        int row_note[5][1000]; /// 0 = x , 1 = y , 2 = type , 3 = time_bar , 4 = 音高
        int row_note_count = 0;
        for(int i = 0 ; i < 5 ; i++)
            for(int j = 0 ; j < 1000 ; j++)
                row_note[i][j] = 0;

        /// 0  1 全音符
        /// 1  1 全休止
        /// 2  2 分音符
        /// 3  2 休止
        /// 4  4 分音符
        /// 5  4 休止
        /// 6 16 休止
        /// 7 32 休止
        /// 8  8 休止

        ///if(go_staff != 7) continue;

        Mat template_img_4 = imread("Resource/note/4/4.bmp",0);
        Mat template_img_4_rest = imread("Resource/note/4-rest/4-rest.bmp",0);
        Mat template_img_2 = imread("Resource/note/2/2.bmp",0);
        Mat template_img_0 = imread("Resource/note/0/0.bmp",0);
        Mat template_img_1 = imread("Resource/note/0-rest/0-rest.bmp",0);
        Mat template_img_3 = imread("Resource/note/2-rest/2-rest.bmp",0);
        Mat template_img_8 = imread("Resource/note/8-rest/8-rest-3.bmp",0);
        Mat template_img_6 = imread("Resource/note/6-rest/6-rest-2.bmp",0);

        Mat template_img_9 = imread("Resource/note/9/9.bmp",0);

        // recognition_0_all_head(2,staff_img_erase_line[go_staff],staff_img[go_staff],e_count,l_edge,distance,trans_start_point_y[go_staff],row_note_count,row_note);

        // start_time = getTickCount();
        // end_time   = getTickCount() - start_time;
        // cout<<"maybe_head cost Time = "<<end_time<<endl;
        
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



        bubbleSort_note(row_note_count, row_note, Y_INDEX);
        bubbleSort_note(row_note_count, row_note, X_INDEX);

        recognition_5_find_pitch(staff_img[go_staff], Mat(15, 15, CV_8UC1), row_note_count, row_note, trans_start_point_y[go_staff], go_staff);


        // 高音譜記號
        recognition_0_all_head( 9, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);
        recognition_0_all_head(10, staff_img_erase_line[go_staff], staff_img[go_staff], e_count, l_edge, distance, trans_start_point_y[go_staff], row_note_count, row_note, developing_debuging);

        // midi(row_note_count,row_note);



        // 把row為單位的note 存進去所有的note的array~~~
        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            note[0][note_count+go_row_note] = row_note[0][go_row_note];
            note[1][note_count+go_row_note] = row_note[1][go_row_note];
            note[2][note_count+go_row_note] = row_note[2][go_row_note];
            note[3][note_count+go_row_note] = row_note[3][go_row_note];
            note[4][note_count+go_row_note] = row_note[4][go_row_note];
        }

        // 把row為單位的note 存進去所有的note的array~~~
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
    }

    //UI_loading_recognition(staff_count,staff_img,note_count,note,row_note_count_array);


    // ******************************************************************************
    // midi(note_count,note,row_note_count_array,staff_img);


    // waitKey(0);

    // return 0;
}
