#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void See_mountain(const Mat & src_img, int e_count, int l_edge[], int r_edge[], int distance[], int mountain_area[], string dir_path="", string file_name="");
void Horizon_map_to_find_line(Mat src_img, vector<Vec2f> & mountain_lines2, Mat & containor, bool debuging);
#ifndef HORIZON_MAP_TO_FIND_LINE_H_INCLUDED
#define HORIZON_MAP_TO_FIND_LINE_H_INCLUDED

#endif // HORIZON_MAP_TO_FIND_LINE_H_INCLUDED
