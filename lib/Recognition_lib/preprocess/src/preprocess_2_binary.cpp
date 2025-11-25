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

#include "Recognition.h"

using namespace std;
using namespace cv;

// 目前用的是這個
unsigned char Recognition_page::Binary(Mat& dst){  
    // 統計 dst 裡面的 灰階 顏色數量
    // 初始化容器
    int color_count[256];
    for(int i = 0 ; i < 256 ; i++) color_count[i] = 0;
    
    // 開始走訪dst 每一個pixel 統計 裡面的 灰階 顏色數量
    for(int i = 0 ; i < dst.rows ; i++){
        for(int j = 0 ; j < dst.cols ; j++){
            color_count[dst.at<uchar>(i,j)]++;
        }
    }
    
    // 有用到的顏色 挑出來, 記錄其 顏色 和 使用次數
    int go_color_value = 0;
    int color_value      [256];
    int color_value_count[256];
    for(int i = 0 ; i < 256 ; i++){
        color_value      [i] = 0;
        color_value_count[i] = 0;
    }

    for(int i = 0 ; i < 256 ; i++){
        if(color_count[i]){
            color_value      [go_color_value] = i;
            color_value_count[go_color_value] = color_count[i];
            go_color_value++;
        }
    }

    // 計算影像面積
    int area = dst.cols * dst.rows;


    // (排除沒使用到顏色的部分, 只看有使用到顏色部分的)
    // 累積灰階顏色 達到 總面積的 XX%(目前測50%效果好) 時 差不多就是內容物+雜訊了, 因為有雜訊所以 threshold 要再向左 shift 一些數值
    int color_acc_count[256];
    float area_rate    = 0.50;
    int threshold      = 0;
    int threshold_posi = 0;
    int shift          = 30;

    for(int i = 0 ; i < go_color_value ; i++){
        if(i== 0) color_acc_count[0] = color_value_count[0];
        else{
            color_acc_count[i] = color_value_count[i] + color_acc_count[i-1];
            if(color_acc_count[i] < area * area_rate){
                threshold = color_value[i];
                threshold_posi = i;
            }
        }
    }

    threshold -= shift;
	if(threshold < 0) threshold = 0;
	// cout<<"threshold = "<< threshold << endl;

    // 二值化
	for(int row = 0 ; row < dst.rows; row++){
		for(int col = 0 ; col < dst.cols ; col++){
			if(dst.at<uchar>(row,col) <= threshold) dst.at<uchar>(row,col) = 0;
			else                                    dst.at<uchar>(row,col) = 255;
		}
	}

	return threshold;
}

// 目前用的是這個
void Recognition_page::Binary_by_patch(Mat& dst, const int div_row, const int div_col){
	debuging = debuging_pre2;
    Mat patch_img;
	Mat threshold_map(div_row, div_col, CV_8UC1);  // debug用的, 觀察每個patch用什麼threshold
    // 切塊來做 二值化
    const int height = dst.rows;
	// const int div_row = 80;
	const int div_height = height / div_row;
	const int mod_height = height % div_row;

	const int width = dst.cols;
	// const int div_col = 160;
	const int div_width = width / div_col;
	const int mod_width = width % div_col;
	if(debuging){
		cout << "    width=" <<     width << ",     height=" <<     height << endl;
		cout << "mod_width=" << mod_width << ", mod_height=" << mod_height << endl;
		cout << endl;
	}

	int height_frame_acc = mod_height;
	int height_acc = 0;

	int width_frame_acc = mod_width;
	int width_acc = 0;

	for(int go_row = 0 ; go_row < div_row ; go_row++){
		//推出 下一格 height
		int height_acc_next = height_acc + div_height - 1;
		if(height_frame_acc/div_row){
			height_acc_next++;
			height_frame_acc %= div_row;
		}

		for(int go_col = 0 ; go_col < div_col ; go_col++){
			//推出 下一格 width
			int width_acc_next = width_acc + div_width - 1;
			if(width_frame_acc / div_col){
				width_acc_next++;
				width_frame_acc %= div_col;
			}
			if(debuging){
				cout<< "colRange="<< width_acc  <<" ~ "<< width_acc_next  << ", " <<
					   "rowRange="<< height_acc <<" ~ "<< height_acc_next << endl;
			}

			patch_img = dst.rowRange(height_acc, height_acc_next+1)
					       .colRange( width_acc,  width_acc_next+1);
			threshold_map.at<uchar>(go_row,go_col) = Binary(patch_img);

			//更新 width
			width_acc        = width_acc_next + 1;
			width_frame_acc += mod_width;
		}
		//更新 height
		height_acc        = height_acc_next + 1;
		height_frame_acc += mod_height;

		//下一row時，width從頭開始
		width_acc = 0;
		width_frame_acc = mod_width;
	}
	if(debuging){
		cout << endl;
		imshow("threshold_map", threshold_map);
		cvMoveWindow("threshold_map", 10, 10);
		imshow("dst", dst);
		cvMoveWindow("dst", 10, 80);
		cout<< threshold_map << ' ' << endl;
		waitKey(0);
		
		cv::destroyWindow("threshold_map");
		cv::destroyWindow("dst");
		imwrite("debug_img/pre2-Binarize.bmp", dst);
	}

}
