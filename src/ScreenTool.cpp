#include <iostream>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

void FadeInOut(string Title, Mat Now, Mat After, int delay){
    int row=Now.rows;
    int col=Now.cols;
    int a;
    imshow(Title, Now);
    for(int k=10; k > 0; k--){
        for(int i=0; i < row; i++){
            for(int j=0; j < col; j++){
                a = i * col + j;
                if(a % k == 0){
                    Now.at<Vec3b>(i, j)= After.at<Vec3b>(i, j);
                }
            }
        }
        imshow(Title, Now);
        waitKey(delay);
    }
    imshow(Title, After);
}


// Input圖去白色背景(200以上) 貼進 Output圖
int DrawTalk2(Mat Input, Mat& Output, int row, int col){
    int OutputRow = Output.rows;
    int OutputCol = Output.cols;
    int InputRow = Input.rows;
    int InputCol = Input.cols;
    if(row < OutputRow && col < OutputCol){
        for(int i = row;i < (row + InputRow);i ++ ){
            for(int j = col;j < (col + InputCol);j ++ ){
                if(Input.at<Vec3b>(i-row, j-col)[0] < 200)
                    Output.at<Vec3b>(i, j) = Input.at<Vec3b>(i-row, j-col);
            }
        }

    }
    else{
        cout << "Draw Out of Range" << endl;
        waitKey(0);
    }
    // imshow("Output", Output);
    // waitKey(0);
    return 0;
}