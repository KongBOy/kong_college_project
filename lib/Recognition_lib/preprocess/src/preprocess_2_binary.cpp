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

//給一個譜的灰階圖近來，可以做漂亮的二值化
//想法：
//我認為的"做二值化的目的"：區分"符號"和"背景"

//我們是如何判別"符號"和"背景"
	//→ 我覺得是看"輪廓"→再利用輪廓"分辨顏色"→利用顏色來區分"符號"和"背景"
	//→反過來也可以~~ 先"分辨顏色"→ 在找出"輪廓"→再來區分"符號"和"背景"
//但用前者比較方便，因為已經有canny可以很方便的找輪廓了~~
//所以以下方法是：
	//先把圖片做canny 找出輪廓
	//再利用輪廓 找出 符號顏色

//threshold = 找到的note顏色

Mat threshold_map; //把所有的threshold存起來的容器，debug用
static int go_row = 0; //在Mat裡面跑的座標
static int go_col = 0; //在Mat裡面跑的座標

static Mat temp_bin;

const int exp_color = 40;  //自己測試了很多圖大概估測note的顏色大概在30左右


//方法一：速度比方法二快，

void Binary(Mat src, Mat& dst , const int div_row,const int div_col)
{
	threshold_map.create(div_row, div_col, CV_8UC1);
	/////////////////// 試試看 二質化 //////////////////////////
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

/*
#define ENTER 13
#define ESC 27

#define BROWSE_WINDOW "browse_img"

#define BAR_WINDOW "~ a lot of track bar ~"
#define BAR_WINDOW_WIDTH 300
#define BAR_WINDOW_HEIGHT 300

#define CANNY_LOW_BAR "【Canny_l】"
#define CANNY_HIGH_BAR "【Canny_h】"
#define CANNY_L_SLIDER_DEFAULT 0
#define CANNY_H_SLIDER_DEFAULT 0
int canny_l_slider;
int canny_h_slider;
Mat temp_src_img;
Mat temp_proc_img;
void on_Cannyy(int,void *)
{
        Mat temp_proc_img2 = temp_src_img.clone();
		Canny(temp_proc_img2,temp_proc_img,canny_l_slider,canny_h_slider,3);
		imshow(BROWSE_WINDOW,temp_proc_img);
		cout<<"do canny"<<endl;

}
*/

unsigned char Binary(Mat src, Mat & dst)  //src：原圖的copy ； dst：會改掉原來傳進來的圖片~~
{
	/////////////////////////////// 步驟一 /////////////////////////
	Mat temp_dst;
	//Canny(src,temp_dst,25,75,3);
    int color[256];
    int color_count[256];
    int color_value[3][256];
    int color_count_1[256];
    int color_count_2[256];
    for(int i = 0 ; i < 256 ; i++){
        color[i] = 0;
        color_count[i] = 0;
        color_count_1[i] = 0;
        color_count_2[i] = 0;
    }
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 256 ; j++){
             color_value[i][j] = 0;
        }
    }

    int color_high_range = 0;
    int color_low_range  = 0;
    int local_max[256];

    for(int i = 0 ; i < src.rows ; i++){
        for(int j = 0 ; j < src.cols ; j++){
            color_count[src.at<uchar>(i,j)]++;
        }
    }

    int last_posi = 0;
    int color_range_count = 0;
    int color_acc_count[256];
    int area = src.cols * src.rows;
    int threshold = 0;
    int threshold_acc = 0;

    for(int i = 0 ; i < 256 ; i++){
        if(color_count[i]){
            color[color_range_count] = i;
            color_value[0][color_range_count]= color_count[i];
            color_range_count++;

            color_high_range = i;
        }


        if(color_count[255-i]){
            color_low_range = 255-i;
        }

    }


    for(int level = 1 ; level < 3 ; level++){
        for(int i = 0 ; i < color_range_count-level ; i++){
            color_value[level][i] = color_value[level-1][i+1] - color_value[level-1][i];
        }
    }

    /*
    ////顯示資料
    ofstream ofile("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_star_data.txt",ios::trunc);

    for(int i = 0 ; i < color_range_count ;i++){
        cout<<"i = "<<color[i];
        ofile<<"i = "<<color[i];
        cout<<" value= "<<setw(4)<<setfill('0')<<color_value[0][i];
        ofile<<" value= "<<setw(4)<<setfill('0')<<color_value[0][i];


        if(i >=1 && i < color_range_count -0){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_value[1][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_value[1][i-1];
        }
        if(i >=1 && i < color_range_count -1){
            cout<<" ,dir= "<<setw(4)<<setfill('0')<<color_value[2][i-1];
            ofile<<" ,dir= "<<setw(4)<<setfill('0')<<color_value[2][i-1];
        }


        ofile<<' ';
        for(int j = 0 ; j < color_value[0][i];j++){
            ofile<<'*';
        }
        if(i >=1 && i < color_range_count -1){
            if(color_value[2][i-1] < 0)ofile<<"●";
        }

        ofile<<endl;

        cout<<endl;
    }

    */
    ////累積
    int threshold_posi = 0;
    float area_rate = 0.50;

    int threshold2 = 0;
    int threshold_posi2 = 0;
    float area_rate2 = 0.03125;

    for(int i = 0 ; i < color_range_count ; i++){
        if(i== 0) color_acc_count[0] = color_value[0][0];
        else{
            color_acc_count[i] = color_value[0][i] + color_acc_count[i-1];
            if(color_acc_count[i] <area * area_rate){
                threshold = color[i];
                threshold_posi = i;
            }

            if(color_acc_count[i] <area * area_rate2){
                threshold2 = color[i];
                threshold_posi2 = i;
            }
        }
    }


    for(int i = 0 ; i < color_range_count ; i++){
        if(i== 0) color_acc_count[0] = color_value[0][0];
        else{
            color_acc_count[i] = color_value[0][i] + color_acc_count[i-1];

        }
    }


    //ofile.close();


    /* 
    for(int i = 0 ; i < 256; i++){
        if(i == 0){
            color_acc_count[0] = color_count[0];
        }
        else{
            color_acc_count[i] = color_count[i] + color_acc_count[i-1];
            if(color_acc_count[i] < area/2){
                threshold = i - 30;
                threshold_acc = color_acc_count[i];
            }
        }

        if(color_count[i]){

            cout<<"i = "<<i<<" value = "<<color_count[i];


            if(color_range_count == 0) cout<<endl;


            if(color_range_count >0){
                color_count_1[last_posi] = color_count[i] - color_count[last_posi];
                cout<<"  dir1["<<last_posi<<"] = "<<color_count_1[last_posi];
            }


            if(color_range_count  == 1 ) cout<<endl;



            if(color_range_count >1){
                color_count_2[last_posi] = color_count_1[i] - last_color_count_1;
                cout<<"  dir2["<<last_posi<<"] = "<<color_count_2[last_posi]<<endl;
                last_color_count_1 = color_count_1[last_posi];
            }

            color_high_range = i;
            last_posi = i;
            color_range_count++;
        }

        if(color_count[255-i]){
            color_low_range = 255-i;
        }
    }
    */

    int shift2 = 30;
    /*
    cout<<endl;

    cout<<"src.cols = "<<src.cols<<" src.rows = "<<src.rows<<" area = "<<area<<endl;
    cout<<"area1 = "<<area * area_rate<<endl;
    cout<<"area2 = "<<area * area_rate2<<endl;
    cout<<endl;
    cout<<"ord_acc = "<<color_acc_count[threshold_posi]<<endl;
    cout<<"ord_per = "<<(float)color_acc_count[threshold_posi]/area<<endl;
    cout<<"ord_threshold = "<<threshold<<endl<<endl;

    cout<<"ord2_acc = "<<color_acc_count[threshold_posi2]<<endl;
    cout<<"ord2_per = "<<(float)color_acc_count[threshold_posi2]/area<<endl;
    cout<<"ord2_threshold = "<<threshold2<<endl<<endl;

    cout<<"new_acc = "<<color_acc_count[(threshold_posi+threshold_posi2)/2 - shift2]<<endl;
    cout<<"new_per = "<<(float)color_acc_count[(threshold_posi+threshold_posi2)/2 - shift2]/area<<endl;
    cout<<"new_threshold = "<<(threshold+threshold2)/2 - shift2<<endl;
    cout<<endl;

    cout<<"color_low_range  = "<<color_low_range <<endl;
    cout<<"color_high_range = "<<color_high_range<<endl;
    cout<<"color_range      = "<<color_high_range - color_low_range<<endl;
    cout<<"end~~~~~~~"<<endl<<endl;
    */

    // threshold = threshold2;
    // threshold = ((float)threshold*0.5 +(float)threshold2*0.5);
    threshold -= shift2;
    // imshow("src_img",src);


    // waitKey(0);
    // return 0;

    /*
    Mat temp_src_img = src.clone();

	ofstream ofile("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_src_data.txt",ios::trunc);
	if(ofile.is_open()){
        for(int go_row = 0 ; go_row < src.rows ; go_row++){
            for(int go_col = 0 ; go_col < src.cols ; go_col++){
                 ofile<<setw(3)<<setfill('0')<<(int)src.at<uchar>(go_row,go_col)<<' ';
            }
            ofile<<endl;
        }
        cout<<"mat is stored in src_txt"<<endl;
        // ofile<<setw(3)<<setfill('0')<<src<<' '<<endl;

    }
    else cout<<"開檔失敗"<<endl;
    ofile.close();
    imwrite((string)BIN_DIR + "src_txt_img.bmp",src);
    imshow("debug_src",src);

    ofile.open("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_can_data.txt",ios::trunc);
    if(ofile.is_open())
    {
        for(int go_row = 0 ; go_row < src.rows ; go_row++)
        {
            for(int go_col = 0 ; go_col < src.cols ; go_col++)
            {
                 ofile<<setw(3)<<setfill('0')<<(int)temp_dst.at<uchar>(go_row,go_col)<<' ';
            }
            ofile<<endl;
        }
        cout<<"mat is stored in can_txt"<<endl;
        // ofile<<setw(3)<<setfill('0')<<temp_dst<<' '<<endl;
    }
    else cout<<"開檔失敗"<<endl;
    ofile.close();
    imwrite((string)BIN_DIR + "can_txt_img.bmp",temp_dst);
	imshow("debug_can",temp_dst);
	// waitKey();

    */

    /*
	/////////////////////////  步驟二 //////////////////////////
	int color_hist[255] = {0}; //統計canny格子附近較深的顏色，較深的顏色就是五線譜、豆芽菜之類的囉!!!
	int edge_point = 0; //統計有幾格canny的格子

	///// 對 x y方向看，所以看該點的 左上 右下
	for(int row = 1 ; row < temp_dst.rows - 1 ; row++) //因為要 當格的上下一格做運算 所以 +-1 控制不會超過圖片range
	{
		for(int col = 1 ; col < temp_dst.cols - 1 ; col++)
		{
			////////////////////// 如果有輪廓的話 → edge_point的值會 > 0 ////////////////////////
			if(temp_dst.at<uchar>(row,col))  //canny出來的有值得話，代表該點為edge(周邊顏色變動很大)
			{
				if( (src.at<uchar>(row-1,col-1) - src.at<uchar>(row+1,col+1)) > 0)  //把附近較深的顏色記錄下來
						color_hist[src.at<uchar>(row+1,col+1)]++;
				else color_hist[src.at<uchar>(row-1,col-1)]++;
				edge_point++;
			}
		}
	}

    // cout<<src.cols<<endl;
    // cout<<"edge_point = "<<edge_point<<endl;
    // imshow("debug_can",temp_dst);
	// waitKey();

	float acc_color_hist[255]={0};
	int temp_posi = 0;
	int threshold = 0;
	int decrease_point = 0; //統計直條圖的頂點，就是做多的點(微分開始往下掉的前一點 且 本身的值還要有一定的數量 約 2%)

	//////////////////// 如果有輪廓的話 ///////////////////////////
	if(edge_point > src.cols*3)	{
		for(int i = 0 ; i < 255 ; i++){
			if(color_hist[i]){
                // cout<<"color_hist["<<i<<"] = "<<color_hist[i];//////////
				acc_color_hist[i] = (float)color_hist[i]/edge_point * 100;
                // cout<<"  percent_hist["<<i<<"] = "<<acc_color_hist[i];/////////////////
				if( (color_hist[i] - color_hist[temp_posi] <= 0)){
					 if(color_hist[temp_posi] > color_hist[decrease_point] ){
                        decrease_point = temp_posi;
                        // threshold = decrease_point;
                        // cout<<"  in_decrease_point = "<< decrease_point;
                     }
				}

				acc_color_hist[i] += acc_color_hist[temp_posi];
                // cout<<"  acc_hist["<<i<<"] = "<<acc_color_hist[i]<<endl;/////////////

				//// 自動找出threshold，測試後覺得在頂點後面點比較好，因為note的邊框的顏色較淺！
				if(i >=1){
                    // if( ((acc_color_hist[i] - acc_color_hist[temp_posi]) > 3) ) threshold = i;
				}
				if(acc_color_hist[i] <= 70) threshold = i;

				temp_posi = i;
			}
		}

        threshold = (cvRound((float)threshold *1.0 + (float)decrease_point*0.0))*0.8;

        threshold += 15;

		cout<<"threshold = "<<threshold<<endl;
		cout<<"decrease_point = "<< decrease_point<<endl;
		cout<<endl;//////////////////
	}
	////////////////////////// 如果沒有輪廓的話 ////////////////////
	//可能一：切的roi剛剛好是四分音符裡面的實心部分 或者 剛剛好就是橫的線之類的
	//可能二：背景
	//→判斷"周圍"的 threshold值
	//因為是"周圍"，其實正常的流程應該要整張圖先做完一次取得所有的threshold，然後再重新Scan一次圖來判斷"周圍"
	//但是太麻煩了，所以直接用目前有的周圍→左邊 和 上邊，來猜這格的note顏色大概是怎麼樣子，測試過後覺得OK！
	else //edge_point == 0{
		if( (go_col == 0) && (go_row == 0) ){
			if(src.at<uchar>(src.rows/2,src.cols/2) < exp_color) threshold = src.at<uchar>(src.rows/2,src.cols/2)+10;
			else threshold = exp_color;
		}
		else if( (go_col != 0) && (go_row == 0)) threshold = threshold_map.at<uchar>(go_row,go_col-1);
		else if( (go_col == 0) && (go_row != 0)) threshold = threshold_map.at<uchar>(go_row-1,go_col);
		else{
			threshold = threshold_map.at<uchar>(go_row-1,go_col) * 0.2;
			threshold += (threshold_map.at<uchar>(go_row,go_col-1)*0.8);
		}
	}
    */
	const int shift = 0;
	if(threshold >= shift) threshold -= shift ; else threshold = 0;

	//cout<<"threshold = "<<threshold<<endl;

	for(int row = 0 ; row < src.rows; row++){
		for(int col = 0 ; col < src.cols ; col++){
			if(src.at<uchar>(row,col) <= threshold) src.at<uchar>(row,col) = 0;
			else src.at<uchar>(row,col) = 255;
		}
	}

	dst = src.clone();// dst：會改掉原來傳進來的圖片~~所以複製一下二值化好的圖片就可以改道原來的圖囉!!!
    //~~    imshow("bin_img",dst);
    // waitKey(0);

    // waitKey(0);
    /*
    ofile.open("C:\\Users\\may\\Desktop\\winter_week2\\1-Binary\\Mat_bin_data.txt",ios::trunc);
    if(ofile.is_open())
    {
        for(int go_row = 0 ; go_row < src.rows ; go_row++)
        {
            for(int go_col = 0 ; go_col < src.cols ; go_col++)
            {
                 ofile<<setw(3)<<setfill('0')<<(int)dst.at<uchar>(go_row,go_col)<<' ';
            }
            ofile<<endl;
        }
        ofile<<"thresh9ld = "<<threshold<<endl;
        cout<<"mat is stored in bin_txt"<<endl;
        // ofile<<setw(3)<<setfill('0')<<temp_dst<<' '<<endl;
    }
    else cout<<"開檔失敗"<<endl;



    ofile.close();
    imwrite((string)BIN_DIR + "bin_txt_img.bmp",dst);
	imshow("debug_bin",dst);


    if(waitKey() == 'C' )
    {
        namedWindow(BROWSE_WINDOW,WINDOW_AUTOSIZE);
	moveWindow(BROWSE_WINDOW,BAR_WINDOW_WIDTH + 20,0);
	namedWindow(BAR_WINDOW ,WINDOW_AUTOSIZE);
	resizeWindow(BAR_WINDOW ,BAR_WINDOW_WIDTH+300 ,BAR_WINDOW_HEIGHT);


        temp_proc_img = temp_src_img.clone();
        canny_l_slider = CANNY_L_SLIDER_DEFAULT;
        canny_h_slider = CANNY_H_SLIDER_DEFAULT;
        createTrackbar(CANNY_LOW_BAR ,BAR_WINDOW ,&canny_l_slider ,500 ,on_Cannyy);//如果用local的丟資料，要強制轉換(void*)&ord_img
        createTrackbar(CANNY_HIGH_BAR ,BAR_WINDOW ,&canny_h_slider ,500 ,on_Cannyy);
        while(waitKey() != ESC) {}
    }
    */
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
