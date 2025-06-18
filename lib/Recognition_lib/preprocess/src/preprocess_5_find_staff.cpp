/*
這個程式的功能是：
利用 preprocess_3 找出的 可能是五線譜的線
利用 preprocess_4 找出的 五線譜線的距離
篩選出真的五線譜
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <iostream>

#include "preprocess_5_find_staff.h"

#define STAFF_LINE_DISTANCE_ERROR 3

#define PI 3.14159

using namespace cv;
using namespace std;

void position_erase(vector<Vec2f>&src_lines,int position){
    if(src_lines.size() > 0){
        if(src_lines.size() == 1){
            src_lines.clear();
            return;
        }
        else{
            if(position == src_lines.size() -1){ //刪最後一個
                src_lines.pop_back();
                return;
            }
            else{
                for(int i = position ; i < src_lines.size()-1 ; i++){
                    src_lines[i][0] = src_lines[i+1][0];
                    src_lines[i][1] = src_lines[i+1][1];
                }
                src_lines.pop_back();
                return;
            }
        }
    }
    else{
        cout<<"沒有線可以刪，沒有做動作"<<endl;
        return;
    }
}
void filter_distance(vector<Vec2f>& src_lines,int line_distance_err){
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////// 把90度的線找出來 /////////////////
    bubbleSort_H_rho(src_lines);

	// const int line_distance_err = 5;//第二條線以後要跟前一條線比較距離最小間隔，經過很多圖片測試(連手寫校歌也測了)， < 5 通常就是同條線
	const double angle_err = 5;



    // //step 1 接近水平
    // double angle_value = (theta/PI)*180;
	for(int i = 0 ; i < src_lines.size() ; i++){
        if( abs( (src_lines[i][1]/PI*180) - 90 ) > angle_err ){
            position_erase(src_lines,i);
            i--;
        }

        if( i >= 1){ //第二條線以後要跟前一條線比較距離，太近就代表同條線不用存喔，還有角度離90度差太多也不用存
            // cout<<abs( (src_lines[i][1]/PI*180) - 90)<<endl;
            // cout<<"i = "<<i <<endl;
            if( ( src_lines[i][0] - src_lines[i-1][0] <= line_distance_err ) ){
                position_erase(src_lines,i);
                i--;
            }
            else if( abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) > 1.5 && ( src_lines[i][0] - src_lines[i-1][0] < line_distance_err )){
                position_erase(src_lines,i);
                i--;

                cout<<" angle_err = "<< abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI))<<endl;
            }
        }
        else ; //do nothing
    }
    return;
}
/*
int find_Staff(vector<Vec2f> src_lines,vector<Vec2f>& select_lines_2, int*& line_num_array, int*& staff_num_array){ //第一個參數放要代找的線，第二個參數放容器
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// 把90度的線找出來 /////////////////
    bubbleSort_H_rho(src_lines);

    const double err = 5.0; //誤差
	const int line_distance_err = 10;//第二條線以後要跟前一條線比較距離最小間隔=
	const double angle_err = 3;
    int temp_line_posi = 0;
    vector<Vec2f> select_lines_1;
*/
/*
////step 1 接近水平
//double angle_value = (theta/PI)*180;
	for(int i = 0 ; i < src_lines.size() ; i++){
        if( ((src_lines[i][1]/PI*180) <= 90 + err) && ((src_lines[i][1]/PI*180) >= 90-err) ){
			if( i >= 1) //第二條線以後要跟前一條線比較距離，太近就代表同條線不用存喔{
				//cout<<"i = "<<i <<endl;
				if(src_lines[i][0] - src_lines[temp_line_posi][0] > line_distance_err){
					select_lines_1.push_back(src_lines[i]);
					temp_line_posi = i;
				}
				//imshow("pause",roi_proc_img);
				//waitKey();
			}
			else{
				select_lines_1.push_back(src_lines[i]);
				temp_line_posi = i;
			}
        }
    }

////step 2 相近的兩條幾乎一樣
    //vector<Vec2f> select_lines_2;
    //cout<<"select_lines_1.size() = "<<select_lines_1.size()<<endl;
    if(select_lines_1.size() >= 5){
        select_lines_2.push_back(select_lines_1[0]);
        for(int i = 1 ; i < select_lines_1.size() ; i++){
            if( abs((select_lines_1[i-1][1]*180/PI) - (select_lines_1[i][1]*180/PI)) < (int)angle_err){
                select_lines_2.push_back(select_lines_1[i]);
                cout<<"select_lines_1[i-1][1]*180/PI= "<<(select_lines_1[i-1][1]*180/PI);
                cout<<" ,select_lines_1[i][1]*180/PI= "<<(select_lines_1[i  ][1]*180/PI)<<endl;
                cout<<" angle_err = "<< abs((select_lines_1[i-1][1]*180/PI) - (select_lines_1[i][1]*180/PI))<<endl;
                //temp_line_posi = i;
            }
        }
    }
    else{
        cout<<"find_line failed~~"<<endl;
        return 0;
    }
*/
    //Watch_Hough_Line(select_lines_2,sobel_proc_color,HOUGH_SOBEL_WINDOW);
