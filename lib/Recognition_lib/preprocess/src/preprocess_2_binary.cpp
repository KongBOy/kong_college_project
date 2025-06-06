/*
這個程式的功能是：
把 輸入譜的圖片 做二值化
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include <iomanip>
#include <string>
#include <fstream>
#include <cstdlib>
#define BIN_DIR "1-Binary/"

#include "preprocess_2_binary.h"

using namespace std;
using namespace cv;


Mat threshold_map; //把所有的threshold存起來的容器，debug用
static int go_row = 0; //在Mat裡面跑的座標
static int go_col = 0; //在Mat裡面跑的座標

static Mat temp_bin;

const int exp_color = 40;  //自己測試了很多圖大概估測note的顏色大概在30左右


//方法一：速度比方法二快，

void Binary_by_patch(Mat src, Mat& dst , const int div_row, const int div_col)
{
	threshold_map.create(div_row, div_col, CV_8UC1);
    // 切塊來做 二值化
    const int height = src.rows;
	// const int div_row = 80;
	const int div_height = height / div_row;
	const int mod_height = height % div_row;

	const int width = src.cols;
	// const int div_col = 160;
	const int div_width = width / div_col;
	const int mod_width = width % div_col;
	/*
	cout<<"width = "<<width<<" height = "<<height <<endl;
	cout<<"mod_width = "<<mod_width<<" mod_height = "<<mod_height <<endl;
	cout<<endl;
	*/

	// Mat debug = test_eliminate_roi1.clone();
	// cvtColor(test_eliminate_roi1,debug,CV_GRAY2BGR);

	int height_frame_acc = mod_height;
	int height_acc = 0;

	int width_frame_acc = mod_width;
	int width_acc = 0;

	for(go_row = 0 ; go_row < div_row ; go_row++){
		//推出 下一格 height
		int height_acc_next = height_acc + div_height - 1;
		if(height_frame_acc/div_row){
			height_acc_next++;
			height_frame_acc %= div_row;
		}

		for(go_col = 0 ; go_col < div_col ; go_col++){
			//推出 下一格 width
			int width_acc_next = width_acc + div_width - 1;
			if(width_frame_acc/div_col){
				width_acc_next++;
				width_frame_acc %= div_col;
			}
			/*
			cout<<"colRange = "<<width_acc<<" ~ "<<width_acc_next<<", "<<
					"rowRange = "<<height_acc<<" ~ "<<height_acc_next<<endl;
			*/

			temp_bin = dst.rowRange(height_acc,height_acc_next+1)
					      .colRange( width_acc, width_acc_next+1).clone();
			threshold_map.at<uchar>(go_row,go_col) =
				Binary(dst.rowRange(height_acc,height_acc_next+1)
					      .colRange( width_acc, width_acc_next+1),
				       temp_bin);
			dst.rowRange(height_acc,height_acc_next+1)
               .colRange( width_acc, width_acc_next+1) = temp_bin.clone();


			//更新 width
			width_acc = width_acc_next + 1;
			width_frame_acc += mod_width;
		}
		//更新 height
		height_acc = height_acc_next + 1;
		height_frame_acc += mod_height;

		//下一row時，width從頭開始
		width_acc = 0;
		width_frame_acc = mod_width;
	}

 //   /**/        imshow("ppt",dst);
 //    /**/       waitKey(0);

//	cout<<endl;
//	imshow("threshold_map",threshold_map);
//	cout<<threshold_map<<' '<<endl;
}



unsigned char Binary(Mat src, Mat & dst)  //src：原圖的copy ； dst：會改掉原來傳進來的圖片~~
{
	Mat temp_dst;
	//Canny(src,temp_dst,25,75,3);
    int color_value[256];
    int color_count[256];
    int color_gradi[3][256];
    // 初始化容器
    for(int i = 0 ; i < 256 ; i++){
        color_value[i] = 0;
        color_count[i] = 0;
    }
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 256 ; j++){
             color_gradi[i][j] = 0;
        }
    }
    
    
    // 
    // int color_high_range = 0;
    // int color_low_range  = 0;
    // int local_max[256];

    for(int i = 0 ; i < src.rows ; i++){
        for(int j = 0 ; j < src.cols ; j++){
            color_count[src.at<uchar>(i,j)]++;
        }
    }

    // int last_posi = 0;
    int color_range_count = 0;
    int color_acc_count[256];
    int area = src.cols * src.rows;
    int threshold = 0;
    int threshold_acc = 0;

    for(int i = 0 ; i < 256 ; i++){
        if(color_count[i]){
            color_value[color_range_count] = i;
            color_gradi[0][color_range_count]= color_count[i];
            color_range_count++;

        }

    }


    for(int level = 1 ; level < 3 ; level++){
        for(int i = 0 ; i < color_range_count-level ; i++){
            color_gradi[level][i] = color_gradi[level-1][i+1] - color_gradi[level-1][i];
        }
    }

    /*
    // 顯示資料
    ofstream ofile("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_star_data.txt",ios::trunc);

    for(int i = 0 ; i < color_range_count ;i++){
        cout<<"i = "<<color_value[i];
        ofile<<"i = "<<color_value[i];
        cout<<" value= "<<setw(4)<<setfill('0')<<color_gradi[0][i];
        ofile<<" value= "<<setw(4)<<setfill('0')<<color_gradi[0][i];


        if(i >=1 && i < color_range_count -0){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[1][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[1][i-1];
        }
        if(i >=1 && i < color_range_count -1){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[2][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[2][i-1];
        }

        ofile<<' ';
        for(int j = 0 ; j < color_gradi[0][i];j++){
            ofile<<'*';
        }
        if(i >=1 && i < color_range_count -1){
            if(color_gradi[2][i-1] < 0)ofile<<"●";
        }

        ofile<<endl;
        cout<<endl;
    }
    */

    // 累積
    int threshold_posi = 0;
    float area_rate = 0.50;

    // int threshold2 = 0;
    // int threshold_posi2 = 0;
    // float area_rate2 = 0.03125;

    for(int i = 0 ; i < color_range_count ; i++){
        if(i== 0) color_acc_count[0] = color_gradi[0][0];
        else{
            color_acc_count[i] = color_gradi[0][i] + color_acc_count[i-1];
            if(color_acc_count[i] < area * area_rate){
                threshold = color_value[i];
                threshold_posi = i;
            }

            // if(color_acc_count[i] < area * area_rate2){
            //     threshold2 = color_value[i];
            //     threshold_posi2 = i;
            // }
        }
    }



    int shift2 = 30;
    threshold -= shift2;
	const int shift = 0;
	if(threshold >= shift) threshold -= shift ; else threshold = 0;

	//cout<<"threshold = "<<threshold<<endl;

    // 二值化
	for(int row = 0 ; row < src.rows; row++){
		for(int col = 0 ; col < src.cols ; col++){
			if(src.at<uchar>(row,col) <= threshold) src.at<uchar>(row,col) = 0;
			else                                    src.at<uchar>(row,col) = 255;
		}
	}

	dst = src.clone();// dst：會改掉原來傳進來的圖片~~所以複製一下二值化好的圖片就可以改道原來的圖囉!!!

	return threshold;
}

