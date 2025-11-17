/*
這個程式的功能是：
找出符號的音高
*/
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include <cstring>
#include <direct.h>
#include <iomanip>

#include "Recognition.h"
#include "recognition_4_find_picth.h"

using namespace cv;
using namespace std;


int find_staff_shift(const Mat cut_ord_img, int first_line_y_ord, int go_staff_x, int go_staff_y, Mat& debug_img, bool debuging){
    // 從五線譜的第一條線 往上往下搜尋 哪裡有 線的上邊緣(本格會是黑色 前一格會是白色), 如果第一條線搜尋失敗, 會依序換第二~五條來搜尋
    int go_staff_shift = 0;
    while(true){
        // 因為怕只看第一條線會有符桿或音符頭影響的問題，所以多跑幾條，只要找到 shift 就return 就好囉！
        // 我自己的想像是 一定會有可以測到的，要不然我們人類怎麼分辨呢, 所以大膽的用 while 不怕跳不出迴圈, 實際測試也確實如此
        // 走訪目前指定的五線譜 的每條線
        for(int go_staff_line = 0; go_staff_line < 5; go_staff_line++){
            go_staff_y = first_line_y_ord + 11 * go_staff_line;
            if(debuging) circle(debug_img, Point(go_staff_x, go_staff_y), 1, Scalar(165, 0, 151), 5);  // 紫色

            // 線的特性： 本格是黑色(x, y) 而 上面一格格是白色(x, y-1)
            // 往上下跑 測試 4格, 不能測太多格喔！！因為怕會定位定到別條五線譜就gg了！(五線譜間距11, 所以測 4格 就好)
            for(int go_shift = 0; go_shift <= 4; go_shift++ ){
                // 搜尋的地方畫一下確定沒找錯, 想要一個個row 看的話就把 waitKey 註解拿掉
                if(debuging){
                    cout << "down case, go_shift = " << go_shift << endl;
                    // 往上搜尋 上邊緣(上白下黑)
                    circle(debug_img, Point(go_staff_x, go_staff_y -go_shift), 1, Scalar(14, 105 + 30 * go_staff_line, 60 * go_staff_line), 1);  // 深綠色, 換線時顏色會越來越亮
                    // 往下搜尋 上邊緣(上白下黑)
                    circle(debug_img, Point(go_staff_x, go_staff_y +go_shift), 1, Scalar(14, 105 + 30 * go_staff_line, 60 * go_staff_line), 1);  // 深綠色, 換線時顏色會越來越亮
                    imshow("debug", debug_img);
                    // cv::waitKey(0);
                }
                // **** 往上 ****// 
                if( (cut_ord_img.at<uchar>(go_staff_y -1 -go_shift, go_staff_x) == 255) &&(cut_ord_img.at<uchar>(go_staff_y -go_shift, go_staff_x) == 0) ){
                    // 找到go_shift了，看要不要保留go_shift, 用線寬 來判斷 看要不要保留go_shift
                    // 找線寬~~~可以function化 不過短短的就直接寫近來好了~~
                    int check_width = 0;
                    int check_x = go_staff_x;
                    int check_y = go_staff_y -go_shift;
                    // 找 線寬
                    while(cut_ord_img.at<uchar>(check_y, check_x) == 0){
                        check_width++;
                        check_y++;
                    }
                    if(debuging) cout << "check_width = " << check_width << endl;

                    // 由 線寬 來判斷現在是不是 五線譜的線 而不是 符桿 或 音頭
                    if(check_width < 5){
                        if(debuging) line(debug_img, Point(go_staff_x, go_staff_y), Point(go_staff_x, go_staff_y + go_shift), Scalar(0, 255, 255), 1);  // 黃綠色
                        return  go_shift * -1;
                    }
                }
                // **** 往下 ****// 幾乎複製貼上~~上面的case，再小改即可~~
                else if( (cut_ord_img.at<uchar>(go_staff_y -1 +go_shift, go_staff_x) == 255) &&(cut_ord_img.at<uchar>(go_staff_y +go_shift, go_staff_x) == 0) ){
                    // 找到go_shift了，看要不要保留go_shift, 用線寬 來判斷 看要不要保留go_shift
                    // 找線寬~~~可以function化 不過短短的就直接寫近來好了~~
                    int check_width = 0;
                    int check_x = go_staff_x;
                    int check_y = go_staff_y +go_shift;

                    // 找 線寬
                    while(cut_ord_img.at<uchar>(check_y, check_x) == 0){
                        check_width++;
                        check_y++;
                    }
                    if(debuging)  cout << "check_width = " << check_width << endl;
                    
                    // 由 線寬 來判斷現在是不是 五線譜的線 而不是 符桿 或 音頭
                    if(check_width < 5){
                        if(debuging) line(debug_img, Point(go_staff_x, go_staff_y), Point(go_staff_x, go_staff_y + go_shift), Scalar(170, 255, 0), 1);   // 青綠色
                        return go_shift;
                    }
                }
            }
        }
        // 正常來說做一次即可~~但是！
        // 有可能整條都被符桿蓋住了所以找不到~~~所以如果找不到就往右走一點點再五條線重新找一次看看囉！
        go_staff_x +=3;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Recognition_staff_img::recognition_5_find_pitch(Mat template_img){
    debuging = debuging_recog5;
    cout << "go_staff:" << go_staff << endl;
    Mat debug_img = staff_img.clone();
    cvtColor(staff_img, debug_img, CV_GRAY2BGR);

    // 每一顆 note 都會重新定位一次 自己的 first_line_y, 因為怕譜面會有彎曲的狀況不會完全是水平的
    // 大至的方法是 從五線譜的第一條線 往上往下搜尋 哪裡有 線的上邊緣(本格會是黑色 前一格會是白色), 如果第一條線搜尋失敗, 會依序換第二~五條來搜尋
    // 補充: 不建議看 本格黑色 前一格黑色 前兩格白色, 因為線寬有點不穩定不一定會是線寬2以上, 所以 只看一格黑色最保險
    // 用到的變數先宣告
    // 第一階段: 找 五線譜的位移
    int note_x;
    int note_y;
    int note_x_shift = template_img.cols / 2;  // + template_img.cols / 2 是怕被符桿影響 找成符桿中間因為二質化品質差斷斷續續時產生的上邊緣
    int go_staff_x;
    int go_staff_y;
    int staff_shift;
    int first_line_y_result;  
    
    // 第二階段: 根據 五線譜的位移, 從第一條線來算音高
    int first_line_y_result_calculate;
    int first_line_cal_to_note_vec_y;
    int vec_y_through_line;
    int full_space_pitch;
    int half_space_pitch;
    int pitch;
    int vec_y_on_space;
    for(int go_note = 0; go_note < note_count; go_note++){
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 第一階段: 找 五線譜的位移
        note_x = note[0][go_note];
        note_y = note[1][go_note];
        // 看一下 note位置
        if(debuging) rectangle( debug_img, Point(note_x, note_y), Point( note_x + template_img.cols , note_y + template_img.rows ), Scalar(255, 0, 0), 1 );  // 藍色

        // 看此組 此組五線譜 位移了多少, 如果 是線的上邊緣 那麼 本格會是黑色 前一格會是白色,
        //   如果 一開始就符合 就代表直接在線上了 staff_shift = 0,
        //   如果不符合 代表 五線譜可能當初拍照時沒有攤平有點彎曲導致位移, 呼叫 find_staff_shift 找 此 note_x 往右偏移一點(怕被符桿影響)狀態下的 五線譜 往上往下搜尋 哪裡有 線的上邊緣(本格會是黑色 前一格會是白色)
        go_staff_x = note_x + note_x_shift;  // note_x 再加 note_x_shift 是怕受符桿影響
        go_staff_y = pitch_base_y;
        if( (staff_img.at<uchar>(go_staff_y -1, go_staff_x) == 255) && (staff_img.at<uchar>(go_staff_y, go_staff_x) == 0) )
            staff_shift = 0;
        else{
            staff_shift = find_staff_shift(staff_img, pitch_base_y, go_staff_x, go_staff_y, debug_img, debuging);
        }
        // 文字顯示一下 staff_shift 是多少
        if(debuging) cout<<"staff_shift = "<< staff_shift <<endl;


        // 第一條線 根據上面找出的 五線譜位移 做調整
        first_line_y_result = pitch_base_y + staff_shift;
        // 畫短短的五線譜水平紅線 看一下找的效果
        if(debuging){
            for(int i = 0; i < 5; i++) line(debug_img, Point(go_staff_x, first_line_y_result + i * 11), Point(go_staff_x+ 5, first_line_y_result + i * 11), Scalar(0, 0, 255), 1);  // 紅色
            cout << "pitch_base_y = " << pitch_base_y << " , first_line_y_result = " <<  first_line_y_result  << endl;
            imshow("debug", debug_img);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 第二階段: 根據 五線譜的位移, 從第一條線來算音高
        //  一條線到下一條線 中間 稱為 一間, 用眼睛觀察後, 線的間距是 11 px, 如果 以線為基準當0時規律是
        // -2 , note_y 在 -2 ~ 3 算 E
        // -1 
        //  0 ------------------------------- 第一條線
        //  1 
        //  2 
        //  3   
        //  4 , note_y 在  4 ~ 8 算 D
        //  5 
        //  6 
        //  7 
        //  8
        // -2 , note_y 在 -2 ~ 3 算 C 往下以此類推
        // -1
        //  0 -------------------------------  第二條線
        //  1 
        //  2 
        //  3 
        // 視覺化 -2 ~ 3 是 "間的音", 4 ~ 8 是 "線的音"
        if(debuging){
            int go_pitch;
            // 視覺畫 第一條線 往上四間(-4 ~ -1) ~ 第一條線 往下9間(0 ~ 8)
            for(int staff_line_index = -4; staff_line_index <= 8; staff_line_index++){
                for(int i = 0; i < 11; i++){
                    go_pitch = i - 2;
                    if     ( -2 <= go_pitch && go_pitch <= 3) line(debug_img, Point(note_x - 5, first_line_y_result + 11 * staff_line_index + go_pitch), Point(note_x - 1, first_line_y_result + 11 * staff_line_index + go_pitch), Scalar(0, 200, 200), 1);
                    else if(  4 <= go_pitch && go_pitch <= 8) line(debug_img, Point(note_x - 5, first_line_y_result + 11 * staff_line_index + go_pitch), Point(note_x - 1, first_line_y_result + 11 * staff_line_index + go_pitch), Scalar(200, 0, 200), 1);
                }
            }
        }

        // 規律是 11個數字一輪, 從-2開始, 所以計算音高的時候一開始就先 -2 比較好算
        first_line_y_result_calculate = first_line_y_result -2;
        // 變成 線上兩格基準
        //  0 , note_y 在 0 ~ 5 算 E
        //  1 
        //  2 ------------------------------- 第一條線
        //  3 
        //  4 
        //  5   
        //  6 , note_y 在 6 ~ 10 算 D
        //  7 
        //  8 
        //  9 
        // 10
        //  0 , note_y 在 0 ~ 5 算 C 往下以此類推
        //  1
        //  2 -------------------------------  第二條線
        //  3 
        //  4 
        //  5
        // 計算 第一條線 到 note_y 的 向量
        first_line_cal_to_note_vec_y = note_y - first_line_y_result_calculate;
        if(debuging) cout << "note_y=" << note_y << ", first_line_y_result_calculate=" << first_line_y_result_calculate << " , first_line_cal_to_note_vec_y=" << first_line_cal_to_note_vec_y;

        //  一間 高度用眼睛觀察後是 11 px,
        vec_y_through_line = first_line_cal_to_note_vec_y / 11;
        // 走過一間會經過兩顆音, 比如 第一間是 E, 第二間就是 C
        full_space_pitch   = vec_y_through_line * 2;
        // first_line_cal_to_note_vec_y == -10, -9, -8, ..., -1 來 / 11 都會 == 0, 但希望要是 -1, 所以 如果 first_line_cal_to_note_vec_y < 0 這邊 full_space_pitch 直接 -= 2
        if(first_line_cal_to_note_vec_y < 0) full_space_pitch -= 2;

        // 如果count %11 = 6, 7, 8, 9, 10 就算 在線上的音, pitch + 1, 比如第一間是 E, 第二條線是 D
        vec_y_on_space = first_line_cal_to_note_vec_y % 11;
        if(vec_y_on_space < 0){
            vec_y_on_space += 11;  // 太高音跑到負數，就要調整到正數的位置就好囉, 因為是用 %後的結果來看, 所以 +11 就足夠一定可以用到正數的位置
            vec_y_on_space %= 11;  // 別忘了這個喔！因為當on_line == -11時, %完為0, 再加11後就==11了！但要map到0, 所以就要再%11拉！
        }
        half_space_pitch = 0;
        if(vec_y_on_space / 6) half_space_pitch++;
        
        // 走過幾個間的音 和 剩下線上的音 加起來 就是 我們要的音囉
        pitch = full_space_pitch + half_space_pitch;
        if(debuging){
            cout << " , pitch = " << pitch <<endl;
            imshow("debug", debug_img);
            waitKey(0);
        }

        // 找好的pitch 存起來
        note[4][go_note] = pitch;
    }

    
    for(int i = 0; i < note_count; i++){
        // *********** 防呆 ***********
        if( (note[4][i] <= 16) && (note[4][i] >= -11) ){
            switch(note[4][i]){
                case  16:note[4][i] = 48;  // C; 低音3C
                    break;
                case  15:note[4][i] = 50;
                    break;
                case  14:note[4][i] = 52;
                    break;
                case  13:note[4][i] = 53;
                    break;
                case  12:note[4][i] = 55;
                    break;
                case  11:note[4][i] = 57;
                    break;
                case  10:note[4][i] = 59;
                    break;


                case   9:note[4][i] = 60;  // C; 中央4C
                    break;
                case   8:note[4][i] = 62;  // D;
                    break;
                case   7:note[4][i] = 64;  // E;
                    break;
                case   6:note[4][i] = 65;  // F;
                    break;
                case   5:note[4][i] = 67;  // G;
                    break;
                case   4:note[4][i] = 69;  // A;
                    break;
                case   3:note[4][i] = 71;  // B;
                    break;

                case   2:note[4][i] = 72;  // C; 高音5C
                    break;
                case   1:note[4][i] = 74;  // D;
                    break;
                case   0:note[4][i] = 76;  // E; 第一間起點
                    break;
                case - 1:note[4][i] = 77;  // F;
                    break;
                case - 2:note[4][i] = 79;  // G;
                    break;
                case - 3:note[4][i] = 81;  // A;
                    break;
                case - 4:note[4][i] = 83;  // B;
                    break;


                case - 5:note[4][i] = 84;  // C; 高高音6C
                    break;
                case - 6:note[4][i] = 86;  // D;
                    break;
                case - 7:note[4][i] = 88;  // E;
                    break;
                case - 8:note[4][i] = 89;  // F;
                    break;
                case - 9:note[4][i] = 91;  // G;
                    break;
                case -10:note[4][i] = 93;  // A;
                    break;
                case -11:note[4][i] = 95;  // B;
                    break;
            }
        }
    }

    // 存debug圖
    string debug_dir = "debug_img/reg4_find_pitch";
    _mkdir(debug_dir.c_str()); // 建立 框內影像 存放的資料夾
    stringstream ss;
    ss << setw(2) <<setfill('0') << go_staff;
    string str_staff_num = ss.str();
    ss.str("");
    ss.clear();
    string  debug_path = debug_dir + "/" + str_staff_num + ".bmp";
    imwrite(debug_path, debug_img);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void recognition_5_find_pitch(Mat cut_ord_img, 
                              Mat template_img, 
                              int note_count , int note[][1000], 
                              const int first_line_y_ord,
                              int go_staff,
                              bool debuging){
    cout << "go_staff:" << go_staff << endl;
    Mat debug_img = cut_ord_img.clone();
    cvtColor(cut_ord_img, debug_img, CV_GRAY2BGR);

    // 每一顆 note 都會重新定位一次 自己的 first_line_y, 因為怕譜面會有彎曲的狀況不會完全是水平的
    // 大至的方法是 從五線譜的第一條線 往上往下搜尋 哪裡有 線的上邊緣(本格會是黑色 前一格會是白色), 如果第一條線搜尋失敗, 會依序換第二~五條來搜尋
    // 補充: 不建議看 本格黑色 前一格黑色 前兩格白色, 因為線寬有點不穩定不一定會是線寬2以上, 所以 只看一格黑色最保險
    // 用到的變數先宣告
    // 第一階段: 找 五線譜的位移
    int note_x;
    int note_y;
    int note_x_shift = template_img.cols / 2;  // + template_img.cols / 2 是怕被符桿影響 找成符桿中間因為二質化品質差斷斷續續時產生的上邊緣
    int go_staff_x;
    int go_staff_y;
    int staff_shift;
    int first_line_y_result;  
    
    // 第二階段: 根據 五線譜的位移, 從第一條線來算音高
    int first_line_y_result_calculate;
    int first_line_cal_to_note_vec_y;
    int vec_y_through_line;
    int full_space_pitch;
    int half_space_pitch;
    int pitch;
    int vec_y_on_space;
    for(int go_note = 0; go_note < note_count; go_note++){
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 第一階段: 找 五線譜的位移
        note_x = note[0][go_note];
        note_y = note[1][go_note];
        // 看一下 note位置
        if(debuging) rectangle( debug_img, Point(note_x, note_y), Point( note_x + template_img.cols , note_y + template_img.rows ), Scalar(255, 0, 0), 1 );  // 藍色

        // 看此組 此組五線譜 位移了多少, 如果 是線的上邊緣 那麼 本格會是黑色 前一格會是白色,
        //   如果 一開始就符合 就代表直接在線上了 staff_shift = 0,
        //   如果不符合 代表 五線譜可能當初拍照時沒有攤平有點彎曲導致位移, 呼叫 find_staff_shift 找 此 note_x 往右偏移一點(怕被符桿影響)狀態下的 五線譜 往上往下搜尋 哪裡有 線的上邊緣(本格會是黑色 前一格會是白色)
        go_staff_x = note_x + note_x_shift;  // note_x 再加 note_x_shift 是怕受符桿影響
        go_staff_y = first_line_y_ord;
        if( (cut_ord_img.at<uchar>(go_staff_y -1, go_staff_x) == 255) && (cut_ord_img.at<uchar>(go_staff_y, go_staff_x) == 0) )
            staff_shift = 0;
        else{
            staff_shift = find_staff_shift(cut_ord_img, first_line_y_ord, go_staff_x, go_staff_y, debug_img, debuging);
        }
        // 文字顯示一下 staff_shift 是多少
        if(debuging) cout<<"staff_shift = "<< staff_shift <<endl;


        // 第一條線 根據上面找出的 五線譜位移 做調整
        first_line_y_result = first_line_y_ord + staff_shift;
        // 畫短短的五線譜水平紅線 看一下找的效果
        if(debuging){
            for(int i = 0; i < 5; i++) line(debug_img, Point(go_staff_x, first_line_y_result + i * 11), Point(go_staff_x+ 5, first_line_y_result + i * 11), Scalar(0, 0, 255), 1);  // 紅色
            cout << "first_line_y_ord = " << first_line_y_ord << " , first_line_y_result = " <<  first_line_y_result  << endl;
            imshow("debug", debug_img);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 第二階段: 根據 五線譜的位移, 從第一條線來算音高
        //  一條線到下一條線 中間 稱為 一間, 用眼睛觀察後, 線的間距是 11 px, 如果 以線為基準當0時規律是
        // -2 , note_y 在 -2 ~ 3 算 E
        // -1 
        //  0 ------------------------------- 第一條線
        //  1 
        //  2 
        //  3   
        //  4 , note_y 在  4 ~ 8 算 D
        //  5 
        //  6 
        //  7 
        //  8
        // -2 , note_y 在 -2 ~ 3 算 C 往下以此類推
        // -1
        //  0 -------------------------------  第二條線
        //  1 
        //  2 
        //  3 
        // 視覺化 -2 ~ 3 是 "間的音", 4 ~ 8 是 "線的音"
        if(debuging){
            int go_pitch;
            // 視覺畫 第一條線 往上四間(-4 ~ -1) ~ 第一條線 往下9間(0 ~ 8)
            for(int staff_line_index = -4; staff_line_index <= 8; staff_line_index++){
                for(int i = 0; i < 11; i++){
                    go_pitch = i - 2;
                    if     ( -2 <= go_pitch && go_pitch <= 3) line(debug_img, Point(note_x - 5, first_line_y_result + 11 * staff_line_index + go_pitch), Point(note_x - 1, first_line_y_result + 11 * staff_line_index + go_pitch), Scalar(0, 200, 200), 1);
                    else if(  4 <= go_pitch && go_pitch <= 8) line(debug_img, Point(note_x - 5, first_line_y_result + 11 * staff_line_index + go_pitch), Point(note_x - 1, first_line_y_result + 11 * staff_line_index + go_pitch), Scalar(200, 0, 200), 1);
                }
            }
        }

        // 規律是 11個數字一輪, 從-2開始, 所以計算音高的時候一開始就先 -2 比較好算
        first_line_y_result_calculate = first_line_y_result -2;
        // 變成 線上兩格基準
        //  0 , note_y 在 0 ~ 5 算 E
        //  1 
        //  2 ------------------------------- 第一條線
        //  3 
        //  4 
        //  5   
        //  6 , note_y 在 6 ~ 10 算 D
        //  7 
        //  8 
        //  9 
        // 10
        //  0 , note_y 在 0 ~ 5 算 C 往下以此類推
        //  1
        //  2 -------------------------------  第二條線
        //  3 
        //  4 
        //  5
        // 計算 第一條線 到 note_y 的 向量
        first_line_cal_to_note_vec_y = note_y - first_line_y_result_calculate;
        if(debuging) cout << "note_y=" << note_y << ", first_line_y_result_calculate=" << first_line_y_result_calculate << " , first_line_cal_to_note_vec_y=" << first_line_cal_to_note_vec_y;

        //  一間 高度用眼睛觀察後是 11 px,
        vec_y_through_line = first_line_cal_to_note_vec_y / 11;
        // 走過一間會經過兩顆音, 比如 第一間是 E, 第二間就是 C
        full_space_pitch   = vec_y_through_line * 2;
        // first_line_cal_to_note_vec_y == -10, -9, -8, ..., -1 來 / 11 都會 == 0, 但希望要是 -1, 所以 如果 first_line_cal_to_note_vec_y < 0 這邊 full_space_pitch 直接 -= 2
        if(first_line_cal_to_note_vec_y < 0) full_space_pitch -= 2;

        // 如果count %11 = 6, 7, 8, 9, 10 就算 在線上的音, pitch + 1, 比如第一間是 E, 第二條線是 D
        vec_y_on_space = first_line_cal_to_note_vec_y % 11;
        if(vec_y_on_space < 0){
            vec_y_on_space += 11;  // 太高音跑到負數，就要調整到正數的位置就好囉, 因為是用 %後的結果來看, 所以 +11 就足夠一定可以用到正數的位置
            vec_y_on_space %= 11;  // 別忘了這個喔！因為當on_line == -11時, %完為0, 再加11後就==11了！但要map到0, 所以就要再%11拉！
        }
        half_space_pitch = 0;
        if(vec_y_on_space / 6) half_space_pitch++;
        
        // 走過幾個間的音 和 剩下線上的音 加起來 就是 我們要的音囉
        pitch = full_space_pitch + half_space_pitch;
        if(debuging){
            cout << " , pitch = " << pitch <<endl;
            imshow("debug", debug_img);
            waitKey(0);
        }

        // 找好的pitch 存起來
        note[4][go_note] = pitch;
    }

    
    for(int i = 0; i < note_count; i++){
        // *********** 防呆 ***********
        if( (note[4][i] <= 16) && (note[4][i] >= -11) ){
            switch(note[4][i]){
                case  16:note[4][i] = 48;  // C; 低音3C
                    break;
                case  15:note[4][i] = 50;
                    break;
                case  14:note[4][i] = 52;
                    break;
                case  13:note[4][i] = 53;
                    break;
                case  12:note[4][i] = 55;
                    break;
                case  11:note[4][i] = 57;
                    break;
                case  10:note[4][i] = 59;
                    break;


                case   9:note[4][i] = 60;  // C; 中央4C
                    break;
                case   8:note[4][i] = 62;  // D;
                    break;
                case   7:note[4][i] = 64;  // E;
                    break;
                case   6:note[4][i] = 65;  // F;
                    break;
                case   5:note[4][i] = 67;  // G;
                    break;
                case   4:note[4][i] = 69;  // A;
                    break;
                case   3:note[4][i] = 71;  // B;
                    break;

                case   2:note[4][i] = 72;  // C; 高音5C
                    break;
                case   1:note[4][i] = 74;  // D;
                    break;
                case   0:note[4][i] = 76;  // E; 第一間起點
                    break;
                case - 1:note[4][i] = 77;  // F;
                    break;
                case - 2:note[4][i] = 79;  // G;
                    break;
                case - 3:note[4][i] = 81;  // A;
                    break;
                case - 4:note[4][i] = 83;  // B;
                    break;


                case - 5:note[4][i] = 84;  // C; 高高音6C
                    break;
                case - 6:note[4][i] = 86;  // D;
                    break;
                case - 7:note[4][i] = 88;  // E;
                    break;
                case - 8:note[4][i] = 89;  // F;
                    break;
                case - 9:note[4][i] = 91;  // G;
                    break;
                case -10:note[4][i] = 93;  // A;
                    break;
                case -11:note[4][i] = 95;  // B;
                    break;
            }
        }
    }

    // 存debug圖
    string debug_dir = "debug_img/reg4_find_pitch";
    _mkdir(debug_dir.c_str()); // 建立 框內影像 存放的資料夾
    stringstream ss;
    ss << setw(2) <<setfill('0') << go_staff;
    string str_staff_num = ss.str();
    ss.str("");
    ss.clear();
    string  debug_path = debug_dir + "/" + str_staff_num + ".bmp";
    imwrite(debug_path, debug_img);
}
