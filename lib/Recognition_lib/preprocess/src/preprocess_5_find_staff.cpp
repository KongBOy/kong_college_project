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

#include "Recognition.h"

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
        cout << "no line, not erase line" << endl;
        return;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Recognition_page::filter_multi_same_line(){
    debuging = debuging_pre5;
    // 去除重複線, 走訪每一條線, 第二條線以後 和前一條線比 如果太相近 或 差太多 就去掉
	for(int i = 0 ; i < lines.size() ; i++){
        // 每一條線都不可以跟90度差太遠, 差太遠就不存, 經過很多圖片測試(連手寫校歌也測了)， < 5 通常就是同條線
        if( abs( (lines[i][1]/PI*180) - 90 ) > 5 ){
            position_erase(lines, i);
            i--;
        }

        // 第二條線以後要還要跟前一條線比較距離，太近就代表同條線不用存喔，還有角度離上一條線差太多也不用存
        if( i >= 1){
            if(debuging) cout << "line_" << i << " and line_" << i - 1 << " distance: " << lines[i][0] - lines[i-1][0] << ", angle_diff: " << abs((lines[i-1][1]*180/PI) - (lines[i][1]*180/PI)) << " , ";
            
            // 太近就代表同條線不用存
            if( ( lines[i][0] - lines[i-1][0] <= dist_level[0] ) ){
                if(debuging) cout << "line_" << i <<" distance_too_short: " << lines[i][0] - lines[i-1][0] << endl;
                position_erase(lines, i);
                i--;
                continue;
            }
            // 如果稍近 但 角度離上一條線差太多也不用存, 測試後覺得 1.5度不錯
            if( ( lines[i][0] - lines[i-1][0] <= dist_level[0] * 2 ) && abs((lines[i-1][1]*180/PI) - (lines[i][1]*180/PI)) > 1.5 ){
                if(debuging) cout<< "line_" << i <<" angle_diff_too_big: " << abs((lines[i-1][1]*180/PI) - (lines[i][1]*180/PI)) << endl;
                position_erase(lines, i);
                i--;
                continue;
            }
            if(debuging) cout << endl;
        }
    }
    if(debuging) Watch_Hough_Line(lines);
    return;
}


