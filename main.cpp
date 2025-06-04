/*
這個程式的功能是：
整個系統的整合介面(動畫、拍照、辨識+轉midi+攝影機)
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "string_tools.h"
#include "UserEnterFile.h"
#include "Recognition.h"

#include "recognition_0_debug_tools.h"

#include "Generate_Play_Midi.h"
#include "ScreenTool.h"
#include "DirectoryTool.h"
///****************************************************
#include <cmath>
#include <windows.h>
#include <mmsystem.h>

///****************************************************

using namespace std;
using namespace cv;

///***********************************************

int NextStep=0;

int main(){
    // C:\\Users\\Dennis\\Desktop\\新增資料夾\\UIDesign\\UI PIC\\UI
    // C:\\Users\\Dennis\\Desktop\\新增資料夾\\UIDesign\\MusicSheetLocation\\

    Mat UI0         = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0.jpg"        , 1);
    Mat UI0_enter   = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_enter.jpg"  , 1);
    Mat UI0_1       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_1.jpg"      , 1);
    Mat UI0_2       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_2.jpg"      , 1);
    Mat UI0_3       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_3.jpg"      , 1);
    Mat UI0_4       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_4.jpg"      , 1);
    Mat UI0_5       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_5.jpg"      , 1);
    Mat UI0_6       = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_6.jpg"      , 1);
    Mat UI0_6_enter = imread("Resource\\UI_all_picture/UI PIC\\UI\\UI0_6_enter.jpg", 1);


    Mat UI1   = imread("Resource\\UI_all_picture/UI PIC/UI/UI1.jpg"  ,1);
    Mat UI1_1 = imread("Resource\\UI_all_picture/UI PIC/UI/UI1_1.jpg",1);
    Mat UI1_2 = imread("Resource\\UI_all_picture/UI PIC/UI/UI1_2.jpg",1);
    Mat UI2   = imread("Resource\\UI_all_picture/UI PIC/UI/UI2.jpg"  ,1);
    Mat UI2_5 = imread("Resource\\UI_all_picture/UI PIC/UI/UI2_5.jpg",1);
    Mat UI3   = imread("Resource\\UI_all_picture/UI PIC/UI/UI3.jpg"  ,1);
    Mat UI4_0 = imread("Resource\\UI_all_picture/UI PIC/UI/UI4_0.jpg",1);
    Mat UI4_1 = imread("Resource\\UI_all_picture/UI PIC/UI/UI4_1.jpg",1);
    Mat UI5_0 = imread("Resource\\UI_all_picture/UI PIC/UI/UI5_0.jpg",1);
    Mat UI5_1 = imread("Resource\\UI_all_picture/UI PIC/UI/UI5_1.jpg",1);

    // 檢查 讀UI圖
    if(  UI0.empty()||UI0_1.empty()||UI0_2.empty()||UI0_3.empty()||UI0_4.empty()||UI0_5.empty()||UI0_6.empty()\
       ||UI1.empty()||UI1_1.empty()||UI1_2.empty()||UI2.empty()||UI3.empty()||UI0_enter.empty()||UI0_6_enter.empty()){
        cout<<"checkUIPic Error!"<<endl;
        return 0;
    }
    if(background.empty()||bar.empty()){
        cout<<"Error Load Pic!"<<endl;
        return 0;
    }


    // opencv 中文字 編碼只支援 cp950(Big5), 所以要先把 utf8 轉 cp950
    Title = utf8_to_cp950(Title);
    namedWindow(Title);

    // NextStep 1: 等待 user 輸入樂譜 用到的容器 在這邊宣告
    Mat SrcMusicSheet;

    // NextStep 2: 開始辨識 中間 用到的容器 在這邊宣告
    // 小蝌蚪部分
    Note_infos* note_infos = new Note_infos();
    // 五線譜部分
    int staff_count;
    Mat final_rl_img_roi[40];
    Mat final_img_roi[40];
    double trans_start_point_x[40];
    double trans_start_point_y[40];

    
    while(true){
        switch(NextStep){
        // NextStep 0: 進入程式的前導動畫
        case 0:

            imshow(Title,UI0);
            waitKey(0);
            imshow(Title,UI0_enter);
            waitKey(200);
            FadeInOut(Title, UI0_enter,UI0_1,50);
            waitKey(2000);
            FadeInOut(Title, UI0_1,UI0_2,50);
            waitKey(2000);
            FadeInOut(Title, UI0_2,UI0_3,50);
            waitKey(800);
            FadeInOut(Title, UI0_3,UI0_4,50);
            waitKey(2000);
            FadeInOut(Title, UI0_4,UI0_5,50);
            waitKey(2000);
            FadeInOut(Title, UI0_5,UI0_6,50);
            //waitKey(0);
            imshow(Title,UI0_6_enter);
            waitKey(2000);

            NextStep=1;
            

            break;
        // NextStep 1: 等待 user 輸入樂譜
        case 1:{
            imshow(Title,UI1);
            waitKey(100);

            // // debug 用, 直接指定想要的樂譜
            // SrcMusicSheet=imread("Resource/test_img/phone_clear_template14.jpg",0);

            // 上次的結果移動到finish
            MoveJPGFiles("Resource/iPhone 5c (Global)/photo",
                         "Resource/iPhone 5c (Global)/photo/finish");
            cout<<"Initail Finish"<<endl;
            cout<<"Please Upload your Picture"<<endl;

            // 監聽 PicFolder 裡面有沒有 .jpg 傳進來, 沒有的話就卡在這個迴圈繼續監聽, 有傳進來的話就跳出迴圈進入下個step
            vector<string> files;
            while(true){
                files = get_all_jpg_files_in_folder(PicFolder);

                // 如果有檔案傳進來, 不管傳進來幾個, 只 讀取第一個.jpg檔案 
                if(files.size() > 0){
                    SrcMusicSheet = imread(PicFolder+"\\" + files[0], 0);

                    // 如果讀取成功
                if(!SrcMusicSheet.empty()){
                        imwrite("Upload_img_OK-write_a_log.jpg",SrcMusicSheet);
                        cout<<"Music Sheet Upload Finish, go to NextStep"<<endl;
                    break;
                }
                    // 如果讀取失敗, 把這批圖片傳進 read_failed, 才不會一直繼續讀失敗圖耗資源
                    else{
                        Move_JPG_Files("Resource/iPhone 5c (Global)/photo",
                                     "Resource/iPhone 5c (Global)/photo/read_failed");
                        cout<<"Music Sheet Upload Failed, images are sent to read_failed dicrectory!"<<endl;
                    }
                }
            }

            NextStep=2;
            break;
        }
        // NextStep 2: 開始辨識(切割五線譜區域, 五線譜拉正, 辨識五線譜內音高)
        case 2:
            cout<<"Case 2"<<endl;
            imshow(Title,UI1_2);
            // 初始化容器 後 開始辨識
            try{
                // 初始化 容器, 小蝌蚪部分
                note_infos -> note_count  = 0;
                note_infos -> go_note     = 0;
                note_infos -> go_row_note = 0;
                for(int i = 0; i < 5; i++ )
                    for(int j = 0; j < 5; j++ )
                        note_infos -> note[i][j] = 0;
                for(int i = 0; i < 40; i++ )
                    note_infos -> row_note_count_array[i] = 0;
                // 初始化 容器, 五線譜部分
                staff_count = 0;
                for(int i = 0 ; i < 40 ; i++){
                    trans_start_point_x[i] = 0;
                    trans_start_point_y[i] = 0;
                    note_infos -> row_note_count_array[i] = 0;
                }
                // 開始辨識
                Recognition(SrcMusicSheet,staff_count,final_rl_img_roi,final_img_roi,trans_start_point_x,trans_start_point_y,
                            note_infos -> note_count,note_infos -> note,note_infos -> row_note_count_array,
                            UI2,Title,
                            Title,UI2_5);
            }
            catch (exception e){
                imshow(Title,UI2_5);
                waitKey(2000);
                NextStep=0;
                break;
            }
            imshow(Title,UI3);
            waitKey(2000);

            NextStep=3;
            break;
        // NextStep 3: 辨識完成後 開始畫音高 和 建立 MIDI音樂
        case 3:
            cout<<"Case 3"<<endl;
            imshow(Title,UI3);
            waitKey(0);
            GenerateMidiFile(note_infos, final_img_roi);
            speed = 100;

            ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Play Music

            NextStep=4;
            break;
        // NextStep 4: 播放MIDI音樂, 顯示畫面指揮畫面, 顯示樂譜音高
        case 4:
            //imshow(Title,UI5);
            cout<<"Case 4"<<endl;
            Output=background.clone();

            PlayMidiFile(note_infos);
            NextStep=HandShaking(Title);
            switch(NextStep){
                case 1:
                    NextStep=0;
                    MusicPlayback=false;
                    break;
                case 3:

                    NextStep=0;
                    MusicPlayback=false;
                    break;

            }
            resize(UI4_0,UI4_0,Size(1600,900),0,0, INTER_CUBIC);
            resize(UI4_1,UI4_1,Size(1600,900),0,0, INTER_CUBIC);
            resize(UI5_0,UI5_0,Size(1600,900),0,0, INTER_CUBIC);
            imshow(Title,UI4_0);
            waitKey(2000);
            FadeInOut(Title, UI4_0,UI4_1,50);
            waitKey(2000);

            FadeInOut(Title, UI4_1,UI5_0,50);
            waitKey(2000);
            break;

        }
    }
    /*                              ///Animation
    for(int i = 0;i<=100;i++){
                rectangle( UI0, Point( i*5, 0 ), Point( (i*5)+5, 5), Scalar( 0, 55, 255 ), CV_FILLED, 4 );
                imshow("Title",UI0);
                waitKey(100);
            }

    */
}
