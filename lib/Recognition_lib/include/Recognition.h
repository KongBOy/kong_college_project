#pragma once

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#include "Note_infos.h"


class Recognition_staff_img{
    private:
        bool debuging;
        bool debuging_recog0;
        bool debuging_recog1;
        bool debuging_recog2;
        bool debuging_recog3a;
        bool debuging_recog3b;
        bool debuging_recog4;
        bool debuging_recog5;

        // preprocess 來的
        int go_staff;
        Mat staff_img;
        Mat staff_img_erase_line;
        int pitch_base_y;

        // recognition0 對 一組 刪除五線譜線後的staff影像 做垂直投影, 有音符的部分就會形成一座座黑色小山, 把小山的 左邊緣, 右邊緣, 山的寬度, 山的面積 找出來
        int e_count           ;  // edge_count
        int l_edge       [200];
        int r_edge       [200];
        int distance     [200];
        int mountain_area[200];


        // recognition0 根據垂直投影小山的結果, 把五線譜上的符號抓出來 放入 自己設計的 note資料結構
        // note[0] = 左上角x, 
        // note[1] = 左上角y, 
        // note[2] = head_type, 
        // note[3] = time_bar,
        // note[4] = 音高
        int note[5][1000];
        int note_count;

        // recognition1 
        // 自己設資料結構 head, 0是左上角x, 1是左上角y, 2是similarity, 200是我目前還沒看過一組五線譜裡面有超過200顆音的所以隨便設一個大大的數字200
        int maybe_head_count;
        float maybe_head[3][200];

        // 自己設資料結構 line, 200是我目前還沒看過一組五線譜裡面有超過200顆音的所以隨便設一個大大的數字200
        int   bars_count        ;
        short bars    [3][200];  //[0]頂點x [1]頂點y [2]長度
        bool  bars_dir   [200];  //[0]左(下, TOPTODOWN) [1]右(上, DOWNTOTOP), 需要方向的原因是需要找 8, 16, 32, 64, ... 分音符的橫線, 所以 標記往哪個方向走 來找 會輕鬆很多
        int   bars_time  [200];

    public:
        Recognition_staff_img(int in_go_staff, Mat in_staff_img, Mat in_staff_img_erase_line, int pitch_base_y, bool in_debuging=true);
        void run();
        // recognition0 對 一組 刪除五線譜線後的staff影像 做垂直投影, 有音符的部分就會形成一座座黑色小山, 把小山的 左邊緣, 右邊緣, 山的寬度, 山的面積 找出來
        void recognition_0_vertical_map_to_speed_up();

        // recognition0 把五線譜上的符號抓出來 放入 自己設計的 note資料結構
        void recognition_0_all_head( int head_type);

        // recognition1 
        void recognition_1_find_all_MaybeHead(Mat template_img, Mat target_img, Mat& staff_result_map, string method);
        // recognition1 
        void Grab_MaybeHead_from_ResultMap   (Mat template_img, Mat target_img, Mat staff_result_map, float thresh_hold=0.38);

        // recognition2 a
        void recognition_2_a_head_charactristic(int head_type, Mat template_img);
        // recognition2 b
        void recognition_2_b_head_recheck(int head_type, Mat MaybeHead_final_template);
        // recognition3 a
        void recognition_3_a_find_vertical_bar(Mat template_img);
        // recognition3 b
        void recognition_3_b_find_time_bar(Mat template_img);
        // recognition4
        void recognition_4_merge_head_and_time(int head_type,  Mat template_img);
        // recognition5
        void recognition_5_find_pitch(Mat template_img);

        // get_note
        void get_note(int in_note[][1000], int& in_note_count);
        int  get_go_staff();
        Mat  get_staff_img();
        int  get_note_count();
};

class Recognition_page{
    private:
        Mat ord_img;
        bool debuging;
        bool debuging_pre1;
        bool debuging_pre2;
        bool debuging_pre3a;
        bool debuging_pre3b;
        bool debuging_pre4;
        bool debuging_pre5;
        bool debuging_pre6;
        bool debuging_pre7;

        // recognition0 根據垂直投影小山的結果, 把五線譜上的符號抓出來 放入 自己設計的 note資料結構
        // note[0] = 左上角x, 
        // note[1] = 左上角y, 
        // note[2] = head_type, 
        // note[3] = time_bar,
        // note[4] = 音高
        int note[5][1000];  // 所有五線譜的note最後收集到這裡
        int note_count;     // 所有五線譜的note最後收集到這裡 總共有幾顆
        int note_count_every_staff[40];  // 每一組五線譜裡面的note有幾顆