//不要用 vector<Vec2f>*& staff 原因寫在下面
int Recognition_page::find_Staff_lines(){
    debuging = debuging_pre5;
    /*
    把 距離相近 和 角度相近的 的重複線 清除,
    再把 相近的線分成同一群
    每一群再根據 內部有幾條線來做處理
        a: == 5條, 直接過關
        b: >  5條, 調整 dist_th 弄成5條
        c: <  5條, 刪除
    所以線會被弄成 5條5條 一組,
    因此回傳 staff_amount 幾組5條 即可
    */
    if(debuging) cout << "find_Staff_lines" << endl;
    // 把可能是相同線的線先簡單的濾掉
    filter_multi_same_line();

    // 用更詳細的方式在濾一次, 或者把失敗的 那組五線譜整組拿掉
    //一、根據distance_level_1的距離來分類線

    //二、a、如果每個類別剛好五條，就成功
    //    b、如果大於五條，代表上面沒濾乾淨再濾一次
    //    c、如果小於五條，代表有問題把線那組線通通刪掉
    if(lines.size() > 0){

    //一： 分群, 目前的線 與 自己上一條的線 距離 如果小於 staff_inter_distance 就算同一群
        // 先把 所有 目前的線 與 自己上一條的線 距離 算出來
		int * distance = new int[lines.size()];
		distance[0] = lines[0][0];
		for(int i = 1 ; i < lines.size() ; i++) distance[i] = lines[i][0] - lines[i-1][0];
        
        if(debuging){
            cout << "i = " << setw(2) << '0' << " distance = " << distance[0] << endl;
            for(int i = 1 ; i < lines.size() ; i++) cout<<"i = " << setw(2) << i << " distance = " << distance[i] << endl;
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

		// 距離夠短就分成一群, 每條線 根據自己跟上一條線的距離 和 五線譜線內部的距離(dist_level[1]) 比對來做編號
		int group_index = 0;                           // 標示第幾組五線譜
		int * line_num_array = new int[lines.size()];  // 標示 每條線 屬於第幾組五線譜 的容器
		line_num_array[0] = 0;                         // 第一條線 一定是第0組
		const int staff_inter_distance = dist_level[1] + STAFF_LINE_DISTANCE_ERROR;  // + 3還是怕誤差, 反正目前的五線譜內的距離 跟五線譜間的距離還是有點差距, 所以 +3 問題不大
        // 走訪每條線, 如果跟上一條線的距離 <= staff_inter_distance, 屬於同一組五線譜, 如果 > staff_inter_distance, 就屬於下一組五線譜
		for(int i = 1 ; i < lines.size(); i++){
            if(distance[i] <= staff_inter_distance) line_num_array[i] = group_index;
			else line_num_array[i] = ++group_index;
		}
        // 每一條線屬於哪一群顯示一下
        if(debuging) for(int i = 0 ; i < lines.size(); i++) cout<<"i:" << i <<", line_group:" << line_num_array[i] << endl;
        
    // 二：
		// 統計每個群 裡面的線數量
		int * gid_count_array = new int[group_index + 1];                               // 建立   每一群 有多少線的容器, group_index 是 index喔!!! index轉數量要 "+1"
		for(int i = 0 ; i <= group_index  ; i++) gid_count_array[i] = 0;                // 初始化 每一群 有多少線的容器, group_index 是 index喔!!! 所以用 "=", 初始化 array
		for(int i = 0 ; i <  lines.size() ; i++) gid_count_array[line_num_array[i]]++;  // 走訪每條線 的 群組位置 在該位置++ 來統計 每個群組 共有幾條線

		// 如果群內 線個數有剛好五個的，代表找到五線譜了, 存一下group_id
		int * staff_group_ids  = new int[group_index + 1];  // group_index 是 index喔!!! index轉數量要 "+1"
		int staff_amount = 0;  // 有過關的 幾組數量 紀錄在這裡

		int go_line = 0;  // 現在在第幾條線
        // 走訪每一群
		for(int go_group = 0 ; go_group <= group_index ; go_group++){  // group_index 是 index喔!!! 所以用 "="
            // b: 如果線大於五條的話，慢慢增加 distance_threshold 再偵測一次 且 角度要盡可能靠近 群內平均角度, 直到五條以內
		    if(gid_count_array[go_group] > 5){
                if(debuging) cout << "case b, > 5 :" << endl;

                // distance_threshold, 從最小的 dist_level[0] 開始慢慢往上++
                int go_dist_th = dist_level[0];

                // 顯示一下 目前有的資訊
                if(debuging) cout << "inital: " <<  "go_group:" << go_group << " have " << gid_count_array[go_group] << "line" << ", go_dist_th:" << go_dist_th << ", go_line:" << go_line <<  endl;

                // 群內平均角度
                float ave_theta = 0;
                for(int j = go_line ; j < gid_count_array[go_group]+ go_line ; j++) ave_theta += lines[j][1];
                ave_theta /= gid_count_array[go_group];

                do{
                    // 走訪群內每一條線
                    for(int go_group_line = go_line; go_group_line < gid_count_array[go_group] + go_line; go_group_line++){
                        // 看此條 與 下一條線的距離, 如果 > go_dist_th 就進一步 比較 與群內的角度差, 差比較多的那條線刪除
                        // 防呆, 因為是 現在這條 跟 下一條 相減來看距離，所以最後一條不能做(因為沒有下一條了)，一定要這個if
                        if( go_group_line < gid_count_array[go_group]+ go_line -1){
                            // 不用distance是因為, 下面會用到 position_erase, erase以後這樣子會跟distance 對不上,
                            // 除非 position_erase 一次就重新更新一次 distance, 那這樣就乾脆 直接計算 此條 與 下一條的距離 就好啦
                            if( (lines[go_group_line + 1][0] - lines[go_group_line][0]) < go_dist_th ){
                                // 比較 與群內的角度差, 差比較多的那條線刪除
                                if(abs(lines[go_group_line+1][1] - ave_theta) > abs(lines[go_group_line][1] - ave_theta)){
                                    position_erase(lines, go_group_line + 1);
                                    gid_count_array[go_group]--;
                                    go_group_line--;
                                }
                                else{
                                    position_erase(lines, go_group_line);
                                    gid_count_array[go_group]--;
                                    go_group_line--;
                                }
                            }
                            else;  // 距離在 dist_th以內過關, do nothing 換下一條線繼續做
                            
                            // 如果 已經是5條了 就不用跑完 break 出去囉
                            if(gid_count_array[go_group] == 5) break;
                        }
                    }
                    if(debuging) cout << "after filter: " << "go_group:" << go_group << " have " << gid_count_array[go_group] << "line" << ", go_dist_th:" << go_dist_th << ", go_line:" << go_line << endl;
                    
                    // go_dist_th++ 完 判斷 如果此群 還是超過5條就繼續做
                    go_dist_th++;
                } while(gid_count_array[go_group] > 5);
                if(debuging) cout<<"case b, > 5 finish, go_dist_th=" << go_dist_th << endl;

                // go_line 跳轉到下一個群 的線
                go_line += gid_count_array[go_group];
                // 紀錄一下 此組候選群 
                staff_group_ids[staff_amount++] = go_group;
            }

            // c: 如果線小於五條的話, 把這個群的線都刪除,
            //    然後注意 go_group不需要-- 或 刪除group喔, 因為我是用 go_line 來走訪所有線, 只要這裡 go_line 不+-, 就會在原地不動這樣子
            else if(gid_count_array[go_group] < 5){
                if(debuging) cout << "case c, < 5, " << "go_group:" << go_group << ", delete its " << gid_count_array[go_group] << " lines" << endl;
                for(int j = 0 ; j < gid_count_array[go_group] ; j++){  // 這裡的j 表示的是for跑的次數, 所以看起來像是沒用到這樣子
                    position_erase(lines, go_line);
                    if(debuging) cout<<"    delete go_line = " << go_line << endl;
                }
            }

            // a: 如果等於五條線的話, 正確 直接儲存進 候選staff_index, go_line 跳轉到下一個群 的線
			else if(gid_count_array[go_group] ==5){
                if(debuging) cout << "case a, == 5, " << "go_group:" << go_group << ", save its " << gid_count_array[go_group] << " lines" << endl;
                go_line += 5;
                staff_group_ids[staff_amount++] = go_group;
            }

		}
		if(debuging) for(int i = 0 ; i < staff_amount ; i++) cout << "Group id:" << staff_group_ids[i] << " is staff" << endl;
		return staff_amount;
	}
	else{
        if(debuging) cout << "no line" << endl;
        return 0;  // 沒有 line 一定就 沒有五線譜, return 0 代表0組
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
            if(debuging) cout << "line_" << i << " and line_" << i - 1 << " distance: " << src_lines[i][0] - src_lines[i-1][0] << ", angle_diff: " << abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) << " , ";
            
            // 太近就代表同條線不用存
            if( ( src_lines[i][0] - src_lines[i-1][0] <= line_distance_err ) ){
                if(debuging) cout << "line_" << i <<" distance_too_short: " << src_lines[i][0] - src_lines[i-1][0] << endl;
                position_erase(src_lines, i);
                i--;
                continue;
            }
            // 如果稍近 但 角度離上一條線差太多也不用存, 測試後覺得 1.5度不錯
            if( ( src_lines[i][0] - src_lines[i-1][0] <= line_distance_err * 2 ) && abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) > 1.5 ){
                if(debuging) cout<< "line_" << i <<" angle_diff_too_big: " << abs((src_lines[i-1][1]*180/PI) - (src_lines[i][1]*180/PI)) << endl;
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


//不要用 vector<Vec2f>*& staff 原因寫在下面
int find_Staff_lines(vector<Vec2f>& select_lines, int dist_level_0, int dist_level_1, bool debuging){
    /*
    把 距離相近 和 角度相近的 的重複線 清除,
    再把 相近的線分成同一群
    每一群再根據 內部有幾條線來做處理
        a: == 5條, 直接過關
        b: >  5條, 調整 dist_th 弄成5條
        c: <  5條, 刪除
    所以線會被弄成 5條5條 一組,
    因此回傳 staff_amount 幾組5條 即可
    */
    if(debuging) cout << "find_Staff_lines" << endl;
    // 把可能是相同線的線先簡單的濾掉
    filter_multi_same_line(select_lines, dist_level_0, debuging);

    // 用更詳細的方式在濾一次, 或者把失敗的 那組五線譜整組拿掉
    //一、根據distance_level_1的距離來分類線

    //二、a、如果每個類別剛好五條，就成功
    //    b、如果大於五條，代表上面沒濾乾淨再濾一次
    //    c、如果小於五條，代表有問題把線那組線通通刪掉
    if(select_lines.size() > 0){

    //一： 分群, 目前的線 與 自己上一條的線 距離 如果小於 staff_inter_distance 就算同一群
        // 先把 所有 目前的線 與 自己上一條的線 距離 算出來
		int * distance = new int[select_lines.size()];
		distance[0] = select_lines[0][0];
		for(int i = 1 ; i < select_lines.size() ; i++) distance[i] = select_lines[i][0] - select_lines[i-1][0];
        
        if(debuging){
            cout << "i = " << setw(2) << '0' << " distance = " << distance[0] << endl;
            for(int i = 1 ; i < select_lines.size() ; i++) cout<<"i = " << setw(2) << i << " distance = " << distance[i] << endl;
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

		// 距離夠短就分成一群, 每條線 根據自己跟上一條線的距離 和 五線譜線內部的距離(dist_level_1) 比對來做編號
		int group_index = 0;                                  // 標示第幾組五線譜
		int * line_num_array = new int[select_lines.size()];  // 標示 每條線 屬於第幾組五線譜 的容器
		line_num_array[0] = 0;                                // 第一條線 一定是第0組
		const int staff_inter_distance = dist_level_1 + STAFF_LINE_DISTANCE_ERROR;  // + 3還是怕誤差, 反正目前的五線譜內的距離 跟五線譜間的距離還是有點差距, 所以 +3 問題不大
        // 走訪每條線, 如果跟上一條線的距離 <= staff_inter_distance, 屬於同一組五線譜, 如果 > staff_inter_distance, 就屬於下一組五線譜
		for(int i = 1 ; i < select_lines.size(); i++){
            if(distance[i] <= staff_inter_distance) line_num_array[i] = group_index;
			else line_num_array[i] = ++group_index;
		}
        // 每一條線屬於哪一群顯示一下
        if(debuging) for(int i = 0 ; i < select_lines.size(); i++) cout<<"i:" << i <<", line_group:" << line_num_array[i] << endl;
        
    // 二：
		// 統計每個群 裡面的線數量
		int * gid_count_array = new int[group_index + 1];                                      // 建立   每一群 有多少線的容器, group_index 是 index喔!!! index轉數量要 "+1"
		for(int i = 0 ; i <= group_index         ; i++) gid_count_array[i] = 0;                // 初始化 每一群 有多少線的容器, group_index 是 index喔!!! 所以用 "=", 初始化 array
		for(int i = 0 ; i <  select_lines.size() ; i++) gid_count_array[line_num_array[i]]++;  // 走訪每條線 的 群組位置 在該位置++ 來統計 每個群組 共有幾條線

		// 如果群內 線個數有剛好五個的，代表找到五線譜了, 存一下group_id
		int * staff_group_ids  = new int[group_index + 1];  // group_index 是 index喔!!! index轉數量要 "+1"
		int staff_amount = 0;  // 有過關的 幾組數量 紀錄在這裡

		int go_line = 0;  // 現在在第幾條線
        // 走訪每一群
		for(int go_group = 0 ; go_group <= group_index ; go_group++){  // group_index 是 index喔!!! 所以用 "="
            // b: 如果線大於五條的話，慢慢增加 distance_threshold 再偵測一次 且 角度要盡可能靠近 群內平均角度, 直到五條以內
		    if(gid_count_array[go_group] > 5){
                if(debuging) cout << "case b, > 5 :" << endl;

                // distance_threshold, 從最小的 dist_level_0 開始慢慢往上++
                int go_dist_th = dist_level_0;

                // 顯示一下 目前有的資訊
                if(debuging) cout << "inital: " <<  "go_group:" << go_group << " have " << gid_count_array[go_group] << "line" << ", go_dist_th:" << go_dist_th << ", go_line:" << go_line <<  endl;

                // 群內平均角度
                float ave_theta = 0;
                for(int j = go_line ; j < gid_count_array[go_group]+ go_line ; j++) ave_theta += select_lines[j][1];
                ave_theta /= gid_count_array[go_group];

                do{
                    // 走訪群內每一條線
                    for(int go_group_line = go_line; go_group_line < gid_count_array[go_group] + go_line; go_group_line++){
                        // 看此條 與 下一條線的距離, 如果 > go_dist_th 就進一步 比較 與群內的角度差, 差比較多的那條線刪除
                        // 防呆, 因為是 現在這條 跟 下一條 相減來看距離，所以最後一條不能做(因為沒有下一條了)，一定要這個if
                        if( go_group_line < gid_count_array[go_group]+ go_line -1){
                            // 不用distance是因為, 下面會用到 position_erase, erase以後這樣子會跟distance 對不上,
                            // 除非 position_erase 一次就重新更新一次 distance, 那這樣就乾脆 直接計算 此條 與 下一條的距離 就好啦
                            if( (select_lines[go_group_line + 1][0] - select_lines[go_group_line][0]) < go_dist_th ){
                                // 比較 與群內的角度差, 差比較多的那條線刪除
                                if(abs(select_lines[go_group_line+1][1] - ave_theta) > abs(select_lines[go_group_line][1] - ave_theta)){
                                    position_erase(select_lines, go_group_line + 1);
                                    gid_count_array[go_group]--;
                                    go_group_line--;
                                }
                                else{
                                    position_erase(select_lines, go_group_line);
                                    gid_count_array[go_group]--;
                                    go_group_line--;
                                }
                            }
                            else;  // 距離在 dist_th以內過關, do nothing 換下一條線繼續做
                            
                            // 如果 已經是5條了 就不用跑完 break 出去囉
                            if(gid_count_array[go_group] == 5) break;
                        }
                    }
                    if(debuging) cout << "after filter: " << "go_group:" << go_group << " have " << gid_count_array[go_group] << "line" << ", go_dist_th:" << go_dist_th << ", go_line:" << go_line << endl;
                    
                    // go_dist_th++ 完 判斷 如果此群 還是超過5條就繼續做
                    go_dist_th++;
                } while(gid_count_array[go_group] > 5);
                if(debuging) cout<<"case b, > 5 finish, go_dist_th=" << go_dist_th << endl;

                // go_line 跳轉到下一個群 的線
                go_line += gid_count_array[go_group];
                // 紀錄一下 此組候選群 
                staff_group_ids[staff_amount++] = go_group;
            }

            // c: 如果線小於五條的話, 把這個群的線都刪除,
            //    然後注意 go_group不需要-- 或 刪除group喔, 因為我是用 go_line 來走訪所有線, 只要這裡 go_line 不+-, 就會在原地不動這樣子
            else if(gid_count_array[go_group] < 5){
                if(debuging) cout << "case c, < 5, " << "go_group:" << go_group << ", delete its " << gid_count_array[go_group] << " lines" << endl;
                for(int j = 0 ; j < gid_count_array[go_group] ; j++){  // 這裡的j 表示的是for跑的次數, 所以看起來像是沒用到這樣子
                    position_erase(select_lines, go_line);
                    if(debuging) cout<<"    delete go_line = " << go_line << endl;
                }
            }

            // a: 如果等於五條線的話, 正確 直接儲存進 候選staff_index, go_line 跳轉到下一個群 的線
			else if(gid_count_array[go_group] ==5){
                if(debuging) cout << "case a, == 5, " << "go_group:" << go_group << ", save its " << gid_count_array[go_group] << " lines" << endl;
                go_line += 5;
                staff_group_ids[staff_amount++] = go_group;
            }

		}
		if(debuging) for(int i = 0 ; i < staff_amount ; i++) cout << "Group id:" << staff_group_ids[i] << " is staff" << endl;
		return staff_amount;
	}
	else{
        if(debuging) cout << "no line" << endl;
        return 0;  // 沒有 line 一定就 沒有五線譜, return 0 代表0組
    }
}

