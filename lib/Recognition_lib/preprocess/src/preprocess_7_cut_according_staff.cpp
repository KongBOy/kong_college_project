/*
這個程式的功能是：
利用 preprocess_6 找出的 五線譜端點
把 輸入譜的圖片 以五線譜為單位做切割(根據 每組五線譜端點 做切割
*/

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include "preprocess_7_cut_according_staff.h"

// #include "vertical_map_to_recognize.h"



#include "string_tools.h"

#define HORIZONTAL_DIR "horizontal_test/"

#define EXTEND_RATE 1.0
#define STAFF_LINE_COUNT 5
#define X_Y_COUNT 2



using namespace cv;
using namespace std;


void Perspective_trans(double x,double y,Mat warp_matrix,double & result_x,double& result_y)
{
    float trans_proc_1[3];

    trans_proc_1[0] = x;
    trans_proc_1[1] = y;
    trans_proc_1[2] = 1;
    // cout << "_1[0] = " << trans_proc_1[0] << " , _1[1] = " << trans_proc_1[1] << " , _1[2] = " << trans_proc_1[2] << endl;

    float trans_proc_2[3];
    for(int j = 0 ; j < 3 ; j++){
        trans_proc_2[j]  = trans_proc_1[0]*warp_matrix.at<double>(j,0);
        trans_proc_2[j] += trans_proc_1[1]*warp_matrix.at<double>(j,1);
        trans_proc_2[j] += trans_proc_1[2]*warp_matrix.at<double>(j,2);
    }
    cout << "_2[0] = " << trans_proc_2[0] << " , _2[1] = " << trans_proc_2[1] << " , _2[2] = " << trans_proc_2[2] << endl;

    result_x = (trans_proc_2[0]/trans_proc_2[2]);
    result_y = (trans_proc_2[1]/trans_proc_2[2]);
    cout << endl;


    cout <<   "result_x = " << result_x
         <<" , result_y = " << result_y << endl;

}


