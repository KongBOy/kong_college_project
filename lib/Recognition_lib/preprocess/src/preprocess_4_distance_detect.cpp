/*
這個程式的功能是：
分析 preprocess_3找出的 可能是五線譜的線 之間的距離，找出五線譜線的距離
*/
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <iomanip>

#include "preprocess_0_hough_tool.h"
#include "preprocess_4_distance_detect.h"
using namespace cv;
using namespace std;

#define CHANGE_TOLORANCE 4
#define ENOUGH_LINE_HAVE_SAME_DISTANCE 3
#define STAFF_LINE_INTER_DISTANCE 11


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

void bubbleSort(int amount, int*& data){
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

int * Distance_detect(vector<Vec2f> mountain_lines, bool debuging){
    // 找線之間距離的層級

    // 先排好rho再微分 來看 rho的變化量 即 distance, 之後再對distance排序 即可看出線之間的距離有哪些種類 , preprocess_3 找出 lines 已經有對 rho 做排序了 這邊就不用排囉
    int * rho = new int[mountain_lines.size()];
    for(int i = 0 ; i < mountain_lines.size() ; i++) rho[i] = mountain_lines[i][0];

    int * dist = new int[mountain_lines.size()-1];
    for(int i = 0 ; i < mountain_lines.size() -1 ; i++) dist[i] = rho[i+1] - rho[i];
    bubbleSort(mountain_lines.size()-1, dist);

    // distance先排好之後再微分(上面已排) 來看 distance的變化量, 可看出不同distance層級之間的變化
    int * dist_diffi_1 = new int[mountain_lines.size()-1-1];
    for(int i = 0 ; i < mountain_lines.size() -1 -1; i++) dist_diffi_1[i] = dist[i+1] - dist[i];
    if(debuging){
        for(int i = 0 ; i < mountain_lines.size() -1 -1; i++) cout << "rho[" << setw(2) << i << "]=" << setw(4) << rho[i] << " , dist(sorted, the order is not related to rho)[" << setw(2)  << i                           << "] = " << setw(4) << dist[i]                           << " , dist_diffi_1[" << setw(2) << i << "] = " << setw(4) << dist_diffi_1[i] << endl;
        cout << "rho[" << setw(2) << mountain_lines.size() -1 -1 << "]=" << setw(4)  << rho[mountain_lines.size() -1 -1]  << " , dist(sorted, the order is not related to rho)[" << setw(2)  << mountain_lines.size() -1 -1 << "] = " << setw(4) << dist[mountain_lines.size() -1 -1] << endl;
        cout << "rho[" << setw(2) << mountain_lines.size() -1    << "]=" << setw(4)  << rho[mountain_lines.size() -1   ]  << setw(2) << endl;
        cout << endl;
        // 直接看結果比較好理解
        // rho[ 0]= 889 , dist[ 0] =    1 , dist_diffi_1[ 0] =    0
        // rho[ 1]= 891 , dist[ 1] =    1 , dist_diffi_1[ 1] =    0
        // rho[ 2]= 906 , dist[ 2] =    1 , dist_diffi_1[ 2] =    0
        // rho[ 3]= 908 , dist[ 3] =    1 , dist_diffi_1[ 3] =    0
        // rho[ 4]= 925 , dist[ 4] =    1 , dist_diffi_1[ 4] =    0
        // rho[ 5]= 926 , dist[ 5] =    1 , dist_diffi_1[ 5] =    0
        // rho[ 6]= 943 , dist[ 6] =    1 , dist_diffi_1[ 6] =    0
        // rho[ 7]= 945 , dist[ 7] =    1 , dist_diffi_1[ 7] =    0
        // rho[ 8]= 961 , dist[ 8] =    1 , dist_diffi_1[ 8] =    0
        // rho[ 9]= 963 , dist[ 9] =    1 , dist_diffi_1[ 9] =    0
        // rho[10]=1204 , dist[10] =    1 , dist_diffi_1[10] =    0
        // rho[11]=1206 , dist[11] =    1 , dist_diffi_1[11] =    1
        // rho[12]=1223 , dist[12] =    2 , dist_diffi_1[12] =    0
        // rho[13]=1224 , dist[13] =    2 , dist_diffi_1[13] =    0
        // rho[14]=1240 , dist[14] =    2 , dist_diffi_1[14] =    0
        // rho[15]=1242 , dist[15] =    2 , dist_diffi_1[15] =    0
        // rho[16]=1258 , dist[16] =    2 , dist_diffi_1[16] =    0
        // rho[17]=1260 , dist[17] =    2 , dist_diffi_1[17] =    0
        // rho[18]=1276 , dist[18] =    2 , dist_diffi_1[18] =    0
        // rho[19]=1278 , dist[19] =    2 , dist_diffi_1[19] =    0
        // rho[20]=1520 , dist[20] =    2 , dist_diffi_1[20] =    0
        // rho[21]=1536 , dist[21] =    2 , dist_diffi_1[21] =    0
        // rho[22]=1537 , dist[22] =    2 , dist_diffi_1[22] =    0
        // rho[23]=1554 , dist[23] =    2 , dist_diffi_1[23] =    0
        // rho[24]=1555 , dist[24] =    2 , dist_diffi_1[24] =    0
        // rho[25]=1572 , dist[25] =    2 , dist_diffi_1[25] =    0
        // rho[26]=1573 , dist[26] =    2 , dist_diffi_1[26] =    0
        // rho[27]=1589 , dist[27] =    2 , dist_diffi_1[27] =    0
        // rho[28]=1590 , dist[28] =    2 , dist_diffi_1[28] =    0
        // rho[29]=1830 , dist[29] =    2 , dist_diffi_1[29] =    0
        // rho[30]=1831 , dist[30] =    2 , dist_diffi_1[30] =    0
        // rho[31]=1834 , dist[31] =    2 , dist_diffi_1[31] =    0
        // rho[32]=1848 , dist[32] =    2 , dist_diffi_1[32] =    0
        // rho[33]=1849 , dist[33] =    2 , dist_diffi_1[33] =    1
        // rho[34]=1852 , dist[34] =    3 , dist_diffi_1[34] =    0
        // rho[35]=1865 , dist[35] =    3 , dist_diffi_1[35] =    0
        // rho[36]=1867 , dist[36] =    3 , dist_diffi_1[36] =    0
        // rho[37]=1883 , dist[37] =    3 , dist_diffi_1[37] =    0
        // rho[38]=1885 , dist[38] =    3 , dist_diffi_1[38] =    0
        // rho[39]=1901 , dist[39] =    3 , dist_diffi_1[39] =    0
        // rho[40]=1902 , dist[40] =    3 , dist_diffi_1[40] =    0
        // rho[41]=1905 , dist[41] =    3 , dist_diffi_1[41] =    0
        // rho[42]=2144 , dist[42] =    3 , dist_diffi_1[42] =    0
        // rho[43]=2147 , dist[43] =    3 , dist_diffi_1[43] =    0
        // rho[44]=2161 , dist[44] =    3 , dist_diffi_1[44] =    0
        // rho[45]=2162 , dist[45] =    3 , dist_diffi_1[45] =   10
        // rho[46]=2165 , dist[46] =   13 , dist_diffi_1[46] =    1
        // rho[47]=2180 , dist[47] =   14 , dist_diffi_1[47] =    0
        // rho[48]=2182 , dist[48] =   14 , dist_diffi_1[48] =    0
        // rho[49]=2196 , dist[49] =   14 , dist_diffi_1[49] =    0
        // rho[50]=2197 , dist[50] =   14 , dist_diffi_1[50] =    1
        // rho[51]=2200 , dist[51] =   15 , dist_diffi_1[51] =    0
        // rho[52]=2214 , dist[52] =   15 , dist_diffi_1[52] =    0
        // rho[53]=2215 , dist[53] =   15 , dist_diffi_1[53] =    0
        // rho[54]=2218 , dist[54] =   15 , dist_diffi_1[54] =    1
        // rho[55]=2456 , dist[55] =   16 , dist_diffi_1[55] =    0
        // rho[56]=2458 , dist[56] =   16 , dist_diffi_1[56] =    0
        // rho[57]=2461 , dist[57] =   16 , dist_diffi_1[57] =    0
        // rho[58]=2476 , dist[58] =   16 , dist_diffi_1[58] =    0
        // rho[59]=2478 , dist[59] =   16 , dist_diffi_1[59] =    0
        // rho[60]=2494 , dist[60] =   16 , dist_diffi_1[60] =    0
        // rho[61]=2496 , dist[61] =   16 , dist_diffi_1[61] =    0
        // rho[62]=2512 , dist[62] =   16 , dist_diffi_1[62] =    0
        // rho[63]=2514 , dist[63] =   16 , dist_diffi_1[63] =    0
        // rho[64]=2530 , dist[64] =   16 , dist_diffi_1[64] =    0
        // rho[65]=2532 , dist[65] =   16 , dist_diffi_1[65] =    0
        // rho[66]=2775 , dist[66] =   16 , dist_diffi_1[66] =    0
        // rho[67]=2777 , dist[67] =   16 , dist_diffi_1[67] =    0
        // rho[68]=2793 , dist[68] =   16 , dist_diffi_1[68] =    0
        // rho[69]=2795 , dist[69] =   16 , dist_diffi_1[69] =    0
        // rho[70]=2811 , dist[70] =   16 , dist_diffi_1[70] =    1
        // rho[71]=2813 , dist[71] =   17 , dist_diffi_1[71] =    0
        // rho[72]=2829 , dist[72] =   17 , dist_diffi_1[72] =    0
        // rho[73]=2832 , dist[73] =   17 , dist_diffi_1[73] =    0
        // rho[74]=2847 , dist[74] =   17 , dist_diffi_1[74] =    0
        // rho[75]=2850 , dist[75] =   17 , dist_diffi_1[75] =    0
        // rho[76]=3097 , dist[76] =   17 , dist_diffi_1[76] =    0
        // rho[77]=3100 , dist[77] =   17 , dist_diffi_1[77] =  221
        // rho[78]=3116 , dist[78] =  238 , dist_diffi_1[78] =    1
        // rho[79]=3118 , dist[79] =  239 , dist_diffi_1[79] =    1
        // rho[80]=3135 , dist[80] =  240 , dist_diffi_1[80] =    1
        // rho[81]=3137 , dist[81] =  241 , dist_diffi_1[81] =    1
        // rho[82]=3153 , dist[82] =  242 , dist_diffi_1[82] =    1
        // rho[83]=3155 , dist[83] =  243 , dist_diffi_1[83] =    4
        // rho[84]=3172 , dist[84] =  247
        // rho[85]=3175
    }

    // 由上面的觀察可以知道, 走訪 dist_diffi_1 如果累積到足夠的數值 就換到下一個level 的 distance了, 
    int * dist_level = new int[100];
    for(int i = 0 ; i < 100 ; i++) dist_level[i] = 0;
    int dist_level_count = 0;  // 找到的 distance_level數量
    int flat_step = 0;         // 要有足夠的線有相同的距離來排除雜線

    int tolorate_count = 0;    // 在 dist_diffi_1 array裡面跑 且一邊 紀累積量的東西，當可忍受的值爆表，就找到了層級階梯的腳腳拉
    for(int i = 0 ; i < mountain_lines.size() -1 -1; i++){
        flat_step++;
        tolorate_count += dist_diffi_1[i];
        if(tolorate_count > CHANGE_TOLORANCE && flat_step >= ENOUGH_LINE_HAVE_SAME_DISTANCE){
            dist_level[dist_level_count++] = dist[i];
            tolorate_count = 0;  // 找下一個階梯，所以歸零囉~~
            flat_step = 0;
        }
    }
    if(debuging){
        cout << "totally find out " << dist_level_count << " distance_level" << endl;
        for(int i = 0 ; i < dist_level_count ; i++) cout << "dist_level[" << i << "]= " << dist_level[i] << endl;
        cout << endl;
        // totally find out 4 distance_level
        // dist_level[0]= 3
        // dist_level[1]= 16
        // dist_level[2]= 17
        // dist_level[3]= 241
    }

    // 注意要 i = 1 開始喔！
    //   要再做一次的主要原因是，因為上面會受到 一條粗條線但找到很多條細線 的干擾，上面的dist_level[0]就已經找到了這種的 dist_level 大概在哪，
    //   就直接在用dist_level[0]為底再來篩一次就可以更準的找到五線譜或其他他家有共同距離的距離囉
    /////////// COPY START //////////
    for(int i = 1 ; i < dist_level_count -1 ; i++){
        if(dist_level[i+1] - dist_level[i] < STAFF_LINE_INTER_DISTANCE ){
            array_position_erase(dist_level, i, dist_level_count);
            i--;
        }
    }
    if(debuging){
        cout << "totally find out " << dist_level_count << " distance_level" << endl;
        for(int i = 0 ; i < dist_level_count ; i++) cout << "dist_level[" << i << "]= " << dist_level[i] << endl;
        cout << endl;
        // totally find out 3 distance_level
        // dist_level[0]= 3
        // dist_level[1]= 17
        // dist_level[2]= 241
    }
    ///////// COPY END ////////////

    // 誤差要補上, 因為上面算的是 粗線裡面的最下一條 到下一條粗線的最上一條 所以有點誤差
    // for(int i = 0 ; i < dist_level_count ; i++) dist_level[i] += 3;
    dist_level[0] += 0;
    dist_level[1] += 5;

    return dist_level;
}
