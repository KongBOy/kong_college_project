#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void recognition_0_vertical_map_to_speed_up(Mat staff_img_erase_line , ///消掉五線譜的圖 = 之後稱的 reduce_line
                                            int& e_count, int l_edge[200], int r_edge[200] , int distance[200] , int mountain_area[200] , /// 好像只需要e_count、l_edge、distance 即可ˊ口ˋ
                                            bool debuging=false);
