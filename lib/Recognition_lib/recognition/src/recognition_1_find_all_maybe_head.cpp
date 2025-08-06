/*
這個程式的功能是：
找出 輸入符號 的精確位置，但是找到得符號還不確定是確的，只能說"可能是"
比對符號的方法是opencv的function
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "recognition_0_array_tools.h"
#include "recognition_0_debug_tools.h"
#include "Note_infos.h"
#include "recognition_1_find_all_maybe_head.h"


using namespace cv;
using namespace std;


static int start_time = 0;
static int end_time = 0;


void minMaxLoc2(Mat img,double& maxVal ,Point& maxLoc){
    maxVal = -10000;
    for(int go_row = 0 ; go_row < img.rows ; go_row++){
        for(int go_col = 0 ; go_col < img.cols ; go_col++){
            if(img.at<float>(go_row,go_col) > maxVal){
                maxVal = img.at<float>(go_row,go_col);
                maxLoc.x = go_col;
                maxLoc.y = go_row;
            }
        }
    }
}

/// 用來 de~~ range設錯的bug~~~~
void debug(Mat result_src, Mat src_bin_erase_line, Mat template_img, int x, int y, int width, int height){
    int note_w = width;
    int note_h = height;
    int note_l = x;
    int note_r = note_l + note_w;
    int note_t = y;
    int note_d = note_t + note_h;
    ///************ 防呆 ***************
    if( note_l < 0) note_l = 0;
    if( note_t < 0) note_t = 0;
    if( note_r > result_src.cols-1) note_r = result_src.cols -1;
    if( note_d > result_src.rows-1) note_d = result_src.rows -1;
    note_w = note_r - note_l;
    note_h = note_d - note_t;
    ///************ 防呆 ***************

    Mat result_src_color;
    cvtColor(result_src, result_src_color, CV_GRAY2BGR);
    rectangle(result_src_color, Point(note_l,note_t), Point(note_r,note_d), Scalar(0, 0, 255), 1, 8, 0);
    imshow("where_debuging", result_src_color);
    cout << result_src( Rect(note_l, note_t, note_w, note_h) )<<" "<<endl;


    double debug_minVal; double debug_maxVal; Point debug_minLoc; Point debug_maxLoc;
    Point debug_matchLoc;
    minMaxLoc( result_src( Rect(note_l, note_t, note_w, note_h) ) , &debug_minVal, &debug_maxVal, &debug_minLoc, &debug_maxLoc, Mat() );
    debug_maxLoc.x += note_l   ; // 因為上面是用ROI，所以要位移到正確的位置
    debug_maxLoc.y += note_t;

    Mat src_bin_erase_line_color;
    cvtColor(src_bin_erase_line, src_bin_erase_line_color, CV_GRAY2BGR);
    rectangle(src_bin_erase_line_color, debug_maxLoc, Point(debug_maxLoc.x + template_img.cols, debug_maxLoc.y + template_img.rows), Scalar(0, 255, 0), 1, 8, 0);
    imshow("where_is_the_max",src_bin_erase_line_color);
    cout<<"2.x = "<<debug_maxLoc.x<<" , y = "<<debug_maxLoc.y<<" , value = "<<debug_maxVal<<endl;
    waitKey(0);
}

/// 用來 de~~ range設錯的bug~~~~
void merge_head(Mat result_src,Mat reduce_line,Mat template_img){
    // imshow("merge_head",result_src);
    // waitKey(0);
    // debug(result_src,reduce_line,template_img,0,0,result_src.cols,result_src.rows);

    for(int go_row = 0 ; go_row < result_src.rows ; go_row++){
        for(int go_col = 0 ; go_col < result_src.cols ; go_col++){
            if(result_src.at<float>(go_row,go_col)){

                // 一、框result_src的框框，防呆變數設定START///
                int left  = go_col - 0.5*template_img.cols;
                int right = go_col + 0.5*template_img.cols;
                int top   = go_row - 0.5*template_img.rows;  // 0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~
                int down  = go_row + 0.5*template_img.rows;  // 0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~

                // 如果超出範圍，就設定為最大範圍
                if(top < 0) top = 0;
                if(left  < 0) left = 0;
                if(right > result_src.cols-1) right = result_src.cols-1;
                if(down  > result_src.rows-1) down = result_src.rows -1;

                int range_width = right - left;
                int range_height = down - top;
                // 框result_src的框框，防呆變數設定END///


                // 二、找出框框內最好的點(最像的地方)
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;

                minMaxLoc( result_src( Rect(left,top,range_width+1,range_height+1) ) , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                ///minMaxLoc2(result_src(Rect(left,top,range_width,range_height)),maxVal,maxLoc);
                maxLoc.x += left; ///因為上面是用ROI，所以要位移到正確的位置
                maxLoc.y += top;  ///因為上面是用ROI，所以要位移到正確的位置
                // imshow("before",result_src); ///合併前


                // 三、只留下那一點
                // 因為做threshold 清不乾淨所以乾脆直接自己寫for迴圈囉~~
                bool one_point = false;
                for(int i = top ; i <= down ; i++ )
                    for(int j = left ; j <= right ; j++)
                        result_src.at<float>(i,j) = 0;
                        ///if(result_src.at<float>(i,j) < maxVal) result_src.at<float>(i,j) = 0;
                result_src.at<float>(maxLoc.y,maxLoc.x) = maxVal;


                // imshow("after",result_src); ///合併後
                // cout<<"value = "<<maxVal<<endl;


                // ~~~~~ debug用 ~~~~~ 可以看我怎麼合併的過程

                Mat show_result = result_src.clone();
                cvtColor(result_src,show_result,CV_GRAY2BGR);
                rectangle(show_result,Point(left,top),Point(right,down),Scalar(0,255,0),1,8,0);
                cout<<"1-merge_center.x = "<<go_col<<" , y = "<<go_row<<" , maxVal = "<<maxVal<<endl;
                imshow("where we merge",show_result);

                // if(go_col <700 && go_col > 600 && go_row < 130 && go_row > 100)
                debug(result_src,reduce_line,template_img,10,0,20,30);

                Mat temp_show = reduce_line.clone();
                cvtColor(reduce_line,temp_show,CV_GRAY2BGR);
                for(int i = 0; i < result_src.rows ; i++)
                    for(int j = 0 ; j < result_src.cols ; j++)
                        if(result_src.at<float>(i,j) ) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
                imshow("merge_function",temp_show);
                waitKey(0);



                // 這可以把我找到的頭都存進去指定的資料夾裡面
                // imwrite("note/find_template/"+ IntToString(number) +".bmp",reduce_line( Rect(left,top,range_width,range_height) ));
                // number++;

                // destroyWindow("temp_show");

            }
        }
    }
}


void recognition_1_find_all_maybe_head(Mat template_img,Mat reduce_line,
                                       int e_count, int* l_edge, int* distance,
                                       int& maybe_head_count,float maybe_head[][200],
                                       int pitch_base_y){

    /// 一、整個大圖片的 result_src 容器
    /// 二、加速，看想看的小地方地方即可：根據垂直投影找出來的mountain切
    /// 三、對想看的小地方做template_match，結果存在小result
    /// 四、把 小結果result 加回去 大圖result_src

    ///一、Create the src_result matrix
    int result_src_row = reduce_line.rows - template_img.rows+1;
    int result_src_col = reduce_line.cols - template_img.cols+1;
    Mat result_src(result_src_row,result_src_col,CV_32FC1,Scalar(0));

    // cout<<"result_src_row = "<<result_src_row<<endl;
    // cout<<"result_src_col = "<<result_src_col<<endl;

    for(int go_mountain = 0 ; go_mountain < e_count ; go_mountain++){
        // 防呆呆呆 如果template的寬度比較大的話~~~ 就把要看的山的距離拉大點囉~~~
        int width_error = 0;
        while(distance[go_mountain] < template_img.cols && width_error < 20){
            if(l_edge[go_mountain] > 0 ) l_edge[go_mountain]--;
            if(l_edge[go_mountain]+distance[go_mountain] < reduce_line.cols-1)distance[go_mountain] += 2;
            width_error++;
            // cout註解 來看防呆寫得如何
            // cout<<"width_error = "<<width_error<<" , distance[go_mountain] = "<<distance[go_mountain]<<endl;
        }

        // 二、 Source_img，根據垂直投影找出來的mountain切
        Mat proc_img = reduce_line(Rect(l_edge[go_mountain],0, distance[go_mountain], reduce_line.rows ));

        // 三前置、 Create the proc_result matrix，根據垂直投影找出來的mountain切
        int result_row = reduce_line.rows - template_img.rows +1;
        int result_col = distance[go_mountain] - template_img.cols +1;
        Mat result(result_row,result_col,CV_32FC1);
        // cout註解
        // cout<<"result_row = "<<result_row<<endl;
        // cout<<"result_col = "<<result_col<<endl;

        // 三、 Do the Matching and select which part we might need by using threshold function
        matchTemplate(proc_img,template_img,result,CV_TM_CCOEFF_NORMED);
        // threshold(result, result, 0.5, 1., CV_THRESH_TOZERO);

        // 四、 加回 result_src
        result_src( Rect(l_edge[go_mountain],0,result_col, result_row) ) += result;

    }
    // cout註解
    // cout<<endl;

    // 五、簡單篩一下，取大量喔！就是找出可能是要找的頭的概念！之後再用特徵篩一次~~
    float thresh_hold = 0.40;
    // normalize( result_src, result_src, 0, 1, NORM_MINMAX, -1, Mat() ); ///如果用這個的話就連其他版本的譜好像也可以，thr設0.75
    threshold(result_src, result_src, thresh_hold , 1.0 , CV_THRESH_TOZERO);




    // ~~~~~ debug用 ~~~~~ 把所有找到的地方都框出來，還沒有合併附近很像的地方
    //  Localizing the best match with minMaxLoc
    Mat temp_show = reduce_line.clone();
    cvtColor(reduce_line,temp_show,CV_GRAY2BGR);
    for(int i = 0; i < result_src.rows ; i++)
        for(int j = 0 ; j < result_src.cols ; j++)
            if(result_src.at<float>(i,j) ) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
    // imshow("template_find_all",temp_show);


    // debug整合 ///
    // imshow("debug",temp_show);
    // waitKey(0);



    // *******************************************************************************************************
    // *******************************************************************************************************
    // *******************************************************************************************************
    // *******************************************************************************************************
    // *******************************************************************************************************
    // 把附近找出來一堆很像的的東西合併成一個：
    // 一、先框好範圍，
    // 二、找出框框內最好的點，
    // 三、只留下那個點其他點去掉，
    // 四、存起來
    cvtColor(reduce_line,temp_show,CV_GRAY2BGR);


    // ~~~~~~debug~~~~~
    //    merge_head(result_src(Rect(680,100,40,60)),reduce_line(Rect(680,100,40,60)),template_img);

    for(int go_row = 0 ; go_row < result_src.rows ; go_row++){
        ///cout<<"go_row = "<<go_row<<endl;
        for(int go_col = 0 ; go_col < result_src.cols ; go_col++){
            ///cout<<"go_col = "<<go_col<<endl;
            if(result_src.at<float>(go_row,go_col)){

                // 一、框result_src的框框，防呆變數設定START///
                int left  = go_col - 0.5*template_img.cols;
                int right = go_col + 0.5*template_img.cols;
                int top   = go_row - 0.5*template_img.rows;///0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~
                int down  = go_row + 0.5*template_img.rows;///0.3那個是因為音符不可能會兩顆重疊在一起，就算有我們也不辨識user自行解決~~

                ///如果超出範圍，就設定為最大範圍
                if(top < 0) top = 0;
                if(left  < 0) left = 0;
                if(right > result_src_col-1) right = result_src_col-1;
                if(down  > result_src_row-1) down = result_src_row -1;

                int range_width = right - left;
                int range_height = down - top;
                // 框result_src的框框，防呆變數設定END///


                // 二、找出框框內最好的點(最像的地方)
                double minVal; double maxVal; Point minLoc; Point maxLoc;
                Point matchLoc;

                minMaxLoc( result_src( Rect(left,top,range_width+1,range_height+1) ) , &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
                maxLoc.x += left; ///因為上面是用ROI，所以要位移到正確的位置
                maxLoc.y += top;  ///因為上面是用ROI，所以要位移到正確的位置
                // imshow("before",result_src); ///合併前


                // 三、只留下那一點
                // 因為做threshold 清不乾淨所以乾脆直接自己寫for迴圈囉~~
                bool one_point = false;
                for(int i = top ; i <= down ; i++ )
                    for(int j = left ; j <= right ; j++)
                        result_src.at<float>(i,j) = 0;
                        // if(result_src.at<float>(i,j) < maxVal) result_src.at<float>(i,j) = 0;
                result_src.at<float>(maxLoc.y,maxLoc.x) = maxVal;


                // imshow("after",result_src);  // 合併後
                // cout<<"value = "<<maxVal<<endl;


                /// ~~~~~ debug用 ~~~~~ 可以看我怎麼合併的過程
                /*
                Mat show_result = result_src.clone();
                cvtColor(result_src,show_result,CV_GRAY2BGR);
                rectangle(show_result,Point(left,top),Point(right,down),Scalar(0,255,0),1,8,0);
                cout<<"x = "<<go_col<<" , y = "<<go_row<<" , maxVal = "<<maxVal<<endl;
                imshow("where we merge",show_result);

                debug(result_src,reduce_line,template_img,680,100,25,20);

                cvtColor(reduce_line,temp_show,CV_GRAY2BGR);
                for(int i = 0; i < result_src.rows ; i++)
                    for(int j = 0 ; j < result_src.cols ; j++)
                        if(result_src.at<float>(i,j) ) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
                imshow("template_find_all",temp_show);
                waitKey(0);
                */


                // 這可以把我找到的頭都存進去指定的資料夾裡面
                // imwrite("note/find_template/"+ IntToString(number) +".bmp",reduce_line( Rect(left,top,range_width,range_height) ));
                // number++;

                ///destroyWindow("temp_show");

            }
        }
    }

