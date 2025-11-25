/*
這個程式的功能是：
整個系統的整合介面(動畫、拍照、辨識+轉midi+攝影機)
*/
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include "Game_UI.h"

#include "ScreenTool.h"
#include "DirectoryTool.h"

using namespace std;
using namespace cv;

int main(){
    Game game;
    game.run();
}
