/*
這個程式的功能是：
分析 preprocess_3找出的 可能是五線譜的線 之間的距離，找出五線譜線的距離
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "preprocess_0_hough_tool.h"
#include "preprocess_4_distance_detect.h"
using namespace cv;
using namespace std;

#define CHANGE_DRAMATICLY_LEVEL_1 3
#define CHANGE_DRAMATICLY_LEVEL_2 10
#define CHANGE_DRAMATICLY_LEVEL_3 30
#define ENOUGH_PEOPLE_HAVE_SAME_DISTANCE 3

void array_position_erase(int* src_array , int position , int& array_size){
    if(array_size > 0){
        if(array_size == 1){
            src_array[0] = 0;
            array_size--;
            return;
        }
        else{
            if(position == array_size -1){ //刪最後一個
            
                array_size--;
                return;
            }
            else{
                for(int i = position ; i < array_size -1 ; i++){
                    src_array[i]= src_array[i+1];
                }
                array_size--;
                return;
            }
        }
    }
    else{
        cout << "沒有線可以刪，沒有做動作" << endl;
        return;
    }
}

void bubbleSort(int amount,int *& data){
	for(int i = 0 ; i < amount-1 ; i++){
		for(int j = i+1 ; j < amount ; j++){
			if( data[i] > data[j] ){
				int temp0 = data[i];
				data[i]= data[j];
				data[j]= temp0;
			}
		}
	}
}

void Distance_detect(vector<Vec2f> mountain_lines2 , int *& dist_level){
    int * rho = new int[mountain_lines2.size()];
    for(int i = 0 ; i < mountain_lines2.size() ; i++) rho[i] = mountain_lines2[i][0];

    // 精隨：先排好，再微分
    bubbleSort(mountain_lines2.size(),rho);
    int * distance_0 = new int[mountain_lines2.size()-1];
    for(int i = 0 ; i < mountain_lines2.size() -1 ; i++) distance_0[i] = 0;
    for(int i = 0 ; i < mountain_lines2.size() -1 ; i++){
        distance_0[i] = rho[i+1] - rho[i];
        // cout << "distance_0[" << i << "] = " << distance_0[i] << endl;
    }





    // 精隨：先排好，再微分
    bubbleSort(mountain_lines2.size()-1,distance_0);
    // for(int i = 0 ; i < mountain_lines2.size() -1 ; i++) cout << "distance_0[" << i << "] = " << distance_0[i] << endl;
    // cout << endl;
    int * distance_1 = new int[mountain_lines2.size()-1-1];
    for(int i = 0 ; i < mountain_lines2.size() -1-1 ; i++) distance_1[i] = 0;

    for(int i = 0 ; i < mountain_lines2.size() -1 -1; i++){
        distance_1[i] = distance_0[i+1] - distance_0[i];
        cout << "rho[" << i << "]=" << rho[i] << " , distance_0[" << i << "] = " << distance_0[i] << " , distance_1[" << i << "] = " << distance_1[i] << endl;
    }
     cout << "rho[" << mountain_lines2.size() -1 -1 << "]=" << rho[mountain_lines2.size() -1 -1] << " , distance_0[" << mountain_lines2.size() -1 -1 << "] = " << distance_0[mountain_lines2.size() -1 -1] << endl;
     cout << "rho[" << mountain_lines2.size() -1    << "]=" << rho[mountain_lines2.size() -1   ] << endl;

    // cout << "distance_0[" << mountain_lines.size()-1 << "] = " << distance_0[mountain_lines.size()-1] << endl;

    cout << endl;

    dist_level = new int[100];
    // int dist_level[100];
    for(int i = 0 ; i < 100 ; i++) dist_level[i] = 0;
    int go_dist_level_count = 0;  // 同時在distance_level裡面走，走完順便可以當 找到幾個level的容器
    int flat_step = 0;  // 要有足夠的人有相同的距離，排除雜線

    int tolorate_count = 0;  // 在 distance_1 array裡面跑 且一邊 記變化量的東西，當可忍受的值爆表，就找到了階梯的腳腳拉
    for(int i = 0 ; i < mountain_lines2.size() -1 -1; i++){
        flat_step++;
        tolorate_count += distance_1[i];
        if(tolorate_count > CHANGE_DRAMATICLY_LEVEL_1 && flat_step >= ENOUGH_PEOPLE_HAVE_SAME_DISTANCE){
            dist_level[go_dist_level_count++] = distance_0[i];
            tolorate_count = 0;  // 找下一個階梯，所以歸零囉~~
            flat_step = 0;
        }
    }
    cout << "總共找到了 " << go_dist_level_count << "個 distance_level" << endl;
    for(int i = 0 ; i < go_dist_level_count ; i++) cout << "dist_level[" << i << "]= " << dist_level[i] << endl;
    cout << endl;


    // 注意要 i = 1 開始喔！
    //   要再做一次的主要原因是，因為上面會受到 本來是同條線但找到很多條 的干擾，上面的dist_level[0]就已經找到了這level在哪，
    //   就直接在用dist_level[0]為底再來篩一次就可以更準的找到五線譜或其他他家有共同距離的距離囉
    /////////// COPY START //////////
    for(int i = 1 ; i < go_dist_level_count -1 ; i++){
        // +3是怕誤差，因為上面的偵測是有雜訊的(想想如果刪掉一條線，那刪掉的該條的上一條線到下一條線的距離就會增加~~~，now是假設刪掉了，所以要加上些誤差
        if(dist_level[i+1] - dist_level[i] < dist_level[0]*5){  // CHANGE_DRAMATICLY_LEVEL_2)
            array_position_erase(dist_level,i,go_dist_level_count);
            i--;
        }
    }
    cout << "總共找到了 " << go_dist_level_count << "個 distance_level" << endl;
    for(int i = 0 ; i < go_dist_level_count ; i++) cout << "dist_level[" << i << "]= " << dist_level[i] << endl;
    cout << endl;
    ///////// COPY END ////////////


    // 怕誤差呀~~~(想想如果刪掉一條線，那刪掉的該條的上一條線到下一條線的距離就會增加~~~，now是假設刪掉了，所以要加上些誤差
    // for(int i = 0 ; i < go_dist_level_count ; i++) dist_level[i] += 3;
    dist_level[0] += 0;
    dist_level[1] += 5;
}


