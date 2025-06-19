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
#include <iomanip>

#include "preprocess_0_watch_hough_line.h"
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

void filter_multi_same_line(vector<Vec2f>& src_lines, int line_distance_err, bool debuging){
    // 去除重複線, 走訪每一條線, 第二條線以後 和前一條線比 如果太相近 或 差太多 就去掉
	for(int i = 0 ; i < src_lines.size() ; i++){
        // 每一條線都不可以跟90度差太遠, 差太遠就不存, 經過很多圖片測試(連手寫校歌也測了)， < 5 通常就是同條線
        if( abs( (src_lines[i][1]/PI*180) - 90 ) > 5 ){
            position_erase(src_lines, i);
            i--;
        }

        // 第二條線以後要還要跟前一條線比較距離，太近就代表同條線不用存喔，還有角度離上一條線差太多也不用存
        if( i >= 1){
            if(debuging) cout<< "line_" << i << " and line_" << i - 1 << " distance: " << src_lines[i][0] - src_lines[i-1][0]<<", angle_diff: "<< abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) << " , ";
            
            // 太近就代表同條線不用存
            if( ( src_lines[i][0] - src_lines[i-1][0] <= line_distance_err ) ){
                if(debuging) cout<< "line_" << i <<" distance_too_short: "<< src_lines[i][0] - src_lines[i-1][0] << endl;
                position_erase(src_lines, i);
                i--;
                continue;
            }
            // 如果稍近 但 角度離上一條線差太多也不用存, 測試後覺得 1.5度不錯
            if( ( src_lines[i][0] - src_lines[i-1][0] <= line_distance_err * 2 ) && abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) > 1.5 ){
                if(debuging) cout<< "line_" << i <<" angle_diff_too_big: "<< abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) << endl;
                position_erase(src_lines, i);
                i--;
                continue;
            }
            if(debuging) cout << endl;
        }
    }
    if(debuging) Watch_Hough_Line(src_lines);
    return;
}
/*
int find_Staff(vector<Vec2f> src_lines,vector<Vec2f>& select_lines_2, int*& line_num_array, int*& staff_num_array){ //第一個參數放要代找的線，第二個參數放容器
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// 把90度的線找出來 /////////////////

    const double err = 5.0; //誤差
	const int line_distance_err = 10;//第二條線以後要跟前一條線比較距離最小間隔=
	const double angle_err = 3;
    int temp_line_posi = 0;
    vector<Vec2f> select_lines_1;
*/
/*
    ////step 1 接近水平
    // double angle_value = (theta/PI)*180;
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
    // vector<Vec2f> select_lines_2;
    // cout<<"select_lines_1.size() = "<<select_lines_1.size()<<endl;
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
	if(select_lines_2.size()>0){
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
		const int staff_lnter_distance = 25;//這個25是從上面的測試distance資料得來，會想辦法自動取得
		// int * line_num_array = new int[select_lines_2.size()]; //改到上面去
		line_num_array = new int[select_lines_2.size()];

		line_num_array[0] = 0; //first line index = 0, second line index = 1 .......
		int group_index = 0;//group_index is index喔!!! 下面的for迴圈要小心!!!!!!!!!!!!!!!!!
		cout<<"i = "<< '0' <<" line_group "<<group_index<<endl;
		for(int i = 1 ; i < select_lines_2.size(); i++){
			if(distance[i] < staff_lnter_distance) line_num_array[i] = group_index;
			else line_num_array[i] = ++group_index;
			cout<<"i = "<< i <<" line_group "<<group_index<<endl;
		}
		cout<<endl;

		//////////// 找出五線譜 //////////////
		///////////統計上面每個編號的個數////////////=//
		int * gid_count_array = new int[group_index + 1]; // group_index 是 index喔!!! index轉數量要 "+1"
		for(int i = 0 ; i <= group_index ; i++) gid_count_array[i] = 0; // group_index 是 index喔!!! index轉數量要 "+1"
		for(int i = 0 ; i < select_lines_2.size() ; i++) gid_count_array[line_num_array[i]]++;

		/////////// 如果個數有剛好五個的，代表找到五線譜了，存入staff_num/////////////
		// int * staff_num_array  = new int[select_lines_2.size()/4]; 一、寫上去 二、覺得大小改 group_index+1 ~~多存點沒差拉~~
		staff_num_array  = new int[group_index + 1];
		int staff_index = 0;
		for(int i = 0 ; i <= group_index ; i++) //group_index is index喔!!!所以要 "="{
			if(gid_count_array[i] ==5) staff_num_array[staff_index++] = i;
		}
		for(int i = 0 ; i < staff_index ; i++) cout<<"The number of \""<<staff_num_array[i]<<"\" is the staff_number"<<endl;

		return (staff_index);
		/////////////////////////////////////////////

		// cout<<"address of staff_num_array = "<< staff_num_array<<endl;
	}
}
*/