/*
	///////////// 幫找出來90度的線編號 //////////////
	if(select_lines_2.size()>0)
	{
		////////////// 線的距離 //////////////
		int * distance = new int[select_lines_2.size()];
		distance[0] = select_lines_2[0][0];
		cout<<"i = "<<'0'<<" distance = "<<distance[0]<<endl;
		for(int i = 1 ; i < select_lines_2.size() ; i++){
			distance[i] = select_lines_2[i][0] - select_lines_2[i-1][0];
			cout<<"i = "<<i<<" distance = "<<distance[i]<<endl;
		}
		cout<<endl;

		///////////// 開始編號 ////////////////
		const int staff_line_distance = 25;//這個25是從上面的測試distance資料得來，會想辦法自動取得
		//int * line_num_array = new int[select_lines_2.size()]; //改到上面去
		line_num_array = new int[select_lines_2.size()];

		line_num_array[0] = 0; //first line index = 0, second line index = 1 .......
		int num = 0;//num is index喔!!! 下面的for迴圈要小心!!!!!!!!!!!!!!!!!
		cout<<"i = "<< '0' <<" line_num "<<num<<endl;
		for(int i = 1 ; i < select_lines_2.size(); i++){
			if(distance[i] < staff_line_distance) line_num_array[i] = num;
			else line_num_array[i] = ++num;
			cout<<"i = "<< i <<" line_num "<<num<<endl;
		}
		cout<<endl;

		//////////// 找出五線譜 //////////////
		///////////統計上面每個編號的個數////////////=//
		int * num_count_array = new int[num + 1]; //num is index喔!!!所以要 "+1"
		for(int i = 0 ; i <= num ; i++) num_count_array[i] = 0; //num is index喔!!!所以要 "+1"
		for(int i = 0 ; i < select_lines_2.size() ; i++) num_count_array[line_num_array[i]]++;

		/////////// 如果個數有剛好五個的，代表找到五線譜了，存入staff_num/////////////
		//int * staff_num_array  = new int[select_lines_2.size()/4]; 一、寫上去 二、覺得大小改 num+1 ~~多存點沒差拉~~
		staff_num_array  = new int[num + 1];
		int staff_num_count = 0;
		for(int i = 0 ; i <= num ; i++) //num is index喔!!!所以要 "="{
			if(num_count_array[i] ==5) staff_num_array[staff_num_count++] = i;
		}
		for(int i = 0 ; i < staff_num_count ; i++) cout<<"The number of \""<<staff_num_array[i]<<"\" is the staff_number"<<endl;

		return (staff_num_count);
		/////////////////////////////////////////////

//		cout<<"address of staff_num_array = "<< staff_num_array<<endl;
	}
}
*/


