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


const int exp_color = 40;  //自己測試了很多圖大概估測note的顏色大概在30左右


// 嘗試找拐點, 但效果不好, 也是都寫了就保留也許未來有機會用到
void Calculate_gradient(Mat src){
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

    // 統計 src 裡面的 灰階 顏色數量
    for(int i = 0 ; i < src.rows ; i++){
        for(int j = 0 ; j < src.cols ; j++){
            color_count[src.at<uchar>(i,j)]++;
        }
    }

    
    // 有用到的顏色 挑出來, 記錄其 顏色 和 使用次數
    int go_color_value = 0;
    for(int i = 0 ; i < 256 ; i++){
        if(color_count[i]){
            color_value[go_color_value] = i;
            color_gradi[0][go_color_value]= color_count[i];
            go_color_value++;
        }
    }
    // 計算 1階, 2階 微分
    for(int level = 1 ; level < 3 ; level++){
        for(int i = 0 ; i < go_color_value-level ; i++){
            color_gradi[level][i] = color_gradi[level-1][i+1] - color_gradi[level-1][i];
        }
    }
    /*
    // 顯示資料
    ofstream ofile("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_star_data.txt",ios::trunc);

    for(int i = 0 ; i < go_color_value ;i++){
        cout<<"i = "<<color_value[i];
        ofile<<"i = "<<color_value[i];
        cout<<" value= "<<setw(4)<<setfill('0')<<color_gradi[0][i];
        ofile<<" value= "<<setw(4)<<setfill('0')<<color_gradi[0][i];


        if(i >=1 && i < go_color_value -0){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[1][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[1][i-1];
        }
        if(i >=1 && i < go_color_value -1){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[2][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_gradi[2][i-1];
        }

        ofile<<' ';
        for(int j = 0 ; j < color_gradi[0][i];j++){
            ofile<<'*';
        }
        if(i >=1 && i < go_color_value -1){
            if(color_gradi[2][i-1] < 0)ofile<<"●";
        }

        ofile<<endl;
        cout<<endl;
    }
    */
}


unsigned char Binary(Mat & dst){  //src：原圖的copy ； dst：會改掉原來傳進來的圖片~~
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
	//cout<<"threshold = "<<threshold<<endl;

    // 二值化
	for(int row = 0 ; row < dst.rows; row++){
		for(int col = 0 ; col < dst.cols ; col++){
			if(dst.at<uchar>(row,col) <= threshold) dst.at<uchar>(row,col) = 0;
			else                                    dst.at<uchar>(row,col) = 255;
		}
	}

	return threshold;
}

void Binary_by_patch(Mat& dst , const int div_row, const int div_col){
    Mat patch_img;
	threshold_map.create(div_row, div_col, CV_8UC1);  // debug用的, 觀察每個patch用什麼threshold
    // 切塊來做 二值化
    const int height = dst.rows;
	// const int div_row = 80;
	const int div_height = height / div_row;
	const int mod_height = height % div_row;

	const int width = dst.cols;
	// const int div_col = 160;
	const int div_width = width / div_col;
	const int mod_width = width % div_col;
	/*
	cout << "    width = " <<     width << "     height = " <<     height << endl;
	cout << "mod_width = " << mod_width << " mod_height = " << mod_height << endl;
	cout << endl;
	*/

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
			cout<< "colRange = "<<width_acc  <<" ~ "<< width_acc_next  << ", " <<
				   "rowRange = "<<height_acc <<" ~ "<< height_acc_next << endl;
			*/

			patch_img = dst.rowRange(height_acc,height_acc_next+1)
					       .colRange( width_acc, width_acc_next+1);
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
	// cout<<endl;
	// imshow("threshold_map",threshold_map);
    // imshow("dst", dst);
    // waitKey(0);
	// cout<< threshold_map <<' '<<endl;

    imwrite("debug_img/pre2-Binarize.bmp", dst);
}




// 想法：
// 我認為的"做二值化的目的"：區分"符號"和"背景"

// 我們是如何判別"符號"和"背景"
	// → 我覺得是看"輪廓"→再利用輪廓"分辨顏色"→利用顏色來區分"符號"和"背景"
	// →反過來也可以~~ 先"分辨顏色"→ 在找出"輪廓"→再來區分"符號"和"背景"
// 但用前者比較方便，因為已經有canny可以很方便的找輪廓了~~
// 所以以下方法是：
	// 先把圖片做canny 找出輪廓
	// 再利用輪廓 找出 符號顏色
    // 但實際用起來效果不是太好, 寫都寫了還是保留一下未來有機會可以再改善, 比如跟上面同理套 patch 切小塊做也許有機會更好
void test_Binary_by_Canny(Mat src){  //src：原圖的copy ； dst：會改掉原來傳進來的圖片~~
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
				// if( (go_col == 0) && (go_row == 0) ) threshold = src.at<uchar>(go_row+1,go_col+1); // 左上
				// else if( (go_col == 0) && (go_row != 0) ) threshold = src.at<uchar>(go_row+1,go_col);//左
				// else if( (go_col == 0) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col);//左下
				// else if( (go_col != width-1) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//下
				// else if( (go_col == width-1) && (go_row == height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//右下
				// else if( (go_col == width-1) && (go_row != height-1) ) threshold = src.at<uchar>(go_row-1,go_col-1);//右
				// else if( (go_col == width-1) && (go_row == 0) ) threshold = src.at<uchar>(go_row,go_col-1);//右上
				// else if( (go_col != 0) && (go_row == 0) ) threshold = src.at<uchar>(go_row+1,go_col);//上
				// else
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
				// cout<<"threshold = "<<threshold<<endl;
				// imshow("test_binary",dst);
				// imshow("threshold_map",threshold_map);
				// waitKey();
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

			// threshold_map.at<uchar>(go_row,go_col) = threshold;
			// cout<<"go_col = "<<go_col;
		}
		// cout<<endl;
	}
	///******************************************************
	imwrite("debug_img/test_Binary_by_Canny.jpg",dst);
	// dst = src.clone();// dst：會改掉原來傳進來的圖片~~所以複製一下二值化好的圖片就可以改道原來的圖囉!!!
}
