#include "Game_UI.h"
#include "Note_infos.h"
#include "string_tools.h"
#include "ScreenTool.h"

#define LOADING_BAR_SHIFT_X -80

Game::Game():
    loading_item_position{
    Point(LOADING_BAR_SHIFT_X +  200, 527), // 00 treble_clef
    Point(LOADING_BAR_SHIFT_X +  270, 539), // 01 8
    Point(LOADING_BAR_SHIFT_X +  344, 533), // 02 16
    Point(LOADING_BAR_SHIFT_X +  442, 564), // 03 rest_8
    Point(LOADING_BAR_SHIFT_X +  484, 531), // 04 4
    Point(LOADING_BAR_SHIFT_X +  564, 543), // 05 flat
    Point(LOADING_BAR_SHIFT_X +  613, 543), // 06 sharp
    Point(LOADING_BAR_SHIFT_X +  654, 544), // 07 8
    Point(LOADING_BAR_SHIFT_X +  733, 543), // 08 flat
    Point(LOADING_BAR_SHIFT_X +  784, 543), // 09 sharp
    Point(LOADING_BAR_SHIFT_X +  839, 570), // 10 rest_8
    Point(LOADING_BAR_SHIFT_X +  875, 538), // 11 4
    Point(LOADING_BAR_SHIFT_X +  940, 538), // 12 16
    Point(LOADING_BAR_SHIFT_X + 1011, 543), // 13 sharp
    Point(LOADING_BAR_SHIFT_X + 1057, 533), // 14 16
    Point(LOADING_BAR_SHIFT_X + 1120, 535), // 15 4
    Point(LOADING_BAR_SHIFT_X + 1209, 539)  // 16  finish
}{
    debuging = false;

    // 主視窗
    Title = "小小指揮家";
    // opencv 中文字 編碼只支援 cp950(Big5), 所以要先把 utf8 轉 cp950
    UI_WINDOW_NAME = utf8_to_cp950(Title);
    namedWindow(UI_WINDOW_NAME);
    cvMoveWindow(UI_WINDOW_NAME.c_str() , 0, 0);


    UI0             = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0.jpg"        , 1);  // 小小指揮家封面
    UI0_enter       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_enter.jpg"  , 1);  // 小小指揮家封面_press_enter
    UI0_1           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_1.jpg"      , 1);  // 歡迎來到指揮家樂園
    UI0_2           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_2.jpg"      , 1);  // 您可以任意挑選一張您喜愛的樂譜
    UI0_3           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_3.jpg"      , 1);  // 我們將會幫您看懂樂譜
    UI0_4           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_4.jpg"      , 1);  // 並且幫您演奏
    UI0_5           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_5.jpg"      , 1);  // 接著您可以用最直接的方式與音樂共舞
    UI0_6           = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_6.jpg"      , 1);  // 盡情地舞動您的雙手吧！
    UI0_6_enter     = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_6_enter.jpg", 1);  // 盡情地舞動您的雙手吧！_press_enter
    UI1             = imread("Resource\\UI_all_picture/UI PIC/UI/UI1.jpg"           ,1);  // 請挑選一個您喜歡的樂譜，並傳給我們吧!
    UI1_1           = imread("Resource\\UI_all_picture/UI PIC/UI/UI1_1.jpg"         ,1);  // 請挑選一個您喜歡的樂譜，並傳給我們吧!_press_enter
    UI1_2           = imread("Resource\\UI_all_picture/UI PIC/UI/UI1_2.jpg"         ,1);  // 我們當然需要一些時間視譜與練習

    UI2                         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/UI_bass2.bmp"   , 1);  //  進度條背景
    loading_bar_item_4          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_4.bmp"     , 0);  // 04, 11, 15
    loading_bar_item_8          = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_8.bmp"     , 0);  // 01, 07
    loading_bar_item_16         = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_16.bmp"    , 0);  // 02, 12, 14
    loading_bar_item_rest_8     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_rest_8.bmp", 0);  // 03, 10
    loading_bar_item_flat       = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_flat.bmp"  , 0);  // 05, 08
    loading_bar_item_sharp      = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_sharp.bmp" , 0);  // 06, 09, 13
    loading_bar_item_finish     = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp", 0);  // 16
    loading_bar_item_staff_line = imread("Resource/UI_all_picture/UI PIC/UI/loading_bar_item/note_finish.bmp", 0);  // 16
    loading_bar = 0;

    UI2_5           = imread("Resource\\UI_all_picture/UI PIC/UI/UI2_5.jpg",1);  // 很抱歉　您提供的樂譜我們無法辨識 請您重新拍攝樂譜

    UI3             = imread("Resource\\UI_all_picture/UI PIC/UI/UI3.jpg"            , 1);  // 即將進入指揮囉!(Loading...)
    UI3_enter       = imread("Resource\\UI_all_picture/UI PIC/UI/UI3_enter.jpg"      , 1);  // 即將進入指揮囉!(Enter)
    UI3_enter_press = imread("Resource\\UI_all_picture/UI PIC/UI/UI3_enter_press.jpg", 1);  // 即將進入指揮囉!(press Enter)

    UI4_0           = imread("Resource\\UI_all_picture/UI PIC/UI/UI4_0.jpg",1);  // 曲終. 您的指揮真是令我們驚艷！
    UI4_1           = imread("Resource\\UI_all_picture/UI PIC/UI/UI4_1.jpg",1);  // 感謝您參與這趟旅程，期待下一次相遇。
    UI5_0           = imread("Resource\\UI_all_picture/UI PIC/UI/UI5_0.jpg",1);  // 封底 參與人員
    UI5_1           = imread("Resource\\UI_all_picture/UI PIC/UI/UI5_1.jpg",1);  // ******* (沒用到) *******  封底 參與人員 enter


    width_frame_acc = 0;
}




