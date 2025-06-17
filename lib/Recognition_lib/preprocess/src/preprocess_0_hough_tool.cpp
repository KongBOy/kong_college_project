/*
這個程式的功能是：
用來輔助Opencv的工具，可以把找到的所有線依據rho來排序
*/
//#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include "preprocess_0_hough_tool.h"
using namespace cv;

void bubbleSort_H_rho(vector<Vec2f> &list){
	int amount = list.size();
	for(int i = 0 ; i < amount-1 ; i++)
	{
		for(int j = i+1 ; j < amount ; j++)
		{
			if(list[i][0] > list[j][0])
			{
				////////////// swap ///////////////
				double temp0 = list[i][0];   double temp1 = list[i][1];
				list[i][0] = list[j][0];  list[i][1] = list[j][1];
				list[j][0] = temp0;        list[j][1] = temp1; 
			}
		}
	}
}

void bubbleSort_H_P_Y(vector<Vec4i> &list){
	int amount = list.size();
	for(int i = 0 ; i < amount-1 ; i++)
	{
		for(int j = i+1 ; j < amount ; j++)
		{
			if(list[i][1] > list[j][1])
			{
				////////////// swap ///////////////
				Vec4i l = list[i];
				for(int col = 0 ; col < 4 ; col++)
				{
					list[i][col] = list[j][col];
					list[j][col] = l[col];
				}
				/*
				list[i][0] = list[j][0];
				list[i][1] = list[j][1];
				list[i][2] = list[j][2];
				list[i][3] = list[j][3];

				list[j][1] = l[0];
				list[j][1] = l[1];
				list[j][2] = l[2];
				list[j][3] = l[3];
				*/
			}
		}
	}
}
