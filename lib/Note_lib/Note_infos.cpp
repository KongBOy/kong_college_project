/*
這個程式的功能是：
可以把找到的note資訊顯示出來.
    list：列出note資訊
    watch：把note 顯示 在輸入圖上 (不改變輸入圖)
    draw ：把note  畫  在輸入圖上 (會改變輸入圖)
*/

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Note_infos.h"

using namespace std;
using namespace cv;


void get_note_color_and_img(int head_type,int time_bar,Scalar & color , Mat & template_img)
{
    Mat template_img_4 = imread("Resource/note/4/4.bmp",0);
    Mat template_img_5 = imread("Resource/note/4-rest/4-rest.bmp",0);
    Mat template_img_2 = imread("Resource/note/2/2.bmp",0);
    Mat template_img_0 = imread("Resource/note/0/0.bmp",0);
    Mat template_img_1 = imread("Resource/note/0-rest/0_rest_w_line.bmp",0);
    Mat template_img_3 = imread("Resource/note/2-rest/2_rest_w_line.bmp",0);
    Mat template_img_8 = imread("Resource/note/8-rest/8-rest-3.bmp",0);
    Mat template_img_6 = imread("Resource/note/6-rest/6-rest-2.bmp",0);
    Mat template_img_7 = imread("Resource/note/32-rest/7-1-up15w-down15w.bmp",0);

    Mat template_img_9 = imread("Resource/note/9/9-bin.bmp",0);
    Mat template_img_10 = imread("Resource/note/10/10-1.bmp",0);

    switch(head_type){
        // 全音符
        case 0:{
            color = Scalar(140,100,100);  // 淺紫色 
            template_img = template_img_0.clone();
        }
        break;
        
        // 全休止
        case 1:{
            color = Scalar(180,100,100);  // 淺紫色 
            template_img = template_img_1.clone();
        }
        break;

        // 二分音符
        case 2:{
            color = Scalar(220,100,100);  // 淺紫色
            template_img = template_img_2.clone();
        }
        break;

        // 二分休止
        case 3:{
            color = Scalar(255,120,100);  // 淺紫色
            template_img = template_img_3.clone();
        }
        break;
        
        // 四分音符, 八分音符, 十六分音符, 三十二分音符
        case 4:{
            color = Scalar(100 , 100 , (100 + time_bar*70) % 256);  // 四分音符 深灰, 八分音符 淺咖啡, 十六分音符 淡粉紅, 三十二分 亮淡粉紅
            template_img = template_img_4.clone();
        }
        break;

        // 四分休止符
        case 5:{
            color = Scalar(255,200,100);  // 淺藍
            template_img = template_img_5.clone();
        }
        break;

        // 十六分休止符
        case 6:{
            color = Scalar(255,255,100);  // 青藍
            template_img = template_img_6.clone();
        }
        break;

        // 三十二分休止符
        case 7:{
            color = Scalar(140,100,180);  // 淡紫
            template_img = template_img_7.clone();
        }
        break;

        // 八分休止符
        case 8:{
            color = Scalar(140,180,180);  // 亮綠灰
            template_img = template_img_8.clone();
        }
        break;

        // 高音譜記號, 因為 二分音符 容易跟這個搞混, 所以需要額外 把裡面誤判的二分音符頭 刪除
        case 9:{
            color = Scalar(140,255,180);  // 淡亮綠
            template_img = template_img_9.clone();
        }
        break;

        // 八分符桿, 因為 二分音符 容易跟這個搞混, 所以需要額外找 八分音符符桿 然後 把裡面誤判的二分音符頭 刪除
        case 10:{
            color = Scalar(100, 100, 170);  // 用八分音符 淺咖啡
            template_img = template_img_10.clone();
        }
        break;
    }
}

void list_row_note_info(int row_note_count,int row_note[][1000]){
    cout<<endl<<endl;
        cout<<"print row_note~~~"<<endl;
        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            cout<<"go_row_note = "<<go_row_note
                <<" , type = "<<row_note[2][go_row_note]
                <<" , x = "<<row_note[0][go_row_note]
                <<" , y = "<<row_note[1][go_row_note]
                <<" , time = "<<row_note[3][go_row_note]
                <<" , pitch = "<<row_note[4][go_row_note]
                <<endl;
        }
        cout<<"row_note_count = "<<row_note_count<<endl;

}
void watch_row_note(Mat debug_img , int row_note_count, int row_note[][1000]){
    for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
        Scalar color;
        Mat template_img;
        get_note_color_and_img(row_note[2][go_row_note],row_note[3][go_row_note],color,template_img);

        rectangle(debug_img,Point(row_note[0][go_row_note],row_note[1][go_row_note]),
                            Point(row_note[0][go_row_note]+template_img.cols,row_note[1][go_row_note]+template_img.rows),color,2);
        imshow("debug",debug_img);
        waitKey(0);
    }
}
void draw_row_note(Mat debug_img , int row_note_count, int row_note[][1000]){
    for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
        Scalar color;
        Mat template_img;
        get_note_color_and_img(row_note[2][go_row_note],row_note[3][go_row_note],color,template_img);
        rectangle(debug_img,Point(row_note[0][go_row_note],row_note[1][go_row_note]),
                            Point(row_note[0][go_row_note]+template_img.cols,row_note[1][go_row_note]+template_img.rows),color,2);
        // imshow("debug",debug_img);
        // waitKey(0);
    }
}




void list_note_info(int note_count,int note[][1000]){
    cout<<endl<<endl;
    cout<<"print note~~~"<<endl;
    for(int go_note = 0 ; go_note < note_count ; go_note++){
        cout<<"go_note = "<<go_note
            <<" , type = "<<note[2][go_note]
            <<" , x = "<<note[0][go_note]
            <<" , y = "<<note[1][go_note]
            <<" , time = "<<note[3][go_note]
            <<" , pitch = "<<note[4][go_note]
            <<endl;
    }
    cout<<"note_count = "<<note_count<<endl;
}

void watch_note(Mat debug_img , int note_count, int note[][1000]){
    for(int go_note = 0 ; go_note < note_count ; go_note++){
        Scalar color;
        Mat template_img;
        get_note_color_and_img(note[2][go_note],note[3][go_note],color,template_img);

        rectangle(debug_img,Point(note[0][go_note],note[1][go_note]),
                            Point(note[0][go_note]+template_img.cols,note[1][go_note]+template_img.rows),color,2);
        imshow("debug",debug_img);
        waitKey(0);
    }
}

void draw_note(Mat debug_img , int note_count, int note[][1000]){
    for(int go_note = 0 ; go_note < note_count ; go_note++){
        Scalar color;
        Mat template_img;
        get_note_color_and_img(note[2][go_note],note[3][go_note],color,template_img);

        rectangle(debug_img,Point(note[0][go_note],note[1][go_note]),
                            Point(note[0][go_note]+template_img.cols,note[1][go_note]+template_img.rows),color,2);
        // imshow("debug",debug_img);
        // waitKey(0);
    }

}