void Game::run(){

    int NextStep=1;

    // opencv 中文字 編碼只支援 cp950(Big5), 所以要先把 utf8 轉 cp950
    Title = utf8_to_cp950(Title);
    namedWindow(Title);
    cvMoveWindow(Title.c_str() , 0, 0);

    // NextStep 1: 等待 user 輸入樂譜 用到的容器 在這邊宣告
    Mat SrcMusicSheet;

    while(true){
        switch(NextStep){
        // NextStep 0: 進入程式的前導動畫
        case 0:
            imshow(Title, UI0);
            waitKey(0);
            imshow(Title, UI0_enter);
            waitKey(200);
            FadeInOut(Title, UI0_enter, UI0_1,50);
            waitKey(2000);
            FadeInOut(Title, UI0_1, UI0_2,50);
            waitKey(2000);
            FadeInOut(Title, UI0_2, UI0_3,50);
            waitKey(800);
            FadeInOut(Title, UI0_3, UI0_4,50);
            waitKey(2000);
            FadeInOut(Title, UI0_4, UI0_5,50);
            waitKey(2000);
            FadeInOut(Title, UI0_5, UI0_6,50);
            //waitKey(0);
            imshow(Title, UI0_6_enter);
            waitKey(2000);
            NextStep=1;
            
            break;
        // NextStep 1: 等待 user 輸入樂譜
        case 1:{
            imshow(Title, UI1);
            waitKey(100);
            imshow(Title, UI1_1);
            waitKey(100);

            // debug 用, 直接指定想要的樂譜
            // SrcMusicSheet=imread("Resource/test_img/11-01.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/12-01.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/10349964_10207166022684817_1330043015452381144_n.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template15 - multi_head.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template14.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template15.jpg",0);
            SrcMusicSheet=imread("Resource/test_img/phone_clear_template17.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template17-full_note_test.jpg",0);
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template9.jpg",0);

            // // 上次的結果移動到finish
            // Move_JPG_Files("Resource/iPhone 5c (Global)/photo",
            //              "Resource/iPhone 5c (Global)/photo/finish");
            // cout<<"Initail Finish"<<endl;
            // cout<<"Please Upload your Picture"<<endl;

            // // 監聽 PicFolder 裡面有沒有 .jpg 傳進來, 沒有的話就卡在這個迴圈繼續監聽, 有傳進來的話就跳出迴圈進入下個step
            // vector<string> files;
            // while(true){
            //     files = Get_all_JPG_Files_in_folder(PicFolder);

            //     // 如果有檔案傳進來, 不管傳進來幾個, 只 讀取第一個.jpg檔案 
            //     if(files.size() > 0){
            //         SrcMusicSheet = imread(PicFolder+"\\" + files[0], 0);

            //         // 如果讀取成功
            //         if(!SrcMusicSheet.empty()){
            //             imwrite("debug_img/Upload_img_OK-write_a_log.jpg",SrcMusicSheet);
            //             cout<<"Music Sheet Upload Finish, go to NextStep"<<endl;
            //             break;
            //         }
            //         // 如果讀取失敗, 把這批圖片傳進 read_failed, 才不會一直繼續讀失敗圖耗資源
            //         else{
            //             Move_JPG_Files("Resource/iPhone 5c (Global)/photo",
            //                          "Resource/iPhone 5c (Global)/photo/read_failed");
            //             cout<<"Music Sheet Upload Failed, images are sent to read_failed dicrectory!"<<endl;
            //         }
            //     }
            // }

            NextStep=2;
            break;
        }
        // NextStep 2: 開始辨識(切割五線譜區域, 五線譜拉正, 辨識五線譜內音高)
        case 2:
            cout<<"Case 2"<<endl;
            imshow(Title, UI1_2);
            // 初始化容器 後 開始辨識
            try{
                // 開始辨識
                ////////////////////////////////////////////////////////////////////////////////////
                recog_result_ptr = new Recognition_page(SrcMusicSheet);
                Recognition_page& recog_result = *recog_result_ptr;
                recog_result.run_preprocess();
                UI_loading_preprocess();
                

                int staff_count = recog_result.get_staff_count();
                for(int staff_index = 0; staff_index < staff_count; staff_index++){
                    Recognition_staff_img * staff_recog = recog_result.run_one_staff_recognition(staff_index);
                    UI_loading_recognition_one_staff(staff_recog);
                }

                

            }
            catch (exception e){
                imshow(Title, UI2_5);
                waitKey(2000);
                NextStep=0;
                break;
            }
            imshow(Title, UI3);
            waitKey(2000);

            NextStep=3;
            break;

        // NextStep 3: 辨識完成後 開始畫音高 和 建立 MIDI音樂
        case 3:
            // cout<<"Case 3"<<endl;
            // imshow(Title, UI3);
            // GenerateMidiFile(note_infos, staff_img);
            // imshow(Title, UI3_enter);
            // waitKey(0);
            // imshow(Title, UI3_enter_press);
            // waitKey(200);
            // speed = 100;
            // ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Play Music

            NextStep=4;
            break;

        // NextStep 4: 播放MIDI音樂, 顯示畫面指揮畫面, 顯示樂譜音高
        case 4:
            //imshow(Title, UI5);
            // cout<<"Case 4"<<endl;
            // UI_Output=background.clone();

            // PlayMidiFile(note_infos);
            // NextStep=HandShaking(Title);
            // switch(NextStep){
            //     case 1:
            //         NextStep=0;
            //         MusicPlayback=false;
            //         break;
            //     case 3:

            //         NextStep=0;
            //         MusicPlayback=false;
            //         break;

            // }
            // imshow(Title, UI4_0);
            // waitKey(2000);
            // FadeInOut(Title, UI4_0, UI4_1,50);
            // waitKey(2000);

            // FadeInOut(Title, UI4_1, UI5_0,50);
            // waitKey(2000);
            break;

        }
    }

}



