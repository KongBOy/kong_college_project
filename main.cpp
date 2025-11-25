/*
這個程式的功能是：
整個系統的整合介面(動畫、拍照、辨識+轉midi+攝影機)
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Game_UI.h"

#include "ScreenTool.h"
#include "DirectoryTool.h"

using namespace std;
using namespace cv;

int main(){
    Game game;
    game.run();

    Mat img1(10, 10, CV_8UC3, Scalar(10, 20, 30));
    Mat img2(10, 10, CV_8UC3, Scalar(15, 30, 45));
    img2.at<Vec3b>(5, 5)[0] = 10;
    img2.at<Vec3b>(5, 5)[1] = 20;
    img2.at<Vec3b>(5, 5)[2] = 30;
    img1.convertTo(img1, CV_32F);
    img2.convertTo(img2, CV_32F);
    Mat sub_img = img1 - img2;
    pow(sub_img, 2, sub_img);
    cout << "sub_img:" << sub_img << endl;
    cout << "sub_img.rows:" << sub_img.rows << endl;
    cout << "sub_img.cols:" << sub_img.cols << endl;
    cout << "sub_img.depth:" << sub_img.depth() << endl;

    vector<Mat> channels;
    split(sub_img, channels);
    sub_img = channels[0] + channels[1] + channels[2];

    
    // reduce(sub_img, sub_img, 2, CV_REDUCE_SUM, CV_32F);

    cout << "sub_img:" << sub_img << endl;
    cout << "sub_img.rows:" << sub_img.rows << endl;
    cout << "sub_img.cols:" << sub_img.cols << endl;
    cout << "sub_img.depth:" << sub_img.depth() << endl;
    // for(int i = 0; i < sub_img.rows; i ++ ){
    //     for(int j = 0; j < sub_img.cols; j ++ ){
    //         cout << sub_img.at<Vec3f>(i, j)[0] << ", ";
    //         cout << sub_img.at<Vec3f>(i, j)[1] << ", ";
    //         cout << sub_img.at<Vec3f>(i, j)[2] << ", ";
    //     }
    //     cout << endl;
    // }

    double minVal; double maxVal; Point minLoc; Point maxLoc;
    minMaxLoc( sub_img , &minVal, &maxVal, &minLoc, &maxLoc);
    cout << "minLoc.x:" << minLoc.x << endl;
    cout << "minLoc.y:" << minLoc.y << endl;
    
}
