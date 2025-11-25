/*
這個程式的功能是：
把 輸入譜的圖片 轉正 (根據五線譜 和水平線的角度差 做旋轉)
*/

//double angle =  Find_Angle(input);
//Wrap_Straight(input,angle);
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "preprocess_0_hough_tool.h"
#include "preprocess_0_watch_hough_line.h"
//Watch_Hough_Line(lines,color_dst);

#include "Recognition.h"

#define PI 3.14159

using namespace cv;
using namespace std;


void do_HoughLine_w_ColumnThreshold(Mat image, vector<Vec2f> & lines,float threshold){
	HoughLines(image, lines, 1, CV_PI/180, image.cols * threshold, 0, 0 );
}


void Recognition_page::Find_Angle(){
	debuging = debuging_pre1;
	if(debuging) cout << endl << "in Find_Angle" << endl;

	// 抓影像中間的小區域, 用該部分的五線譜轉動的角度 來找 影像被轉了幾度
	Mat imgROI = src_img(Rect( (src_img.cols / 2) - (src_img.cols / 16), (src_img.rows / 2) - (src_img.rows / 16), (src_img.cols / 8), (src_img.rows) / 8) ); // 抓中間小區域
	Mat bin_imgROI = imgROI.clone();     // copy一份保留原始影像
	Binary_by_patch(bin_imgROI, 15, 50); // 影像二值化
    bin_imgROI = ~bin_imgROI; 		     // 五線譜、音符的地方 由黑轉白變前景, 其他背景由白轉黑變背景
	if(debuging){
		imwrite("debug_img/pre1_1_Find_Angle_1-ROI.bmp"            , imgROI);
		imwrite("debug_img/pre1_1_Find_Angle_2-ROI_bin_inverse.bmp", bin_imgROI);
		// imshow("Find_Angle",bin_imgROI);
		// waitKey(0);
	}
	
	// HoughLine 找線(五線譜)
	vector<Vec2f> lines;
	do_HoughLine_w_ColumnThreshold(bin_imgROI, lines, 0.5);
	if(debuging){
		Watch_Hough_Line(lines, imgROI, "","debug_img/pre1_1_Find_Angle_3-ROI_bin_inverse_line");
		cout << "lines.size()=" << lines.size() << endl;
		// waitKey(0);
	}


	// 如果 HoughLine 有找到線, 找出 哪個角度的線 是 最多的
	if(lines.size() > 0){
		// 先排序, 把相同角度的線聚在一起
		bubbleSort_by_angle(lines);

		// 分群數字統計的array 容器宣告與初始化
		double * angle_cluster 		 = new double[lines.size()]; // 角度群 可能有幾群, 最差的情況就是 每條線自己各成一群 所以取 lines.size() 個群最保險
		int *    angle_cluster_count = new int   [lines.size()]; // 角度群 內的數量
		int 	 posi 				 = 0;  						 // posi 是指向 目前群位置的指標
		for(int i = 0 ; i < lines.size(); i++){
			angle_cluster		[i] = 0;
			angle_cluster_count [i] = 0;
		}

		// 開始統計 各角度群 的 數量
		angle_cluster		[0] = lines[0][1]; // 初始化 第一條線
		angle_cluster_count [0] = 1;           // 初始化 第一條線
		// 第二條線以後, 回頭看跟前一條線的角度相不相同, 相同代表在同一群就在同一群的位置 count 就+1, 不同代表換下一群了 就 更新成下群 並 在下群位置的 count+1
		for(int i = 1 ; i < lines.size(); i++){
			if(debuging) cout << "lines[" << i << "][1]=" << (lines[i][1] / PI) * 180 << endl;  // debug, 看線的角度

			// 回頭看跟前一條線的角度相不相同
			if(lines[i][1] - lines[i-1][1] != 0){
				// 不同代表換下一群了 就 更新成下群 並 在下群位置的 count+1
				posi++;									  // 更新成下群
				angle_cluster	   [posi] = lines[i][1];  // 更新成下群
				angle_cluster_count[posi]++;			  // 下群位置的 count+1
			}
			else
				// 相同代表在同一群就在同一群的位置++
				angle_cluster_count[posi]++;
		}

		// 找數量最多的位置 的第一個
		int max_count = 0;
		for(int go_count = 0 ; go_count <= posi; go_count++){
			if(max_count < angle_cluster_count[go_count]){
				max_count = angle_cluster_count[go_count];
			}		
		}
		if(debuging) cout << "max_count=" << max_count << endl;

		// 找最多的位置對應的是誰
		double max_angle = 0.0;
		for(int go_count = 0; go_count <= posi; go_count++){
			if(angle_cluster_count[go_count] == max_count){
				max_angle = angle_cluster[go_count];
				break;
			}
		}
		
		// 把屬於 max_angle 的線畫出來看看
		if(debuging){
			cout << "max_angle=" << max_angle << endl;
			vector<Vec2f> max_angle_lines;
			for(int go_line = 0; go_line < lines.size(); go_line++){
				if(max_angle == lines[go_line][1]){
					max_angle_lines.push_back(lines[go_line]);
				}
			}
			Watch_Hough_Line(max_angle_lines, imgROI, "","debug_img/pre1_1_Find_Angle_4-ROI_bin_inverse_max_angle_lines");
			cout << "max_angle_lines.size()=" << max_angle_lines.size() << endl;
		}

		delete [] angle_cluster		 ;
		delete [] angle_cluster_count;

		warp_angle = ( max_angle / PI ) * 180; //現在是徑度量喔~~~~我習慣度度量且我的Wrap_Straight也是用度度量，所以轉一下再return
	}
	else
		// 如果 HoughLine 沒找到線, 就 return 90度 就代表最正, 不會旋轉影像
		warp_angle = double(90);  

}

void Recognition_page::Wrap_Straight(){
	debuging = debuging_pre1;
	if(debuging) cout << endl<<"in Wrap_Straight" << endl;

	double rotate_angle = warp_angle - 90;
	if(debuging) cout << "rotate_angle=" << rotate_angle << endl;
	
	Point center = Point(src_img.cols/2, src_img.rows/2); // the center of the picture
	Mat rot(2, 3, CV_64FC1);
	rot = getRotationMatrix2D(center, rotate_angle, 1); //produce wrapAffine_matrix
	
	if(debuging){
		imwrite("debug_img/pre1_2_Warp_Straight_1-before_warp.bmp", src_img);
		// imshow("before_warp",src_img);
		// waitKey(0);
		cout << "before_img.cols=" << src_img.cols <<" , rows=" << src_img.rows << endl;
	}

	Mat proc_img = src_img.clone();
	if(src_img.rows < src_img.cols) resize(proc_img, proc_img, Size(src_img.cols,src_img.cols), 0, 0, INTER_CUBIC);
	warpAffine(src_img, proc_img, rot, proc_img.size(), 0, 0, 255);
	src_img = proc_img.clone();

	if(debuging){
		imwrite("debug_img/pre1_2_Warp_Straight_2-after_warp.bmp",src_img);
		// imshow("after_warp",src_img);
		// waitKey(0);
		cout << "after_img.cols=" << src_img.cols <<" , rows=" << src_img.rows << endl;
	}
}
