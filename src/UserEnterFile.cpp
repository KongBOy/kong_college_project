/*
這個程式的功能是：
透過攝影機，擷取要的 "速度" "音量" 資訊
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace cv;

/** Function Headers */
Mat T1=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\1.jpg",1);
Mat T2=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\2.jpg",1);
Mat T3=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\3.jpg",1);
Mat T4=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\4.jpg",1);
Mat T5=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\5.jpg",1);
Mat T6=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\6.jpg",1);
Mat T7=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\7.jpg",1);
Mat T8=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\8.jpg",1);
Mat T9=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\9.jpg",1);
Mat T10=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\10.jpg",1);
Mat T11=imread("Resource\\UI_all_picture/UI PIC\\UI\\UI_say_something\\11.jpg",1);
Mat background1=imread("Resource\\UI_all_picture/UI PIC/UI/Background.png",1);

bool detectAndDisplay( Mat frame,float *facex,float *facey,float *facewidth,float *faceheight );
void SamplePicInitial();
float GaussResult(int a,int b,int c);
bool DetectHandShakeSpeed();
int DrawMat(Mat Input,Mat& Output,int row,int col);
int DrawTalk(Mat Input,Mat& Output,int row,int col);

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
///C:\\Users\\may\\Desktop\\龔洲暐\\學校\\專題\\VC2010\\opencv
String face_cascade_name = "C:\\Users\\may\\Desktop\\龔洲暐\\學校\\專題\\VC2010\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";


static int RedTreshod = 300;
static int UserGoOutWhenPlayingTime=60000;  ///1Min
static int HandStopShankingTime=60000;      ///10S

//////////////////////////臉部偵測用
float facex;
float facey;
float facewidth;
float faceheight;

/////////////////////////高斯運算用
CvScalar MeanScalar;
CvScalar StandardDeviationScalar;



///////////////////////手勢辨識用
int speed=60;
int volume=80;
bool MusicPlayback=false;
Mat Output;
int row_index = 0;
Mat row_proc_img[40];

float prex = 0;
float prey = 0;
float nowx = 0;
float nowy = 0;
float shakhighest=0;
float shaklowest=0;
float shakrange=0;


int a=0;
int compute_speed_M[6]={0};
int compute_volume_M[6]={0};
int average_speed=0;
int sum_handY=0;
int average_volume=0;
int sum_volume=0;
int var=0;

bool handmoveup=false;
bool pre_handmoveup=false;

CvCapture* capture ;


time_t preHandBitClock=clock()+10000;
time_t nowHandBitClock=clock()+10000;