void Game::set_item_img(int position, Mat& setted_img){
    switch(position){
        case 4:
        case 11:
        case 15:
            setted_img = loading_bar_item_4.clone();
            break;
        case 1:
        case 7:
            setted_img = loading_bar_item_8.clone();
            break;
        case 2:
        case 12:
        case 14:
            setted_img = loading_bar_item_16.clone();
            break;
        case 3:
        case 10:
            setted_img = loading_bar_item_rest_8.clone();
            break;
        case 5:
        case 8:
            setted_img = loading_bar_item_flat.clone();
            break;
        case 6:
        case 9:
        case 13:
            setted_img = loading_bar_item_sharp.clone();
            break;
        case 16:
            setted_img = loading_bar_item_finish.clone();
            break;
    }
}

int Game::set_item_position(int in_num){
         if (in_num >=   0 && in_num <=  5) return  0;
    else if (in_num >=   6 && in_num <= 12) return  1;
    else if (in_num >=  13 && in_num <= 19) return  2;
    else if (in_num >=  20 && in_num <= 25) return  3;
    else if (in_num >=  26 && in_num <= 32) return  4;
    else if (in_num >=  33 && in_num <= 39) return  5;
    else if (in_num >=  40 && in_num <= 45) return  6;
    else if (in_num >=  46 && in_num <= 52) return  7;
    else if (in_num >=  53 && in_num <= 59) return  8;
    else if (in_num >=  60 && in_num <= 65) return  9;
    else if (in_num >=  66 && in_num <= 72) return 10;
    else if (in_num >=  73 && in_num <= 79) return 11;
    else if (in_num >=  80 && in_num <= 85) return 12;
    else if (in_num >=  86 && in_num <= 92) return 13;
    else if (in_num >=  93 && in_num <= 99) return 14;
    else if (in_num >= 100 && in_num <=100) return 15;
}

