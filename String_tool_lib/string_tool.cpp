/*
這個程式的功能是：
整數轉字串的工具
*/

#include <iostream>
#include <sstream>
#include "string_tools.h"
using namespace std;

string IntToString(int num)
{
    stringstream ss;
    ss<<num;

    string temp_string;
    ss>>temp_string;
    return temp_string;
}
