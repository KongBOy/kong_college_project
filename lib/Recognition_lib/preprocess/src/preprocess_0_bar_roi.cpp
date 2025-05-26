/*
這個程式的功能：
可以讓使用者自由擷取輸入圖的寬度(由中間為基準)
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "preprocess_0_bar_roi.h"

///////////////////////////////////////////////////////
#define BAR_WINDOW "~ a lot of track bar ~"
#define BAR_WINDOW_WIDTH 300
#define BAR_WINDOW_HEIGHT 300
//////////////////////////////////////////////////////
#define ROI_ROW_BAR "【ROW】"
#define ROI_COL_BAR "【COL】"
#define ROI_R_SLIDER_DEFAULT 100
#define ROI_C_SLIDER_DEFAULT 10
//////////////////////////////////////////////////////
#define ENTER 13
#define ESC 27
#define SRC_IMG_WINDOW "src_img"
#define BROWSE_WINDOW "browse_img"
///////////////////////////////////////////////////////

using namespace cv;

static Mat src_img;
static Mat proc_img;

int roi_r_slider = ROI_R_SLIDER_DEFAULT;
int roi_c_slider = ROI_C_SLIDER_DEFAULT;


void on_Roi(int, void *)
{
    //Mat proc_img = (*(Mat*)in_data).clone();//如果接local資料，先強制轉換，在"*"，在整個括號才能".??()"

//◎Step：做處理
	if ((roi_c_slider != 0) && (roi_r_slider != 0) )//&& ( (roi_c_slider != ROI_C_SLIDER_DEFAULT) || (roi_r_slider != ROI_R_SLIDER_DEFAULT) ))
	{
		Point2f center(src_img.cols / 2, src_img.rows / 2);
		float roi_width = src_img.cols * roi_c_slider / 100;
		float roi_heigh = src_img.rows * roi_r_slider / 100;

		proc_img = src_img(Rect(center.x - roi_width / 2, center.y - roi_heigh / 2, roi_width, roi_heigh));
		//cout<<center.x - roi_width / 2<<endl;
		//左上 往下延伸
		//  roi_proc_img = ord_img(Rect(0,0,roi_width,roi_heigh));
		//左中 往上下延伸
		//  roi_proc_img = ord_img(Rect(0,ord_img.rows/2 - roi_heigh/2,roi_width,roi_heigh));
	//	cout<<"finish roi"<<endl;
		///*****************************************************************
		///imshow(BROWSE_WINDOW, proc_img);  //◎Step SHOW-1：做完處理的結果show出來

	}///*****************************************************************
	///else imshow(BROWSE_WINDOW, proc_img);  //◎Step SHOW-1：做完處理的結果show出來
}

void bar_Roi(Mat & in_src_img , string window_name)
{
    namedWindow(SRC_IMG_WINDOW, WINDOW_AUTOSIZE);
	// moveWindow(SRC_IMG_WINDOW, 0, BAR_WINDOW_HEIGHT + 30);
	namedWindow(BROWSE_WINDOW, WINDOW_AUTOSIZE);
	// moveWindow(BROWSE_WINDOW, BAR_WINDOW_WIDTH + 20, 0);

    namedWindow(BAR_WINDOW, WINDOW_AUTOSIZE);
	//resizeWindow(BAR_WINDOW, BAR_WINDOW_WIDTH, BAR_WINDOW_HEIGHT);
	// moveWindow(BAR_WINDOW, 0, 0);

	createTrackbar(ROI_ROW_BAR, BAR_WINDOW, &roi_r_slider, 100, on_Roi);//如果用local的丟資料，要強制轉換(void*)&ord_img
	createTrackbar(ROI_COL_BAR, BAR_WINDOW, &roi_c_slider, 100, on_Roi);

    src_img = in_src_img.clone();
///*****************************************************************
///    imshow(SRC_IMG_WINDOW, src_img);

    proc_img = src_img.clone();
    on_Roi(0,0);

///*****************************************************************
/*	unsigned char keyboard;
	do
    {
        keyboard = waitKey();
    }
	while ( (keyboard != ESC) && (keyboard != ENTER));
*/
	destroyWindow(BAR_WINDOW);
    destroyWindow(SRC_IMG_WINDOW);
    destroyWindow(BROWSE_WINDOW);

	in_src_img = proc_img.clone();
///******************************************************
///	imwrite(window_name + ".bmp",in_src_img);
//	return;
}
