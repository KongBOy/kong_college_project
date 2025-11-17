/*
這個程式的功能：
可以讓使用者自由擷取輸入圖的寬度(由中間為基準)
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Recognition.h"
#include "preprocess_3_center_roi.h"
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////
#define BAR_WINDOW "~ a lot of track bar ~"
#define BAR_WINDOW_WIDTH 300
#define BAR_WINDOW_HEIGHT 300
//////////////////////////////////////////////////////
#define ROI_ROW_BAR "ROW"
#define ROI_COL_BAR "COL"
#define ROI_R_SLIDER_DEFAULT 100
#define ROI_C_SLIDER_DEFAULT 10
//////////////////////////////////////////////////////
#define ENTER 13
#define ESC 27
#define BROWSE_WINDOW "browse_img"
///////////////////////////////////////////////////////

using namespace cv;

void Recognition_page::Center_ROI(int, void * data){
	Recognition_page* self_ptr = static_cast<Recognition_page*>(data);
	Recognition_page& self     = *self_ptr;

	// 中間為基準, 左右 與 上下 延伸 幾% 來取影像
	if ((self.roi_c_slider != 0) && (self.roi_r_slider != 0) ) {
		Point2f center(self.src_bin_roi.cols / 2, self.src_bin_roi.rows / 2);
		float roi_width = self.src_bin_roi.cols * self.roi_c_slider / 100;
		float roi_heigh = self.src_bin_roi.rows * self.roi_r_slider / 100;

		self.slider_result_img = self.src_bin_roi(Rect(center.x - roi_width / 2., center.y - roi_heigh / 2., roi_width, roi_heigh));
	}
	
	if(self.debuging_pre3a){
		// 不論圖多大都按比例縮放成 高度 650px 來顯示
		Mat browse_img;
		resize(self.slider_result_img, browse_img, Size(self.slider_result_img.cols * (650.0 / self.slider_result_img.rows), self.slider_result_img.rows * (650.0 / self.slider_result_img.rows)));
		imshow(BROWSE_WINDOW, browse_img);
		cout << "self.src_bin_roi.cols = " << self.src_bin_roi.cols << ", self.src_bin_roi.rows = " << self.src_bin_roi.rows << endl;
		cout << "self.slider_result_img   .cols = " << self.slider_result_img   .cols << ", self.slider_result_img   .rows = " << self.slider_result_img   .rows << endl;
		cout << "browse_img.cols = " << browse_img.cols << ", browse_img.rows = " << browse_img.rows << endl;
		cout << endl;
	}

}

void Recognition_page::Center_ROI_by_slider(string window_name){
	debuging = debuging_pre3a;
	if(debuging){
		Mat debug_img = src_bin_roi.clone();
		Center_ROI(0, (void *)this);
		// 建立 滑桿視窗
		namedWindow   (BAR_WINDOW, WINDOW_AUTOSIZE);
		cvMoveWindow  (BAR_WINDOW, 0, 0);
		cvResizeWindow(BAR_WINDOW, BAR_WINDOW_WIDTH, BAR_WINDOW_HEIGHT);
		
		// 建立 預覽視窗
		namedWindow   (BROWSE_WINDOW , WINDOW_AUTOSIZE);
		cvMoveWindow  (BROWSE_WINDOW , BAR_WINDOW_WIDTH + 20, 0);
		
		// 建立滑桿視窗 和 連結相關 滑桿變數, 滑桿function
		createTrackbar(ROI_ROW_BAR, BAR_WINDOW, &roi_r_slider, 100, Center_ROI, (void *)this); //如果用local的丟資料，要強制轉換(void*)&ord_img
		createTrackbar(ROI_COL_BAR, BAR_WINDOW, &roi_c_slider, 100, Center_ROI, (void *)this);
	
		// 按 ESC 或 ENTER 結束預覽
		unsigned char keyboard;
		do{
			keyboard = waitKey();
		}
		while ( (keyboard != ESC) && (keyboard != ENTER));
		// 關閉視窗
	}
	
	Center_ROI(0, (void *)this);

	src_bin_roi = slider_result_img.clone();
	if(debuging) imwrite("debug_img/pre3_HorizL_1_Center_ROI.jpg", src_bin_roi);

	destroyWindow(BAR_WINDOW);
	destroyWindow(BROWSE_WINDOW);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Mat proc_img;
static bool debuging_roi;  // 給 Center_ROI 用的

int roi_r_slider = ROI_R_SLIDER_DEFAULT;
int roi_c_slider = ROI_C_SLIDER_DEFAULT;


void Center_ROI(int, void * data){
	Mat* dst_img_ptr = static_cast<Mat*>(data);
	Mat& dst_img     = *dst_img_ptr;

	// 中間為基準, 左右 與 上下 延伸 幾% 來取影像
	if ((roi_c_slider != 0) && (roi_r_slider != 0) ) {
		Point2f center(dst_img.cols / 2, dst_img.rows / 2);
		float roi_width = dst_img.cols * roi_c_slider / 100;
		float roi_heigh = dst_img.rows * roi_r_slider / 100;

		proc_img = dst_img(Rect(center.x - roi_width / 2., center.y - roi_heigh / 2., roi_width, roi_heigh));
	}
	
	if(debuging_roi){
		Mat browse_img;
		resize(proc_img, browse_img, Size(proc_img.cols * (650.0 / proc_img.rows), proc_img.rows * (650.0 / proc_img.rows)));
		imshow(BROWSE_WINDOW, browse_img);
		cout << "dst_img   .cols = " << dst_img   .cols << ", dst_img   .rows = " << dst_img   .rows << endl;
		cout << "proc_img  .cols = " << proc_img  .cols << ", proc_img  .rows = " << proc_img  .rows << endl;
		cout << "browse_img.cols = " << browse_img.cols << ", browse_img.rows = " << browse_img.rows << endl;
		cout << endl;
	}

}

void Center_ROI_by_slider(Mat & dst_img , string window_name, bool debuging){
	debuging_roi = debuging;  // 給 Center_ROI 用的

	if(debuging_roi){
		Mat debug_img = dst_img.clone();
		Center_ROI(0, (void *)&debug_img);
		// 建立 滑桿視窗
		namedWindow   (BAR_WINDOW, WINDOW_AUTOSIZE);
		cvMoveWindow  (BAR_WINDOW, 0, 0);
		cvResizeWindow(BAR_WINDOW, BAR_WINDOW_WIDTH, BAR_WINDOW_HEIGHT);
		
		// 建立 預覽視窗
		namedWindow   (BROWSE_WINDOW , WINDOW_AUTOSIZE);
		cvMoveWindow  (BROWSE_WINDOW , BAR_WINDOW_WIDTH + 20, 0);
		
		// 建立滑桿視窗 和 連結相關 滑桿變數, 滑桿function
		createTrackbar(ROI_ROW_BAR, BAR_WINDOW, &roi_r_slider, 100, Center_ROI, (void *)&debug_img); //如果用local的丟資料，要強制轉換(void*)&ord_img
		createTrackbar(ROI_COL_BAR, BAR_WINDOW, &roi_c_slider, 100, Center_ROI, (void *)&debug_img);
	
		// 按 ESC 或 ENTER 結束預覽
		unsigned char keyboard;
		do{
			keyboard = waitKey();
		}
		while ( (keyboard != ESC) && (keyboard != ENTER));
		// 關閉視窗
	}
	
	Center_ROI(0, (void *)&dst_img);

	dst_img = proc_img.clone();
	if(debuging_roi) imwrite("debug_img/pre3_HorizL_1_Center_ROI.jpg",dst_img);

	destroyWindow(BAR_WINDOW);
	destroyWindow(BROWSE_WINDOW);
}