//不要用 vector<Vec2f>*& staff 原因寫在下面
int find_Staff2(vector<Vec2f>& select_lines, int dist_level_0, int dist_level_1){
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////// 把90度的線找出來 /////////////////

    // 先根據rho做排序
    bubbleSort_H_rho(select_lines);


    // 再把可能是相同線的線先濾掉
    filter_distance(select_lines,dist_level_0);


//再來要check濾完以後的線
//一、根據distance_level_1的距離來分類線

//二、a、如果每個類別剛好五條，就成功
//    b、如果大於五條，代表上面沒濾乾淨在濾一次
//    c、如果小於五條，代表有問題把線那組線通通刪掉
    if(select_lines.size()>0){

//一：
		////////////// 傳進來的線的 第n條 到 自己下一條的 距離 //////////////
		int * distance = new int[select_lines.size()];
		distance[0] = select_lines[0][0];
		cout<<"i = "<<'0'<<" distance = "<<distance[0]<<endl;
		for(int i = 1 ; i < select_lines.size() ; i++){
			distance[i] = select_lines[i][0] - select_lines[i-1][0];
			cout<<"i = "<<i<<" distance = "<<distance[i]<<endl;
		}
		cout<<endl;


		///////////// 開始根據五線譜線的距離(dist_level_1)編號 ////////////////
		const int staff_line_distance = dist_level_1+STAFF_LINE_DISTANCE_ERROR;//+3還是怕誤差拉~~
		// (想想如果刪掉一條線，那刪掉的該條的上一條線到下一條線的距離就會增加~~~，now是假設刪掉了，所以要加上些誤差


		// int * line_num_array = new int[select_lines_2.size()]; //改到上面去
		int * line_num_array = new int[select_lines.size()];



		line_num_array[0] = 0;  // first line index = 0, second line index = 1 .......
		int num = 0;  // num is index喔!!! 下面的for迴圈要小心!!!!!!!!!!!!!!!!!
		cout<<"i = "<< '0' <<" line_num "<<num<<endl;
		for(int i = 1 ; i < select_lines.size(); i++){
			if(distance[i] < staff_line_distance) line_num_array[i] = num;
			else line_num_array[i] = ++num;
			cout<<"i = "<< i <<" line_num "<<num<<endl;
		}
		cout<<endl;


//二：
		//////////// 找出五線譜 //////////////
		///////////統計上面每個編號的個數//////////////
		int * num_count_array = new int[num + 1]; //num is index喔!!!所以要 "+1"
		for(int i = 0 ; i <= num ; i++) num_count_array[i] = 0; //num is index喔!!!所以要 "+1"
		for(int i = 0 ; i < select_lines.size() ; i++) num_count_array[line_num_array[i]]++;

		/////////// 如果個數有剛好五個的，代表找到五線譜了，存入staff_num/////////////
		int * staff_num_array  = new int[num + 1];//num is index喔!!!所以要 "+1"
		int staff_num_count = 0;

		int go_line = 0;

		for(int i = 0 ; i <= num ; i++){ //num is index喔!!!所以要 "="
//b：		    //如果線大於五條的話，
		    if(num_count_array[i] >5){
                int staff_line_distance_2 = dist_level_0;


                float ave_theta = 0;
                for(int j = go_line ; j < num_count_array[i]+ go_line ; j++) ave_theta += select_lines[j][1];
                ave_theta /= num_count_array[i];

                do{
                    for(int j = go_line ; j < num_count_array[i]+go_line ; j++){
                        // 因為是 現在這條 跟 下一條 相減來看距離，所以最後一條不能做(因為沒有下一條了)，一定要這個if
                        if( j < num_count_array[i]+ go_line -1){
                            // 不用distance是因為~~~要多寫處理array的position_erase麻煩且當除沒想到ˊ口ˋ，所以乾脆直接減拉~~
                            if( (select_lines[j+1][0] - select_lines[j][0]) < staff_line_distance_2 ){
                                if(abs(select_lines[j+1][1] - ave_theta) > abs(select_lines[j][1] - ave_theta)){
                                    position_erase(select_lines,j+1);
                                    num_count_array[i]--;
                                    j--;
                                }
                                else{
                                    position_erase(select_lines,j);
                                    num_count_array[i]--;
                                    j--;
                                }
                            }
                            else ;// do nothing
                        }

                    }
                    staff_line_distance_2++;
//                  cout<<"still while~~  ";
//                  cout<<"staff_line_distance_2 = "<<staff_line_distance_2<<"  ";
//                  cout<<"go_line = "<<go_line<<"  刪完後剩下"<<num_count_array[i]<<"條線"<<endl;

                }while(num_count_array[i] > 5);
//                cout<<"staff_line_distance_2 = "<<staff_line_distance_2<<endl;

//                cout<<"go_line = "<<go_line<<"  刪完後剩下"<<num_count_array[i]<<"條線"<<endl;
                go_line += num_count_array[i];
                staff_num_array[staff_num_count++] = i;

            }

//c：
            else if(num_count_array[i] <5)
            {
                for(int j = 0 ; j < num_count_array[i] ; j++)
                {
                    position_erase(select_lines,go_line);
                    cout<<"go_line = "<<go_line<<endl;
                }
            }

//a：
			else if(num_count_array[i] ==5)
            {
                go_line += 5;
                staff_num_array[staff_num_count++] = i;
            }

		}


		for(int i = 0 ; i < staff_num_count ; i++) cout<<"The number of \""<<staff_num_array[i]<<"\" is the staff_number"<<endl;

//不要包成vector<Vec2f>staff[i]好了，因為  一、傳function不知道怎麼丟(下面是失敗的喔)  二多此一舉的感覺，用下面的for寫法存原來的線就可以拉！
        //staff = new vector<Vec2f>[staff_num_count];
        /*
        for(int i = 0 ; i < staff_num_count ; i++) staff[i].clear();
        for(int i = 0 ; i < staff_num_count ; i++){
            int first_line = 5*i;
            int fifth_line = 5*(i+1)-1;
            for(int j = first_line ; j <= fifth_line ; j++)
            {
                staff[i].push_back(select_lines[j]);
                cout<<"j = "<<j<<"data = "<<select_lines[j][0]<<" "<<select_lines[j][1]<<endl;
            }
        }
        cout<<endl;
        */

//用這個for寫法就可以用 staff 為單位來用線囉！
        for(int i = 0 ; i < staff_num_count ; i++){
            int first_line = 5*i;
            int fifth_line = 5*(i+1)-1;
            for(int j = first_line ; j <= fifth_line ; j++){
//                cout<<"j = "<<j<<" data = "<<select_lines[j][0]<<" "<<select_lines[j][1]<<endl;
            }
        }


		return (staff_num_count);
		/////////////////////////////////////////////

//		cout<<"address of staff_num_array = "<< staff_num_array<<endl;
	}
	else{
        cout<<"ntmd沒有線找啥米呀ˊ口ˋ"<<endl;
    }
}