// 自己設資料結構 移到外面去囉
//    int maybe_head_count = 0;
//    float maybe_head[3][200];
//    for(int i = 0 ; i < 3 ; i++)
//        for(int j = 0 ; j < 200 ; j++)
//            maybe_head[i][j] = 0;


// 四、把可能是頭的點存進我的data structure，改寫from SHOW START 沒有用我的資料結構///
    for(int i = 0; i < result_src.rows ; i++){
        for(int j = 0 ; j < result_src.cols ; j++){
            if( (result_src.at<float>(i,j) ) &&
                (i - pitch_base_y >= -40) && (i - pitch_base_y <= 50 + 40) ){
                /// ~~~~~~ debug用 ~~~~~~
                /*
                if(result_src.at<float>(i,j) >=0.70) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(255,0,0), 1, 8, 0 );
                else if(result_src.at<float>(i,j) <0.70 && result_src.at<float>(i,j) >= 0.49) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(0,255,0), 1, 8, 0 );
                else if(result_src.at<float>(i,j) <0.49 && result_src.at<float>(i,j) >= 0.43) rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(0,0,255), 1, 8, 0 );
                else rectangle( temp_show, Point(j,i), Point( j + template_img.cols ,i + template_img.rows ), Scalar(50,150,255), 2, 8, 0 );
                */

                maybe_head[0][maybe_head_count] = j; /// x
                maybe_head[1][maybe_head_count] = i; /// y
                maybe_head[2][maybe_head_count] = result_src.at<float>(i,j); /// value
                maybe_head_count++;
            }
        }
    }

    draw_head(temp_show,template_img,maybe_head_count,maybe_head);

    // 把可能是頭的點存進我的 data_structure，改寫from SHOW END 沒有用我的資料結構///

    bubbleSort_maybe_head(maybe_head_count,maybe_head,Y_INDEX);
    bubbleSort_maybe_head(maybe_head_count,maybe_head,X_INDEX);
    // imshow("after_merge",temp_show);

    // debug整合
    // imshow("debug",temp_show);
    // waitKey(0);

}