int HandShaking(string Title){
    time_t LastTimeFineUser=clock();
    time_t OverTimeUserDisapper=clock()+UserGoOutWhenPlayingTime;

	IplImage* vframe ;

	Mat copyFrame;
	Mat image;
	Mat frame;

	double MinValue;
	double MaxValue;

	Point MinLocation;
	Point MaxLocation;


    ///臉部辨識 內建的東西~~來判斷是不是人臉
	///if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

    ///開始視訊
	capture = cvCaptureFromCAM( -1 );

    int index=0;
	int orbit_num = 8;
	int orbitX[8]={0};
	int orbitY[8]={0};

    int talktime=0;
    Mat talk;
    Mat Output2;
	///偵測手環是否進入範圍內
	if(capture)
	{
	    SamplePicInitial();
	    Mat ReducePic;
        Mat ReducePicShow;
        int returnval=1;
		while( LastTimeFineUser<OverTimeUserDisapper&&returnval==1)
		{
		    //cout<<"APPNOWTIME"<<LastTimeFineUser<<endl;
            //cout<<"ENDSTEPTIME"<<OverTimeUserDisapper<<endl;
			frame = cvQueryFrame( capture );

			if( !frame.empty() ){
			  //printf("OK!!!!\n");
			}else{
			  printf(" --(!) No captured frame -- Break!"); break;
			}

            ///delay 10 毫秒 抓一次圖
			int c = waitKey(1);
			if( (char)c == 'c' ) { break; }


			image = frame;

			/// 臉部偵測的function
			resize(frame, ReducePic, Size(frame.cols/3, frame.rows/3), 0, 0, INTER_CUBIC);

            //imshow("frame",frame);




            Mat newimg;
			newimg = ReducePic.clone();
			Mat probimg(newimg.rows,newimg.cols,CV_8UC1);
			float B;
            float G;
            float R;
            float stander_dev;
            int MinRow=0;
            int MinCol=0;
            int MinValue=300;
            for(int i = 0; i < newimg.rows; i++){
                for(int j = 0; j < newimg.cols; j++){

                    B=newimg.at<Vec3b>(i,j)[0];
                    G=newimg.at<Vec3b>(i,j)[1];
                    R=newimg.at<Vec3b>(i,j)[2];
                    stander_dev=sqrt(pow((B-MeanScalar.val[0]),2)+pow((G-MeanScalar.val[1]),2)+pow((R-MeanScalar.val[2]),2));
                    if(stander_dev<MinValue){
                        MinValue=stander_dev;
                        MinRow=i;
                        MinCol=j;
                    }
                    //cout<<"stander_dev "<<stander_dev<<endl;
                }
            }
            //cout<<"MinVale "<<MinValue<<endl;

            /*if(detectAndDisplay(newimg,&facex,&facey,&facewidth,&faceheight)){

                OverTimeUserDisapper=clock()+UserGoOutWhenPlayingTime;
            }

            LastTimeFineUser=clock();
            */
            OverTimeUserDisapper=clock()+UserGoOutWhenPlayingTime;
            LastTimeFineUser=clock();

            nowx=MinCol;
            nowy=MinRow;
            if(!DetectHandShakeSpeed()){
                preHandBitClock=clock();
                cout<<"DetectHandShakeSpeed"<<endl;
                returnval=3;
            }

            if(!MusicPlayback){
                    break;
            }
            prex=MinCol;
            prey=MinRow;

            circle(newimg,cvPoint(MinCol,MinRow),4,Scalar(250,0,0), 2, 8,0);
            if(index<orbit_num){
				orbitX[index]=MinCol;
				orbitY[index]=MinRow;
				index++;
			}else {
				index =0;
				orbitX[index]=MinCol;
				orbitY[index]=MinRow;
				index++;
			}



			for(int i = 0;i<orbit_num;i++){
                //newimg.at<Vec3b>(orbitY[i],orbitX[i])[0]=44;
                //newimg.at<Vec3b>(orbitY[i],orbitX[i])[1]=250;
                //newimg.at<Vec3b>(orbitY[i],orbitX[i])[2]=3;
                line( newimg, Point(orbitX[i],orbitY[i]), Point(orbitX[(i+1)%8],orbitY[(i+1)%8]),Scalar(44,250,3), 1, 8 );
				//circle(newimg,cvPoint(orbitX[i],orbitY[i]),2,Scalar(44,250,3), 2, 8,0);
			}
            resize(newimg, newimg, Size(frame.cols*4/5, frame.rows*4/5), 0, 0, INTER_CUBIC);
            //imshow("ReducePic",ReducePicShow);
            if(!Output.empty()){
                DrawMat(newimg,Output,78,65);
                //resize(row_proc_img[row_index], row_proc_img[row_index], Size(1211, 227), 0, 0, INTER_CUBIC);
                if(row_proc_img[row_index].rows>227)
                    DrawMat(row_proc_img[row_index](Rect(0,0,1166,227)),Output,530,62);
                else
                    DrawMat(row_proc_img[row_index],Output,530,62);

            }

            Output2=Output.clone();
            if(clock()>talktime){

                talktime=clock()+5000+(rand()%10*1000);
                switch(1+rand()%11){
                case 1:
                    resize(T1, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 2:
                    resize(T2, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 3:
                    resize(T3, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 4:
                    resize(T4, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 5:
                    resize(T5, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 6:
                    resize(T6, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 7:
                    resize(T7, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 8:
                    resize(T8, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 9:
                    resize(T9, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 10:
                    resize(T10, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                case 11:
                    resize(T11, talk, Size(T1.cols*0.7,T1.rows*0.7), 0, 0, INTER_CUBIC);
                    break;
                }

                //cout<<"talktime"<<talktime<<endl;
                //cout<<"clock()"<<clock()<<endl;
            }

            DrawTalk(talk,Output2,130,633);

            imshow(Title,Output2);
		}
		cvReleaseCapture(&capture);
        return returnval;
	}

}

/**
 * @function detectAndDisplay
 */
bool detectAndDisplay( Mat frame,float *facex,float *facey,float *facewidth,float *faceheight )
{
	std::vector<Rect> faces;
    Mat frame_gray;
/*  float facex=0;
    float facey=0;
    float facewidth=0;
    float faceheight=0;
*/
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //-- Detect faces

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(1, 1) );
    if(faces.size()==0)
        return false;

    int BiggestFace=10;
    int MinFaceLengh=frame.rows+frame.cols;
    for( int i = 0; i < faces.size(); i++ )
    {
        if((faces[i].width+faces[i].height)<MinFaceLengh){
            MinFaceLengh=faces[i].width+faces[i].height;
            BiggestFace=i;
        }


	    *facex = faces[i].x;
	    *facey = faces[i].y;
	    *facewidth = faces[i].width;
	    *faceheight = faces[i].height;

    }
    if(BiggestFace!=10){
        Point center( faces[BiggestFace].x + faces[BiggestFace].width*0.5, faces[BiggestFace].y + faces[BiggestFace].height*0.5 );
        rectangle(frame, cvPoint(faces[BiggestFace].x ,faces[BiggestFace].y), cvPoint(faces[BiggestFace].x +faces[BiggestFace].width ,faces[BiggestFace].y +faces[BiggestFace].height),2,3,0);
        //cout<<"face size x="<<faces[BiggestFace].width<<" y="<<faces[BiggestFace].height<<endl;
    }



     //-- Show what you got
     //imshow( window_name, frame );
     //imwrite( "C:\\Users\\user\\Desktop\\test\\result.jpg", frame );
     return true;
}



void SamplePicInitial(){
    ///C:\Users\Dennis\Desktop\新增資料夾\MIDIPlayer
    ///C:\Users\may\Desktop\winter_week2
    IplImage *src=cvLoadImage("testmode4.jpg",1);
    cvAvgSdv(src,&MeanScalar,&StandardDeviationScalar);

    //printf("Blue Channel Avg is : %.f\n",MeanScalar.val[0]);
    //printf("Blue Channel Standard Deviation is : %.f\n",StandardDeviationScalar.val[0]);
    //printf("Green Channel Avg is : %.f\n",MeanScalar.val[1]);
    //printf("Green Channel Standard Deviation is : %.f\n",StandardDeviationScalar.val[1]);
    //printf("Red Channel Avg is : %.f\n",MeanScalar.val[2]);
    //printf("Red Channel Standard Deviation is : %.f\n",StandardDeviationScalar.val[2]);


}

bool DetectHandShakeSpeed(){

   if(a>5)a=0;
   if((nowy-prey)>10){
        handmoveup=false;
    }else if((nowy-prey)<-10){

        handmoveup=true;
    }
    if(pre_handmoveup!=handmoveup&&pre_handmoveup==true){   //表示改變方向
        shakhighest=nowy;
        nowHandBitClock = clock();

        //cout<<"a "<<a<<endl;
        compute_speed_M[a]=nowHandBitClock-preHandBitClock;
        sum_handY=0;
        for(int i=0;i<6;i++){
            sum_handY=sum_handY+compute_speed_M[i];
        }

        average_speed=sum_handY/6;
        int dev[6]={0};
        for(int i=0;i<6;i++){
            dev[i]= compute_speed_M[i]-average_speed;
        }


        sort(dev,dev+6);
        //for(int i=0;i<6;i++)
        //   cout<<"dev["<<i<<"] "<<dev[i]<<endl;

        var=0;
        for(int i=0;i<4;i++)
            var=var+pow(dev[i],2);
        //cout<<"                 ~~~~var "<<var<<endl;
        if(var<60000000){
            int k=60000/((dev[0]+dev[1]+dev[2]+dev[3])/4+average_speed+1);
            if(k<300&&k>20)
                speed=k;
            //speed=60000/(endclock-startclock);
            cout <<"---------------------------speed      :" << speed <<endl <<endl ;
        }


        a+=1;
        //cout <<"---------------------------during time:" << endclock-startclock <<endl <<endl ;

        preHandBitClock = clock();
        /////////////////////////////////////////////////////clock
    }else if(pre_handmoveup!=handmoveup&&pre_handmoveup==false){
        shaklowest=nowy;
        shakrange=abs(shakhighest-shaklowest);
        //cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!Hand shakrange" <<shakrange<<endl;
        compute_volume_M[a]=shakrange;
        sum_volume=0;
        for(int i=0;i<6;i++){
            sum_volume=sum_volume+compute_volume_M[i];
        }

        average_volume=sum_volume/6;
        average_volume=60+(2*average_volume);
        //for(int i=0;i<6;i++)
        //   cout<<"compute_volume_M["<<i<<"] "<<compute_volume_M[i]<<endl;
        cout<<"average_volume "<<average_volume<<endl;
        if(average_volume<127)
            volume=average_volume;

    }

    pre_handmoveup=handmoveup;
    int NOWTIME=clock();
    cout<<"NOWTIME-preHandBitClock"<<NOWTIME-preHandBitClock<<endl;
    //if(abs(NOWTIME-preHandBitClock)>HandStopShankingTime)return false;

    return true;
}


int DrawMat(Mat Input,Mat& Output,int row,int col){
    int OutputRow=Output.rows;
    int OutputCol=Output.cols;
    int InputRow=Input.rows;
    int InputCol=Input.cols;

    if(row<OutputRow&&col<OutputCol){
        for(int i=row;i<(row+InputRow);i++){
            for(int j=col;j<(col+InputCol);j++){
                Output.at<Vec3b>(i,j)[0]=Input.at<Vec3b>(i-row,j-col)[0];
                Output.at<Vec3b>(i,j)[1]=Input.at<Vec3b>(i-row,j-col)[1];
                Output.at<Vec3b>(i,j)[2]=Input.at<Vec3b>(i-row,j-col)[2];
            }
        }

    }else{
        cout<<"Draw Out of Range"<<endl;
        waitKey(0);
    }
    //imshow("Output",Output);
    //waitKey(0);
    return 0;
}
int DrawTalk(Mat Input,Mat& Output,int row,int col){
    int OutputRow=Output.rows;
    int OutputCol=Output.cols;
    int InputRow=Input.rows;
    int InputCol=Input.cols;
    if(row<OutputRow&&col<OutputCol){
        for(int i=row;i<(row+InputRow);i++){
            for(int j=col;j<(col+InputCol);j++){
                if(Input.at<Vec3b>(i-row,j-col)[0]<200)
                    Output.at<Vec3b>(i,j)=Input.at<Vec3b>(i-row,j-col);
            }
        }

    }else{
        cout<<"Draw Out of Range"<<endl;
        waitKey(0);
    }
    //imshow("Output",Output);
    //waitKey(0);
    return 0;
}