// 進度條
void Game::Show_loading_bar(int start_num, int end_num){
    Mat color_load_img;
    Mat gray_load_img;
    int color_load_img_left = 0;
    int color_load_img_top  = 0;

    start_num = set_item_position(start_num);
    end_num   = set_item_position(end_num);


    for(int go_item = start_num; go_item <= end_num; go_item++){
        set_item_img(go_item, gray_load_img);
        cvtColor(gray_load_img, color_load_img, CV_GRAY2BGR);
        // set_item_img(go_item, color_load_img);
        // cvtColor(color_load_img, gray_load_img, CV_BGR2GRAY);

        color_load_img_left = loading_item_position[go_item].x;
        color_load_img_top  = loading_item_position[go_item].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if  (gray_load_img.at<uchar>(go_row, go_col) == 255) continue;
                else{
                    UI_drawing.at<Vec3b>(go_row+color_load_img_top, go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row, go_col);
                }
            }
        }
        imshow(UI_WINDOW_NAME, UI_drawing);
        waitKey(100);
    }
    imshow(UI_WINDOW_NAME, UI_drawing);

    if(end_num == 15){
        set_item_img(16, gray_load_img);
        cvtColor(gray_load_img, color_load_img, CV_GRAY2BGR);
        // set_item_img(16, color_load_img);
        // cvtColor(color_load_img, gray_load_img, CV_BGR2GRAY);

        color_load_img_left = loading_item_position[16].x;
        color_load_img_top  = loading_item_position[16].y;
        for(int go_row = 0 ; go_row < color_load_img.rows ; go_row++){
            for(int go_col = 0 ; go_col < color_load_img.cols ; go_col++){
                if(gray_load_img.at<uchar>(go_row, go_col) == 255) continue;
                UI_drawing.at<Vec3b>(go_row+color_load_img_top, go_col+color_load_img_left) = color_load_img.at<Vec3b>(go_row, go_col);
            }
        }
        imshow(UI_WINDOW_NAME, UI_drawing);
        waitKey(1000);
    }
}


void Game::UI_loading_preprocess(){
    Mat src_img          = recog_result_ptr -> get_src_img();
    Mat bin_img          = recog_result_ptr -> get_src_bin();
    int staff_count      = recog_result_ptr -> get_staff_count();
    int*** left_point    = recog_result_ptr -> get_left_point();
    int*** right_point   = recog_result_ptr -> get_right_point();

    // 進度條預設0
    loading_bar = 0;  

    // 讀出 UI圖片
    UI_drawing = UI2.clone();  // 空的進度條圖片
    imshow(UI_WINDOW_NAME, UI_drawing);

    // 計算 UI畫面正中心x
    int UI_center_x = UI_drawing.cols / 2.;
    
    // src_img 縮放成 show_bin_windows 的 ratio
    int show_bin_window_height = 500;
    double resize_ratio = show_bin_window_height / (double)src_img.rows;
    // 縮放後的 width, height 計算
    int resize_height = show_bin_window_height;
    int resize_width  = src_img.cols * resize_ratio;

    // src_img, bin_img 做縮放 成 resize_ord_img, 
    Mat resize_ord_img;
    cv::resize(src_img, resize_ord_img, cv::Size(resize_width, resize_height));
    
    Mat resize_bin_img;
    cv::resize(bin_img, resize_bin_img, cv::Size(resize_width, resize_height));


    // 計算 縮放後的 src_img, bin_img 左邊界, 上邊界
    int resize_bin_left = UI_center_x - resize_width  / 2;

    // 定位出 resize_ord_img, reisze_bin_img 要顯示在 UI 的哪裡
    Mat show_pre_roi = UI_drawing( Rect(resize_bin_left, 0, resize_ord_img.cols, resize_ord_img.rows) );
    Mat show_pre_roi_back = show_pre_roi.clone();  // 先把這個roi區塊原始狀態存下來, 在結束時 可以把這個區塊 恢復原狀
    
    // 把 resize_ord_img 從 gray 轉成 RGB 後 貼在 定好的位置
    cvtColor(resize_ord_img, resize_ord_img, CV_GRAY2BGR);
    resize_ord_img.copyTo( show_pre_roi );
    imshow(UI_WINDOW_NAME, UI_drawing);
    waitKey(500);
    
    // 把 resize_bin_img 從 gray 轉成 RGB 後 貼在 定好的位置
    cvtColor(resize_bin_img, resize_bin_img, CV_GRAY2BGR);
    resize_bin_img.copyTo( show_pre_roi );
    imshow(UI_WINDOW_NAME, UI_drawing);
    waitKey(500);

    // 把 五線譜的頭 也做 縮放 和 平移 後 顯示出來
    for(int go_staff = 0 ; go_staff < staff_count ; go_staff++){
        for(int go_line = 0 ; go_line < 5 ; go_line++){
            double trans_left_x = left_point[go_staff][go_line][0] * resize_ratio;
            double trans_left_y = left_point[go_staff][go_line][1] * resize_ratio;
            trans_left_x += resize_bin_left;


            double trans_right_x = right_point[go_staff][go_line][0] * resize_ratio;
            double trans_right_y = right_point[go_staff][go_line][1] * resize_ratio;
            trans_right_x += resize_bin_left;

            line(UI_drawing, Point(trans_left_x , trans_left_y ), 
                          Point(trans_right_x, trans_right_y), 
                          Scalar(0, 0, 255), 1);

            imshow(UI_WINDOW_NAME, UI_drawing);
            waitKey(20);
        }
    }

    // 進度條 30%
    loading_bar +=30;
    Show_loading_bar( 0, loading_bar);
    // roi區塊恢復原始狀態
    show_pre_roi_back.copyTo(show_pre_roi);
}