//不要用 vector<Vec2f>*& staff 原因寫在下面
int find_Staff2(vector<Vec2f>& select_lines, int dist_level_0, int dist_level_1, bool debuging){
    //////////////// 把90度的線找出來 /////////////////


    // 把可能是相同線的線先濾掉
    filter_multi_same_line(select_lines, dist_level_0, debuging);


    //再來要check濾完以後的線
    //一、根據distance_level_1的距離來分類線

    //二、a、如果每個類別剛好五條，就成功
    //    b、如果大於五條，代表上面沒濾乾淨再濾一次
    //    c、如果小於五條，代表有問題把線那組線通通刪掉
    if(select_lines.size() > 0){

    //一：
		////////////// 傳進來的線的 第n條 到 自己上一條的 距離 //////////////
		int * distance = new int[select_lines.size()];
		distance[0] = select_lines[0][0];
		for(int i = 1 ; i < select_lines.size() ; i++) distance[i] = select_lines[i][0] - select_lines[i-1][0];
        
        if(debuging){
            cout << "i = " << setw(2) << '0' << " distance = " << distance[0] << endl;
            for(int i = 1 ; i < select_lines.size() ; i++) cout<<"i = "<< setw(2) << i << " distance = " << distance[i] << endl;
            cout << endl;
        }
        // 大概像這樣子:
        // i =  0 distance = 889
        // i =  1 distance = 17
        // i =  2 distance = 19
        // i =  3 distance = 18
        // i =  4 distance = 18
        // i =  5 distance = 243
        // i =  6 distance = 19
        // i =  7 distance = 17
        // i =  8 distance = 18
        // i =  9 distance = 18
        // i = 10 distance = 244
        // i = 11 distance = 16
        // i = 12 distance = 18
        // i = 13 distance = 18
        // i = 14 distance = 17
        // i = 15 distance = 241
        // i = 16 distance = 18
        // i = 17 distance = 17
        // i = 18 distance = 18
        // i = 19 distance = 18
        // i = 20 distance = 243
        // i = 21 distance = 17
        // i = 22 distance = 19
        // i = 23 distance = 16
        // i = 24 distance = 18
        // i = 25 distance = 242
        // i = 26 distance = 20
        // i = 27 distance = 18
        // i = 28 distance = 18
        // i = 29 distance = 18
        // i = 30 distance = 245
        // i = 31 distance = 18
        // i = 32 distance = 18
        // i = 33 distance = 18
        // i = 34 distance = 18
        // i = 35 distance = 250
        // i = 36 distance = 19
        // i = 37 distance = 19
        // i = 38 distance = 18
        // i = 39 distance = 19

		///////////// 每條線 根據自己跟上一條線的距離 和 五線譜線內部的距離(dist_level_1) 比對來做編號 ////////////////
		int group_index = 0;                                  // 標示第幾組五線譜
		int * line_num_array = new int[select_lines.size()];  // 標示 每條線 屬於第幾組五線譜 的容器
		line_num_array[0] = 0;                                // 第一條線 一定是第0組
		const int staff_lnter_distance = dist_level_1 + STAFF_LINE_DISTANCE_ERROR;  // + 3還是怕誤差, 反正目前的五線譜內的距離 跟五線譜間的距離還是有點差距, 所以 +3 問題不大
        // 走訪每條線, 如果跟上一條線的距離 <= staff_lnter_distance, 屬於同一組五線譜, 如果 > staff_lnter_distance, 就屬於下一組五線譜
		for(int i = 1 ; i < select_lines.size(); i++){
            if(distance[i] <= staff_lnter_distance) line_num_array[i] = group_index;
			else line_num_array[i] = ++group_index;
		}

        if(debuging) for(int i = 0 ; i < select_lines.size(); i++) cout<<"i = "<< i <<" line_group "<<group_index<<endl<<endl;
        
    // 二：
		//////////// 找出五線譜 //////////////
		/////////// 統計上面每個編號的個數 //////////////
		int * gid_count_array = new int[group_index + 1];                        // group_index 是 index喔!!! index轉數量要 "+1"
		for(int i = 0 ; i <= group_index         ; i++) gid_count_array[i] = 0;  // group_index 是 index喔!!! 所以用 "=", 初始化 array
		for(int i = 0 ; i <  select_lines.size() ; i++) gid_count_array[line_num_array[i]]++;

		/////////// 如果個數有剛好五個的，代表找到五線譜了，存入staff_num /////////////
		int * staff_num_array  = new int[group_index + 1];  // group_index 是 index喔!!! index轉數量要 "+1"
		int staff_index = 0;

		int go_line = 0;

		for(int i = 0 ; i <= group_index ; i++){  // group_index 是 index喔!!! 所以用 "="
        // b： // 如果線大於五條的話，
		    if(gid_count_array[i] >5){
                int staff_line_distance_2 = dist_level_0;


                float ave_theta = 0;
                for(int j = go_line ; j < gid_count_array[i]+ go_line ; j++) ave_theta += select_lines[j][1];
                ave_theta /= gid_count_array[i];

                do{
                    for(int j = go_line ; j < gid_count_array[i]+go_line ; j++){
                        // 因為是 現在這條 跟 下一條 相減來看距離，所以最後一條不能做(因為沒有下一條了)，一定要這個if
                        if( j < gid_count_array[i]+ go_line -1){
                            // 不用distance是因為~~~要多寫處理array的position_erase麻煩且當除沒想到ˊ口ˋ，所以乾脆直接減拉~~
                            if( (select_lines[j+1][0] - select_lines[j][0]) < staff_line_distance_2 ){
                                if(abs(select_lines[j+1][1] - ave_theta) > abs(select_lines[j][1] - ave_theta)){
                                    position_erase(select_lines,j+1);
                                    gid_count_array[i]--;
                                    j--;
                                }
                                else{
                                    position_erase(select_lines,j);
                                    gid_count_array[i]--;
                                    j--;
                                }
                            }
                            else ;// do nothing
                        }

                    }
                    staff_line_distance_2++;
                    // cout<<"still while~~  ";
                    // cout<<"staff_line_distance_2 = "<<staff_line_distance_2<<"  ";
                    // cout<<"go_line = "<<go_line<<"  刪完後剩下"<<gid_count_array[i]<<"條線"<<endl;

                } while(gid_count_array[i] > 5);
                // cout<<"staff_line_distance_2 = "<<staff_line_distance_2<<endl;

                // cout<<"go_line = "<<go_line<<"  刪完後剩下"<<gid_count_array[i]<<"條線"<<endl;
                go_line += gid_count_array[i];
                staff_num_array[staff_index++] = i;

            }

        //c：
            else if(gid_count_array[i] <5)
            {
                for(int j = 0 ; j < gid_count_array[i] ; j++)
                {
                    position_erase(select_lines,go_line);
                    cout<<"go_line = "<<go_line<<endl;
                }
            }

        //a：
			else if(gid_count_array[i] ==5)
            {
                go_line += 5;
                staff_num_array[staff_index++] = i;
            }

		}


		for(int i = 0 ; i < staff_index ; i++) cout<<"The number of \""<<staff_num_array[i]<<"\" is the staff_number"<<endl;

        // 不要包成vector<Vec2f>staff[i]好了，因為  一、傳function不知道怎麼丟(下面是失敗的喔)  二多此一舉的感覺，用下面的for寫法存原來的線就可以拉！
        // staff = new vector<Vec2f>[staff_index];
        /*
        for(int i = 0 ; i < staff_index ; i++) staff[i].clear();
        for(int i = 0 ; i < staff_index ; i++){
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

        // 用這個for寫法就可以用 staff 為單位來用線囉！
        for(int i = 0 ; i < staff_index ; i++){
            int first_line = 5*i;
            int fifth_line = 5*(i+1)-1;
            for(int j = first_line ; j <= fifth_line ; j++){
                // cout<<"j = "<<j<<" data = "<<select_lines[j][0]<<" "<<select_lines[j][1]<<endl;
            }
        }


		return (staff_index);
		/////////////////////////////////////////////

		// cout<<"address of staff_num_array = "<< staff_num_array<<endl;
	}
	else{
        cout<<"ntmd沒有線找啥米呀ˊ口ˋ"<<endl;
    }
}