void test_Binary(Mat src)  //src：原圖的copy ； dst：會改掉原來傳進來的圖片~~
{
	const int width = src.cols;
	const int height = src.rows;
	Mat dst = src.clone();

	Mat temp(height,width,CV_8UC1,Scalar(0));
	threshold_map = temp.clone();

	Mat temp_dst;
	Canny(src, temp_dst, 75,75,3);
	// imshow("debug",temp_dst);
	// waitKey();

	int threshold = 0;
	for(go_row = 0 ; go_row < height; go_row++){ //因為要 當格的上下一格做運算 所以 +-1 控制不會超過圖片range
		for(go_col = 0 ; go_col < width; go_col++){
			if(temp_dst.at<uchar>(go_row, go_col)){  //canny出來的有值得話，代表該點為edge(周邊顏色變動很大)
				//if( (go_col == 0) && (go_row == 0) ) threshold = src.at<uchar>(go_row+1,go_col+1); // 左上
				//else if( (go_col == 0) && (go_row != 0) ) threshold = src.at<uchar>(go_row+1,go_col);//左
				//else if( (go_col == 0) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col);//左下
				//else if( (go_col != width-1) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//下
				//else if( (go_col == width-1) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//右下
				//else if( (go_col == width-1) && (go_row != height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//右
				//else if( (go_col == width-1) && (go_row == 0) ) threshold = src.at<uchar>(go_row,go_col-1);//右上
				//else if( (go_col != 0) && (go_row == 0) ) threshold = src.at<uchar>(go_row+1,go_col);//上
				//else
				if((go_col != 0) && (go_row != 0) && (go_col != width-1) && (go_row != height-1)){
					if( (src.at<uchar>(go_row-1,go_col-1) - src.at<uchar>(go_row+1,go_col+1)) > 0)  //canny該點還不夠準，可以仔細看圖，每個黑色的旁邊都有一咪咪的灰灰，所以找該格的附近，較靠近正確的顏色，&記錄下來
						 threshold = src.at<uchar>(go_row+1,go_col+1);
					else threshold = src.at<uchar>(go_row-1,go_col-1);

					//threshold = src.at<uchar>(go_row,go_col);
				}
				else{
					if(src.at<uchar>(go_row,go_col) < exp_color ){
						threshold = src.at<uchar>(go_row,go_col); //雖然不準了點，但少了很多if else!!!
						threshold += 10;
					}
					else threshold = exp_color;
				}

				threshold_map.at<uchar>(go_row,go_col) = threshold;

				//cout<<"threshold = "<<threshold<<endl;
				//imshow("test_binary",dst);
				//imshow("threshold_map",threshold_map);
				//waitKey();
			}
			else{ //edge_point == 0
			
				if( (go_col == 0) && (go_row == 0) ){
					if(src.at<uchar>(go_row,go_col) < exp_color) threshold = src.at<uchar>(go_row,go_col)+10;
					else threshold = exp_color;
				}
				else if( (go_col != 0) && (go_row == 0)) threshold = threshold_map.at<uchar>(go_row,go_col-1);
				else if( (go_col == 0) && (go_row != 0)) threshold = threshold_map.at<uchar>(go_row-1,go_col);
				else
				if( (go_col != 0) && (go_row != 0)){
					threshold = threshold_map.at<uchar>(go_row-1,go_col);
					threshold += threshold_map.at<uchar>(go_row,go_col-1);
					threshold /= 2;
				}

				threshold_map.at<uchar>(go_row,go_col) = threshold;
			}

			const int shift = 15;

			if(src.at<uchar>(go_row,go_col) > (threshold + shift) )
				 dst.at<uchar>(go_row,go_col) = 255;
			else dst.at<uchar>(go_row,go_col) = 0;

			//threshold_map.at<uchar>(go_row,go_col) = threshold;
			//cout<<"go_col = "<<go_col;
		}

		//cout<<endl;
	}
	///******************************************************
	///imwrite("test_binary.jpg",dst);
	//dst = src.clone();// dst：會改掉原來傳進來的圖片~~所以複製一下二值化好的圖片就可以改道原來的圖囉!!!
}