//根據 每組staff 的 最左、右、上、下 切出proc_img
//根據 每組staff 的 第一條和第五條的線頭 assign srdTri
void Cut_staff(Mat test_bin,Mat src_bin_erase_line,
               int& staff_count,int*** left_point, int*** right_point,
               Mat final_rl_img_roi[],Mat final_img_roi[],
               double trans_start_point_x[],double trans_start_point_y[])
               // trans_start_point的意思是 五線譜最左上角的那一點
{
    int* left  = new int[staff_count];
    int* right = new int[staff_count];
    int* up    = new int[staff_count];
    int* down  = new int[staff_count];
    for(int i = 0 ; i < staff_count ; i++){
         left[i] = 1000; // 隨便很 大 的數
        right[i] = -100; // 隨便很 小 的數
           up[i] = 1000; // 隨便很 大 的數
         down[i] = -100; // 隨便很 小 的數
    }

    // 現在要找出 要拉到怎麼樣才算正規 的規格~~~
    // 水平x方面：就五線譜長度 ~~ 但每條線好像都會差一咪咪所以取平均
    // 垂直y方面：就staff五條線間隔的和
    int ave_staff_line_leng = 0;

    // 更新左右
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        /// 需要看第一~五條線的左 右喔！所以要for迴圈
        for(int line_num = 0 ; line_num < STAFF_LINE_COUNT ; line_num++){
            ave_staff_line_leng += right_point[staff_num][line_num][0] - left_point[staff_num][line_num][0];
            if( left_point[staff_num][line_num][0] <=  left[staff_num])  left[staff_num] =   left_point[staff_num][line_num][0];
            if(right_point[staff_num][line_num][0] >= right[staff_num]) right[staff_num] =  right_point[staff_num][line_num][0];
        }
    }
    ave_staff_line_leng /= (staff_count*STAFF_LINE_COUNT);

    // 更新上下
    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        // 只需要看第一條線 的左右兩點 即可！不用for迴圈
        if(left_point[staff_num][0][1] < right_point[staff_num][0][1]) up[staff_num] = left_point[staff_num][0][1];
        else up[staff_num] = right_point[staff_num][0][1];
        // 只需要看第五條線 的左右兩點 即可！不用for迴圈
        if(left_point[staff_num][4][1] > right_point[staff_num][4][1]) down[staff_num] = left_point[staff_num][4][1];
        else down[staff_num] = right_point[staff_num][4][1];
    }


    for(int staff_num = 0 ; staff_num < staff_count ; staff_num++){
        cout << "left [" << left [staff_num] << "] = " << left [staff_num] << endl;
        cout << "right[" << right[staff_num] << "] = " << right[staff_num] << endl;
        cout << "up   [" << up   [staff_num] << "] = " << up   [staff_num] << endl;
        cout << "down [" << down [staff_num] << "] = " << down [staff_num] << endl;
        // waitKey(0);

        int tran_base_left = left[staff_num];
        int tran_base_up = up[staff_num];

        // 0左上  1左下  2右上  3右下
        Point2f dstTri[4];  // distination point
        Point2f srcTri[4];  // sorce point，有扣掉 五線譜 左邊、上面 的空白處
        srcTri[0] = Point2f(left_point [staff_num][0][0] -tran_base_left,left_point [staff_num][0][1] -tran_base_up);
        srcTri[1] = Point2f(left_point [staff_num][4][0] -tran_base_left,left_point [staff_num][4][1] -tran_base_up);
        srcTri[2] = Point2f(right_point[staff_num][0][0] -tran_base_left,right_point[staff_num][0][1] -tran_base_up);
        srcTri[3] = Point2f(right_point[staff_num][4][0] -tran_base_left,right_point[staff_num][4][1] -tran_base_up);
        for(int i = 0 ; i < 4 ; i++) cout << "srcTri[" << i << "].x = " << srcTri[i].x << " srcTri[" << i << "].y = " << srcTri[i].y << endl;

        // 找頭偵錯
        bool right_staff = true;
        for(int i = 0 ; i < 4 ; i++){
            if( srcTri[i].x < 0 || (srcTri[i].x > test_bin.cols -1) ||
                srcTri[i].y < 0 || (srcTri[i].y > test_bin.rows -1) ){
                right_staff = false;  // 代表這組有錯誤，跳過這組
                cout << "wrong staff" << endl;
                break;
            }
        }

        ///代表這組有錯誤，跳過這組
        if(right_staff == false){
            staff_count--;
            continue;
        }


        // int stander_col=925;   //盽计(﹚)
        // int stander_row=(stander_col*(srcTri[3].y-srcTri[0].y)/ ave_staff_line_leng  );  //+1
        // int stander_row_expend=stander_row*3;

        // cout << "standard_col = " << stander_col << endl;
        // cout << "standard_row = " << stander_row << endl;
        cout << "rate = " << (srcTri[3].y-srcTri[0].y)/(srcTri[3].x-srcTri[0].x);

        // Mat stander(stander_row_expend,stander_col,CV_8UC1);

        dstTri[0] = Point2f(0,43*0);
        dstTri[1] = Point2f( 0,43*1 );
        dstTri[2] = Point2f( 1156,43*0);
        dstTri[3] = Point2f( 1156, 43*1);

        for(int i = 0 ; i < 4 ; i++)
            cout << "dstTri[" << i << "].x = " << dstTri[i].x << " dstTri[" << i << "].y = " << dstTri[i].y << endl;

        Mat warp_matrix = getPerspectiveTransform(srcTri,dstTri);
        cout << warp_matrix << ' ' << endl;

        for(int i = 0 ; i < 3 ; i++){
            for(int j = 0 ; j < 3 ; j++){
                cout << warp_matrix.at<double>(i,j) << " ";
            }
            cout << endl;
        }


        ///不能切得剛剛好，上下再多切一些因為有些太高音或太低音這樣子囉
        int extend =(down[staff_num] - up[staff_num])* EXTEND_RATE;


        // ***************************************************************************************************
        // ***************************************************************************************************
        // ***************************************************************************************************
        // 看 "五線譜左上角的點" 轉換後會在哪裡~~~ 不能直接用 最左邊和最上的點喔!!!! 因為他們並不一定等於五線譜的最左上角的點~~
        if(  left_point[staff_num][0][0] < left[staff_num] )
             trans_start_point_x[staff_num] = left_point[staff_num][0][0];
        else trans_start_point_x[staff_num] = left_point[staff_num][0][0] - tran_base_left;

        if(  left_point[staff_num][0][1] < up[staff_num] )
             trans_start_point_y[staff_num] = left_point[staff_num][0][1];
        else trans_start_point_y[staff_num] = left_point[staff_num][0][1] - tran_base_up + extend;


        // trans_start_point_x[staff_num] = (trans_proc_2[0]/trans_proc_2[2]);
        // trans_start_point_y[staff_num] = (trans_proc_2[1]/trans_proc_2[2]);
        Perspective_trans( trans_start_point_x[staff_num] , trans_start_point_y[staff_num],warp_matrix,
                           trans_start_point_x[staff_num] , trans_start_point_y[staff_num] );

        cout << "trans_start_point_x = " << trans_start_point_x
            <<" , trans_start_point_y = " << trans_start_point_y << endl;
        ///***************************************************************************************************
        ///***************************************************************************************************
        ///***************************************************************************************************





        ///***************************************************************************************************
        ///***************************************************************************************************
        ///***************************************************************************************************
        ///根據 最左、下、右、上 來切出proc_img
        ///        int staff_width = srcTri[0].x - srcTri[2].x;
        ///        int staff_height = srcTri[0].y - srcTri[1].y;

        Mat proc_img = test_bin(Rect(left[staff_num] , up[staff_num]-extend,
                                    right[staff_num] - left[staff_num] , down[staff_num] - up[staff_num] +extend*2));
        Mat proc_rl_img = src_bin_erase_line(Rect(left[staff_num] , up[staff_num]-extend,
                                                 right[staff_num] - left[staff_num] , down[staff_num] - up[staff_num] +extend*2));
        // imshow("proc_img",proc_img);
        // imshow("proc_rl_img",proc_rl_img);


        Mat test_size(43*2,1156,CV_8UC1,Scalar(0));
        // Mat final_img = test_size.clone();


        // 怕存的空間太小，
        Mat final_img;
        Mat final_rl_img;
        if(proc_img.cols < 1166 || proc_img.rows < 43*4){ ///小 ~ 大
            int containor_width = proc_img.cols;
            int containor_height = proc_img.rows;
            if(containor_width < 1166) containor_width = 1166;
            if(containor_height < 43*4) containor_height = 43*4;
            Mat temp(containor_height,containor_width,CV_8UC1,Scalar(255));
            final_img = temp.clone();
            final_rl_img = temp.clone();
        }
        else{ ///大 ~ 小
            final_img = proc_img.clone();
            final_rl_img = proc_rl_img.clone();
        }

        ///不用怕存的空間不夠大~~~但是轉完後要roi，因為原本的圖太大了！會剩很多空白~~~




        warpPerspective(proc_img,final_img,warp_matrix, final_img.size(),0,0,255);
        warpPerspective(proc_rl_img,final_rl_img,warp_matrix, final_rl_img.size(),0,0,255);
        ///warpPerspective(proc_img,final_img,warp_matrix, proc_img.size(),0,0,0);
        ///warpPerspective(proc_rl_img,final_rl_img,warp_matrix, proc_rl_img.size(),0,0,0);
        //warp to strength
        //imshow("warp_perspective",final_img);


        // 
        // ****************************************************************************************************
        // ****************************************************************************************************
        // ****************************************************************************************************
        //  找轉換後的底在哪裡(trans_down_point) 就是左下角和右下角去手動轉，然後看哪一個比較"上"面囉！(測過覺得"下"面比較難看)
        //  右下角
        double trans_down_point_right_x = right[staff_num] - left[staff_num];///right[staff_num];
        double trans_down_point_right_y = down[staff_num] - up[staff_num] +extend*2;/// down[staff_num] + extend;
        Perspective_trans(trans_down_point_right_x,trans_down_point_right_y,warp_matrix,
                          trans_down_point_right_x,trans_down_point_right_y);
        // 左下角
        double trans_down_point_left_x = left[staff_num];
        double trans_down_point_left_y = down[staff_num] - up[staff_num] +extend*2;/// down[staff_num] + extend;
        Perspective_trans(trans_down_point_left_x,trans_down_point_left_y,warp_matrix,
                          trans_down_point_left_x,trans_down_point_left_y);
        // 看哪個比較上面
        double trans_down_point = 0;
        if(trans_down_point_left_y < trans_down_point_right_y)
             trans_down_point = trans_down_point_left_y;
        else trans_down_point = trans_down_point_right_y;

        // 用比較下面的點去切roi
        final_img_roi[staff_num] = final_img(Rect(0, 0, 1166, trans_down_point));
        final_rl_img_roi[staff_num] = final_rl_img(Rect(0, 0, 1166, trans_down_point));
        ///****************************************************************************************************
        ///****************************************************************************************************
        ///****************************************************************************************************



        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_0-srcimg.jpg",proc_img);
        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_1-result.bmp",final_img_roi);
        // imwrite((string)HORIZONTAL_DIR + IntToString(staff_num) +"-WARP_PERSPECTIVE_2-rl-result.bmp",final_rl_img_roi);

        Mat debug = final_img_roi[staff_num].clone();
        cvtColor(final_img_roi[staff_num],debug,CV_GRAY2BGR);
        line(debug,Point(trans_start_point_x[staff_num],trans_start_point_y[staff_num]),Point(trans_start_point_x[staff_num],trans_start_point_y[staff_num]+43),Scalar(0,255,0),5);
        // imshow("debug2",debug);
        // waitKey(0);
        // imshow("warp_perspective_roi",final_img_roi[staff_num]);
        // imshow("warp_perspective_rl_roi",final_rl_img_roi[staff_num]);
        // waitKey(0);

        Mat vertical_map(final_rl_img_roi[staff_num].rows,
                         final_rl_img_roi[staff_num].cols,CV_8UC1,Scalar(255));
        // Vertical_map_to_recognize(final_rl_img_roi,vertical_map,final_img_roi,trans[0].y);

        // ***************************************************************************************************
        // if(staff_num < 4 ) continue;

        /*
        int l_edge[200];
        int r_edge[200];
        int distance[200];
        int mountain_area[200];
        int e_count = 0;
        int* note_type;
        recognition_0_vertical_map_to_speed_up(final_rl_img_roi[staff_num],
                                               vertical_map,
                                               e_count,l_edge,r_edge,distance,mountain_area,note_type);
        */



        /*
        // 自己設資料結構 head
        int maybe_head_count = 0;
        float maybe_head[3][200];
        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                maybe_head[i][j] = 0;


        // 自己設資料結構 line
        short lines[3][200];///[0]頂點x [1]頂點y [2]長度
        bool lines_dir[2][200];///[0]左下 [1]右上

        for(int i = 0 ; i < 3 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                lines[i][j] = 0;

        for(int i = 0 ; i < 2 ; i++)
            for(int j = 0 ; j < 200 ; j++)
                lines_dir[i][j] = false;

        int lines_count = 0;

        int lines_time[200];
        for(int i = 0 ; i < 200 ; i++) lines_time[i] = 0;
        */
        /*
        // 自己設資料結構 note ~~~~~~~寫出去~~~~~~~~~

        int row_note[5][1000]; /// 0 = x , 1 = y , 2 = type , 3 = time_bar , 4 = 音高
        int row_note_count = 0;
        for(int i = 0 ; i < 5 ; i++)
            for(int j = 0 ; j < 1000 ; j++)
                row_note[i][j] = 0;
        */



        /// ● 0 全音符
        /// 1 全休止符
        /// ● 2 二分 音符
        /// 3 二分 休止符
        /// ● 4 四分 音符
        /// ● 5 四分 休止符
        /*
        Mat template_img_4 = imread("Resource/note/4/4.bmp",0);
        Mat template_img_4_rest = imread("Resource/note/4-rest/4-rest.bmp",0);
        Mat template_img_2 = imread("Resource/note/2/2.bmp",0);
        Mat template_img_0 = imread("Resource/note/0/0.bmp",0);
        Mat template_img_1 = imread("Resource/note/0-rest/0-rest.bmp",0);
        Mat template_img_3 = imread("Resource/note/2-rest/2-rest.bmp",0);
        Mat template_img_8 = imread("Resource/note/8-rest/8-rest-3.bmp",0);
        Mat template_img_6 = imread("Resource/note/6-rest/6-rest-2.bmp",0);

        recognition_0_all_head(1,template_img_1,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(3,template_img_3,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        */
        /*
        recognition_0_all_head(4,template_img_4,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(2,template_img_2,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(0,template_img_0,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(5,template_img_4_rest,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);


        recognition_0_all_head(8,template_img_8,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        recognition_0_all_head(6,template_img_6,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);
        */
        // recognition_0_all_head(7,template_img_8,final_rl_img_roi[staff_num],final_img_roi[staff_num],e_count,l_edge,distance,trans[0].y,row_note_count,row_note);



        /*
        bubbleSort_note(row_note_count,row_note,Y_INDEX);
        bubbleSort_note(row_note_count,row_note,X_INDEX);

        recognition_5_find_pitch(final_img_roi[staff_num],Mat(15,15,CV_8UC1),row_note_count,row_note,trans[0].y);



        // midi(row_note_count,row_note);

        cout << endl << endl;
        cout << "print row_note~~~" << endl;
        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            cout << "go_row_note = " << go_row_note
                <<" , type = " << row_note[2][go_row_note]
                <<" , x = " << row_note[0][go_row_note]
                <<" , y = " << row_note[1][go_row_note]
                <<" , time = " << row_note[3][go_row_note]
                <<" , pitch = " << row_note[4][go_row_note]
                <<endl;
        }
        cout << "row_note_count = " << row_note_count << endl;

        for(int go_row_note = 0 ; go_row_note < row_note_count ; go_row_note++){
            note[0][note_count+go_row_note] = row_note[0][go_row_note];
            note[1][note_count+go_row_note] = row_note[1][go_row_note];
            note[2][note_count+go_row_note] = row_note[2][go_row_note];
            note[3][note_count+go_row_note] = row_note[3][go_row_note];
            note[4][note_count+go_row_note] = row_note[4][go_row_note];
        }
        note_count += row_note_count;
        // play();
        */
    }

}
