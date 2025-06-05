/*
這個程式的功能是：
用來輔助Opencv的工具，可以把找到的所有線顯示在輸入圖上
*/

//2015/11/01 洲暐更新
//更新一：多了Hough_Line_Probabilistic可以使用
//更新二：有先做過排序(多了bubbleSort)再顯示 & output 圖片

#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>

#include "preprocess_0_watch_hough_line.h"

#include "preprocess_0_hough_tool.h"

#define LINE_DIR "lines/"
#define FILE_TYPE ".bmp"
#define PI 3.14159

using namespace cv;
using namespace std;


// gdst = gray_destination
// cdst = color_destination
void Watch_Hough_Line(vector<Vec2f> lines, Mat gdst, string window_name, string file_name)
{
    Mat cdst;
    cvtColor(gdst,cdst, CV_GRAY2BGR);

	// bubbleSort_H_rho(lines.size(),lines);

	for(size_t i = 0; i < lines.size(); i++ )
	{
		cout<<"i = "<<i<<" , ";
		float rho = lines[i][0], theta = lines[i][1];
		double angle_value = (theta/PI)*180;
		cout<<"rho = "<<rho<<" , theta = "<<angle_value<<endl;

		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line( cdst, pt1, pt2, Scalar(0,i,255), 1, CV_AA);
	//	cout<<"pt1.x = "<<pt1.x<<",pt1.y = "<<pt1.y<<" , pt2.x = "<<pt2.x<<"pt2.y = "<<pt2.y<<endl;

	//	imshow( "Hough", cdst );

	}

	// waitKey(0);
	cout<<endl;
	imshow(window_name,cdst);
	// ******************************************************
	// imwrite(file_name + FILE_TYPE,cdst);
	// imwrite(window_name + ".jpg",cdst);
}


void Watch_Hough_Line_Shift(vector<Vec4f> lines, Mat gdst, string window_name, string file_name){
    Mat cdst;
    cvtColor(gdst,cdst, CV_GRAY2BGR);


	for(size_t i = 0; i < lines.size(); i++ ){
		cout<<"i = "<<i<<" , ";
		float rho = lines[i][2], theta = lines[i][3];
		double angle_value = (theta/PI)*180;

		int shift_x = lines[i][0];
        int shift_y = lines[i][1];
		cout<<"rho = "<<rho<<" , theta = "<<angle_value<<" , shift_x = "<<shift_x<<" , shift_y = "<<shift_y<<endl;



		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho + shift_x, y0 = b*rho + shift_y;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line( cdst, pt1, pt2, Scalar(0,i,255), 1, CV_AA);

	}

	cout<<endl;
	imshow(window_name,cdst);
	// ******************************************************
	// imwrite(file_name + FILE_TYPE,cdst);

}

/*
void Watch_Hough_Line_Shift_Staff(vector<Vec5f> lines , Mat gdst,string window_name,string file_name){
    Mat cdst;
    cvtColor(gdst,cdst, CV_GRAY2BGR);


	for(size_t i = 0; i < lines.size(); i++ ){
		cout<<"i = "<<i<<" , ";


		float rho = lines[i][3], theta = lines[i][4];
		double angle_value = (theta/PI)*180;

		int shift_x = lines[i][1];
        int shift_y = lines[i][2];
		cout<<"staff_num = "<<lines[0]<<" , rho = "<<rho<<" , theta = "<<angle_value<<" , shift_x = "<<shift_x<<" , shift_y = "<<shift_y<<endl;



		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho + shift_x, y0 = b*rho + shift_y;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line( cdst, pt1, pt2, Scalar(0,i,255), 2, CV_AA);

	}

	cout<<endl;
	imshow(window_name,cdst);
	imwrite(file_name + FILE_TYPE,cdst);

}
*/
void Watch_Hough_roh(vector<Vec2f> lines , Mat& dst){
	Mat cdst = dst.clone();
	for(size_t i = 0 ; i < lines.size();i++){
		if(lines[i][0] > 0){
		//想要看什麼訊息就把註解拿掉
		// cout<<"i = "<<i<<" , line at y    = "<<lines[i][0]<<" , value = "<<(int)cdst.at<uchar>(lines[i][0] ,cdst.cols/2 )<<endl;
		// cout<<"i = "<<i<<" , line at y -1 = "<<lines[i][0]-1<<" , value = "<<(int)cdst.at<uchar>(lines[i][0]-1 ,cdst.cols/2 )<<endl;
		// cout<<"i = "<<i<<" , line at y -2 = "<<lines[i][0]+1<<" , value = "<<(int)cdst.at<uchar>(lines[i][0]-2 ,cdst.cols/2 )<<endl;
		cdst.at<uchar>(lines[i][0] ,cdst.cols/2 ) = 50;
		// cdst.at<uchar>(lines[i][0] ,cdst.cols/2 -1 ) = 50;
		// cdst.at<uchar>(lines[i][0] ,cdst.cols/2 +1 ) = 50;
		// cdst.at<uchar>(lines[i][0] -1 ,cdst.cols/2 ) = 50;
		// cdst.at<uchar>(lines[i][0] +1 ,cdst.cols/2 ) = 50;
		}
	}
	imshow("Hough Point",cdst);
}


//void Watch_Hough_P_Line(vector<Vec4i> lines , Mat& cdst,string window_name)
void Watch_Hough_P_Line(vector<Vec4i> lines , Mat gdst,string window_name,string file_name){
    Mat cdst;
    cvtColor(gdst,cdst,CV_GRAY2BGR);
    /*
	Mat dst = cdst.clone();
	Mat gray_dst;
	cvtColor(dst,gray_dst,CV_BGR2GRAY);
	*/
	// bubbleSort_H_P_Y(lines.size(),lines);
	for(size_t i = 0 ; i < lines.size(); i++){
		// cout<<"i = "<<i<<" , ";
		Vec4i l = lines[i];
		int point1_value = gdst.at<uchar>(l[1],l[0]);
		int point2_value = gdst.at<uchar>(l[3],l[2]);
		// cout<<"x = "<<l[0]<<" , "<<"y = "<<l[1]<<" , value = "<<point1_value<<" , "
		// 	<<"x = "<<l[2]<<" , "<<"y = "<<l[3]<<" , value = "<<point2_value<<endl;
		line(cdst,Point( l[0], l[1] ), Point( l[2], l[3]) , Scalar(186,88,255),1,CV_AA);
	}
	cout<<endl;
	// cdst = dst.clone();
	imshow(window_name,cdst);
	// *****************************************
	// imwrite(file_name + FILE_TYPE,cdst);
	// imwrite(window_name +".jpg",dst);
}