// int width_frame_acc = 0;  // mod_width;

void Game::UI_loading_recognition_one_staff(Recognition_staff_img* staff_recog){
    int staff_count = recog_result_ptr->get_staff_count();

    int go_staff  = staff_recog->get_go_staff();
    Mat staff_img = staff_recog->get_staff_img();


    int row_note_count = 0;
    int row_note[5][1000];
    staff_recog->get_note(row_note, row_note_count);
    

    // 計算 UI畫面上方正白色區域中心
    int UI_center_x = UI_drawing.cols / 2.;
    int UI_center_y = 500 / 2.;
    
    // staff_img 轉成 RGB
    Mat staff_img_color;
    cvtColor(staff_img.clone(), staff_img_color, CV_GRAY2BGR);

    // 定位出 resize_ord_img, reisze_bin_img 要顯示在 UI 的哪裡
    int show_staff_top  = UI_center_y - staff_img_color.rows / 2;
    int show_staff_left = UI_center_x - staff_img_color.cols / 2;
    Mat show_staff_roi = UI_drawing( Rect(show_staff_left, show_staff_top, staff_img_color.cols, staff_img_color.rows) );
    staff_img_color.copyTo(show_staff_roi);
    imshow(UI_WINDOW_NAME, UI_drawing);

    // 標出辨識完的note
    Draw_note(UI_WINDOW_NAME, UI_drawing, row_note_count, row_note, show_staff_left, show_staff_top);

    // 計算 一組五線譜 相當於 進度條 幾%
    int div_width = 70 / staff_count;
    int mod_width = 70 % staff_count;
    



    // 上次的進度先存一份下來
    int old_loading_bar = loading_bar;
    // 更新進度
    loading_bar     += div_width;
    width_frame_acc += mod_width;
    if(width_frame_acc / staff_count){
        loading_bar++;
        width_frame_acc %= staff_count;
    }
    if(debuging) cout<<"old_loading_bar:" << old_loading_bar << ", loading_bar:" << loading_bar << ", width_frame_acc:" << width_frame_acc << ", div_width:" << div_width << ", mod_width:" << mod_width << endl;





    // // 上次的進度先存一份下來
    // int old_loading_bar = loading_bar;
    // // 更新進度
    // loading_bar         = (go_staff + 1) * div_width + loading_bar;
    // int width_frame_acc = (go_staff + 1) * mod_width;
    // loading_bar += width_frame_acc / staff_count;      
    // // if(width_frame_acc / staff_count){
    // //     loading_bar++;
    // //     width_frame_acc %= staff_count;
    // // }
    // if(debuging) cout<<"old_loading_bar:" << old_loading_bar << ", loading_bar:" << loading_bar << ", width_frame_acc:" << width_frame_acc << ", div_width:" << div_width << ", mod_width:" << mod_width << endl;

    // 更新一組五線譜組的進度條
    Show_loading_bar(old_loading_bar, loading_bar);
    waitKey(100);
}
