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

#include "preprocess_0_watch_hough_line.h"
#include "preprocess_1_warp_straight_rough.h"
#include "preprocess_2_binary.h"
//Watch_Hough_Line(lines,cdst);

#define PI 3.14159

using namespace cv;
using namespace std;



void bubbleSort(int amount,vector<Vec2f> &list/*,Mat cdst*/){
	for(int i = 0 ; i < amount-1 ; i++){
		for(int j = i+1 ; j < amount ; j++){
			if(list[i][1] > list[j][1]){
				double temp0 = list[i][0];   double temp1 = list[i][1];
				list[i][0]   = list[j][0];    list[i][1]  = list[j][1];
				list[j][0]   = temp0     ;    list[j][1]  = temp1;
			}
		}
	}
}

double Find_Angle(Mat image)
{
	Mat imgROI = image(Rect( (image.cols/2)-(image.cols/16), (image.rows/2)-(image.rows/16),(image.cols/8),(image.rows)/8) );
	Mat bin_imgROI = imgROI.clone();
	Binary(imgROI,bin_imgROI,15,50);
///******************************************************
///    imwrite("2016 03 29 轉正.bmp",imgROI);
///    imshow("Find_Angle",bin_imgROI);
///    waitKey(0);
    bin_imgROI = ~bin_imgROI;

//	imshow("ROI",bin_imgROI);
//	cout<<"img.cols/3 = "<<imgROI.cols<<endl;
//	waitKey();

// /**/	imshow("imgROI",imgROI);


	vector<Vec2f> lines;
	do_HoughLine(bin_imgROI,lines,0.5);
///	cout<<"lines.size() = "<<lines.size()<<endl;
	///////////////////// 以下是要找出最多的線 ///////////////////
	///////////// 先排序/////////////////
	bubbleSort(lines.size(),lines);

/// Watch_Hough_Line(lines,imgROI,"ppt_window" ,"ppt");
/// waitKey(0);
	//////////// 數字統計的array設定 ///////////
	double * most_angle = new double[lines.size()];//initial
	int * count = new int[lines.size()];//initial
	for(int i = 0 ; i < lines.size(); i++)//initial
	{
		most_angle[i] = 0;//initial
		count[i] = 0;//initial
	}

	most_angle[0] = lines[0][1]; //initial
	count[0] = 1;//initial
	int ptr = 1;//initial
	for(int i = 0 ; i < lines.size() - 1;i++)
	{
		if(lines[i][1] - lines[i+1][1] != 0)
		{
			most_angle[ptr] = lines[i][1];
			count[ptr]++;
			ptr++;
		}
		else count[ptr]++;
	}

	/////// 找最多的位置 ////////
	double max = 0; ///////////////////因為我這個變數給很多人用，其中有用到double所以不設int喔!!!
	for(int i = 0 ; i < ptr;i++)  if(max < count[i]) max = count[i];
//	cout<<"most max = "<<max<<endl;

	////// 找最多的位置對應的是誰 ////
	for(int posi = 0; posi < ptr;posi++)
	{
		if(count[posi] == max)
		{
			max = most_angle[posi];
			break;
		}
	}

//	Watch_Hough_Line(lines,cdst);
//	cout<<"most max is who = "<<max<<endl;
	delete [] most_angle;
	delete [] count;

	return (max/PI)*180; //現在是徑度量喔~~~~我習慣度度量且我的Wrap_Straight也是用度度量，所以轉一下再return
}

void Wrap_Straight(Mat & image , double angle ){
		double rotate_angle = angle - 90;
		cout<<"rotate_angle = "<<rotate_angle<<endl;
		Point center = Point(image.cols/2,image.rows/2); // the center of the picture

		Mat rot(2,3,CV_64FC1);
		rot = getRotationMatrix2D(center,rotate_angle,1);//produce wrapAffine_matrix

	// imshow("before_warp",image);
        imwrite("debug_preprocess/before_warp.bmp",image);
	// cout<<"before_img.cols = "<<image.cols<<" , rows = "<<image.rows<<endl;
	// waitKey(0);

        Mat proc_img = image.clone();
        if(image.rows < image.cols) resize(proc_img,proc_img,Size(image.cols,image.cols),0,0, INTER_CUBIC);
	warpAffine(image, proc_img, rot, proc_img.size(), 0, 0, 255);
		image = proc_img.clone();

	// imshow("after_warp",image);
		imwrite("debug_preprocess/after_warp.bmp",image);
	// cout<<"after_img.cols = "<<image.cols<<" , rows = "<<image.rows<<endl;
	// waitKey(0);

	//imwrite寫出去!!!寫裡面起天後自己就忘記了!!!!
	//******************************************************
	// imwrite("Wrap_Straight.jpg",image);
		//waitKey();
}

void do_HoughLine(Mat image,vector<Vec2f> & lines,float threshold){
	// threshold 用成 cols 的比例 可以比較好套用到 不同寬度的影像上

	// 不一定要用Canny, 其實只要把圖片二值化就可以用 HoughLines 囉
	// Mat dst,cdst;
	// Canny(image, dst, 50, 100, 3);
	// cvtColor(dst,cdst, CV_GRAY2BGR);
    // imshow("canny",cdst);

	HoughLines(image, lines, 1, CV_PI/180, image.cols * threshold/*175*/, 0, 0 );   //0.2是測試很多次覺得可以的值
}

