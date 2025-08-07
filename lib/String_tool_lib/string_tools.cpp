/*
這個程式的功能是：
整數轉字串的工具
*/

#include <iostream>
#include <sstream>
#include "string_tools.h"
#include <windows.h>
using namespace std;

string IntToString(int num)
{
    stringstream ss;
    ss<<num;

    string temp_string;
    ss>>temp_string;
    return temp_string;
}

// 把 utf8 轉 cp950(chatGPT 的 code)
string utf8_to_cp950(const string& utf8_str) {
    // UTF-8 -> WideChar (UTF-16)
    int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    wstring wide_str(wide_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_str[0], wide_len);

    // WideChar -> CP950
    int cp950_len = WideCharToMultiByte(950, 0, wide_str.c_str(), -1, NULL, 0, NULL, NULL);
    string cp950_str(cp950_len, 0);
    WideCharToMultiByte(950, 0, wide_str.c_str(), -1, &cp950_str[0], cp950_len, NULL, NULL);

    return cp950_str;
}

vector<string> String_Split(const string & str, char delimiter){
    vector<string> result;
    stringstream ss(str);
    string item;
    while(getline(ss, item, delimiter)){
        result.push_back(item);
    }
    return result;
}