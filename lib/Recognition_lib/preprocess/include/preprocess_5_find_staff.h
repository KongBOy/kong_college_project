#include <opencv2/core/core_c.h>
#include "preprocess_0_hough_tool.h"
using namespace cv;

#ifndef FIND_STAFF_H_INCLUDED
#define FIND_STAFF_H_INCLUDED



#endif // FIND_STAFF_H_INCLUDED



void filter_multi_same_line(vector<Vec2f>& src_lines,int, bool debuging);
void position_erase(vector<Vec2f>&src_lines,int position);


//new things
//不要用vector<Vec2f>*& staff，原因寫在find_staff.cpp
int find_Staff_lines(vector<Vec2f>& select_lines, int dist_level_0, int dist_level_1, bool debuging=false);

