#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

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