        // preprocess1:
        //    ord_img 中心切小塊 找線的角度(warp_angle)
        //    ord_img 根據 warp_angle 轉正( warp_angle - 90 ) 放 src_img
        double warp_angle;
        Mat src_img;
        
        // preprocess2: 影像二值化(切小patch)
        Mat src_bin;
        
        // preprocess3:
        //     取影像正中間 切 cols
        int roi_c_slider;
        int roi_r_slider;
        Mat slider_result_img;  // 給 slider用的 暫存容器
        Mat src_bin_roi;        // slider 調整後的 影像存出來 給接下來的流程用
        // 水平投影 找山 和 找線
        vector<Vec2f> lines;

        // preprocess4 找線之間距離的層級
        int * dist_level;
        
        // preprocess5 利用 dist_level 找出五線譜線 並 組成群組(把 lines 弄成 五條一組, 用5的倍數 來走訪各群, 所以只需return 最終五線譜組數即可)
        int staff_count;

        // preprocess6 每組五線譜群組 的線找頭 存入 left_point, right_point, 並在 順著線跑到頭的過程中 也生出一張 二值化把五線譜刪除的圖
        int*** left_point ;      // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
        int*** right_point;      // [長度==staff_count, 第幾組五線譜][長度==5, 五線譜的第幾條線][長度==2, 0是x, 1是y]
        Mat src_bin_erase_line;  
        
        // preprocess7 每組五線譜群組 找到的頭 的四個角 來透射切出每組五線譜
        Mat staff_img_erase_line[40];
        Mat staff_img[40];
        double trans_start_point_x[40];
        double trans_start_point_y[40];
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Recognition_staff_img staff_recogs[40];
        Recognition_staff_img** staff_img_recogs;
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 把 各staff 辨識結果取出來 方便 後續處理 的容器
        Note_infos note_infos;
        
    public:
        Recognition_page(const Mat input_img, bool in_debuging=false);
        Mat get_src_img();
        Mat get_src_bin();
        int  get_staff_count();
        Mat* get_staff_img();
        Mat* get_staff_img_erase_line();
        int*** get_left_point();
        int*** get_right_point();
        Recognition_staff_img get_staff_recog(int go_staff);

        //////////////////////////////////////////////////////////////////////////////
        // 把 各staff 辨識結果取出來 方便 後續處理
        void set_note_infos_from_staff_imgs();
        Note_infos* get_note_infos();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int run_preprocess();

        // preprocess1
        void Find_Angle();     // ord_img 中心切小塊 找線的角度(warp_angle)
        void Wrap_Straight();  // ord_img 根據 warp_angle 轉正( warp_angle - 90 ) 放 src_img
        
        // preprocess2 影像二值化(切小patch)
        unsigned char Binary(Mat& dst);
        void Binary_by_patch(Mat& dst, const int div_row, const int div_col);

        // preprocess3 a, static 是為了 opencv slider 丟 call_back 時 的 function 必須要是一般function不可以連同this一起丟入, 所以用 static 就變成純粹function 就可以當call_back去囉
        static void Center_ROI(int, void * data);
        void Center_ROI_by_slider(string window_name);
        // preprocess3 b, 把 轉正 和 二值化後的影像 裡面的白點 全部移到左邊形成直條圖 會形成像山一樣的形狀, 找出 由五線譜形成的山 並 再 該區間的影像找出五線譜的線
        void Horizon_map_to_find_line();

        // preprocess4 找線之間距離的層級
        int * Distance_detect();

        // preprocess5 利用 dist_level 找出五線譜線 並 組成群組(把 lines 弄成 五條一組, 用5的倍數 來走訪各群)
        void filter_multi_same_line();
        int find_Staff_lines();
        
        // preprocess6 每組五線譜群組 的線找頭 存入 left_point, right_point, 並在 順著線跑到頭的過程中 也生出一張 二值化把五線譜刪除的圖
        void Find_Head_and_Erase_Line_Interface();
        
        // preprocess7 每組五線譜群組 找到的頭 的四個角 來透射切出每組五線譜
        void Cut_staff();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void run_recognition();
        Recognition_staff_img* run_one_staff_recognition(int staff_index);
};






int Recognition(Mat ord_img,int& staff_count, Mat staff_img_erase_line[],Mat staff_img[],double trans_start_point_x[],double trans_start_point_y[],
                int& note_count , int note[][1000] , int row_note_count_array[],
                Mat UI_bass,string UI_WINDOW_NAME,
                string Title,Mat UI2_5,
                bool debuging=false);
