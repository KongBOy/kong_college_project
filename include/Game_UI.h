#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Recognition.h"
#include "Generate_Play_Midi.h"

using namespace std;
using namespace cv;

class Game{
    private:
        bool debuging;

        string Title;
        string UI_WINDOW_NAME;
        /////////////////////////////////////////////////////////////////
        Mat UI0             ;  // 小小指揮家封面
        Mat UI0_enter       ;  // 小小指揮家封面_press_enter
        Mat UI0_1           ;  // 歡迎來到指揮家樂園
        Mat UI0_2           ;  // 您可以任意挑選一張您喜愛的樂譜
        Mat UI0_3           ;  // 我們將會幫您看懂樂譜
        Mat UI0_4           ;  // 並且幫您演奏
        Mat UI0_5           ;  // 接著您可以用最直接的方式與音樂共舞
        Mat UI0_6           ;  // 盡情地舞動您的雙手吧！
        Mat UI0_6_enter     ;  // 盡情地舞動您的雙手吧！_press_enter
        Mat UI1             ;  // 請挑選一個您喜歡的樂譜，並傳給我們吧!
        Mat UI1_1           ;  // 請挑選一個您喜歡的樂譜，並傳給我們吧!_press_enter
        Mat UI1_2           ;  // 我們當然需要一些時間視譜與練習
        /////////////////////////////////////////////////////////////////
        Mat   UI2                        ;  // 進度條背景
        Mat   loading_bar_item_4         ;  // 進度條上面的圖案
        Mat   loading_bar_item_8         ;  // 進度條上面的圖案
        Mat   loading_bar_item_16        ;  // 進度條上面的圖案
        Mat   loading_bar_item_rest_8    ;  // 進度條上面的圖案
        Mat   loading_bar_item_flat      ;  // 進度條上面的圖案
        Mat   loading_bar_item_sharp     ;  // 進度條上面的圖案
        Mat   loading_bar_item_finish    ;  // 進度條上面的圖案
        Mat   loading_bar_item_staff_line;  // 進度條上面的圖案
        int   loading_bar                ;  // 進度條數值
        Point loading_item_position[17]  ;  // 進度條切成17份的位置
        Mat   UI2_5                      ;  // 很抱歉　您提供的樂譜我們無法辨識 請您重新拍攝樂譜
        Mat   UI_drawing;
        /////////////////////////////////////////////////////////////////
        Mat UI3             ;  // 即將進入指揮囉!(Loading...)
        Mat UI3_enter       ;  // 即將進入指揮囉!(Enter)
        Mat UI3_enter_press ;  // 即將進入指揮囉!(press Enter)
        /////////////////////////////////////////////////////////////////
        // 開始指揮時 用的背景        
        Mat background1;
        Mat background;
        // 開始指揮時 右上出現的對話文字
        Mat T1;
        Mat T2;
        Mat T3;
        Mat T4;
        Mat T5;
        Mat T6;
        Mat T7;
        Mat T8;
        Mat T9;
        Mat T10;
        Mat T11;
        // 開始指揮時 顯示 音量 和 速度 的 白色bar
        Mat bar;
        /////////////////////////////////////////////////////////////////
        Mat UI4_0           ;  // 曲終. 您的指揮真是令我們驚艷！
        Mat UI4_1           ;  // 感謝您參與這趟旅程，期待下一次相遇。
        Mat UI5_0           ;  // 封底 參與人員
        Mat UI5_1           ;  // ******* (沒用到) *******  封底 參與人員 enter

        // 指向 頁面辨識結果
        Recognition_page* recog_page_ptr;
        // 指向 辨識結果生成的 midi_notes
        Midi_Generate*    midi_notes_ptr;
        int width_frame_acc;
        
    public:
        Game();
        void run();

        /////////////////////////////////////////////////////////////////
        void set_item_img     (int position, Mat& setted_img);
        int  set_item_position(int in_num);
        void Show_loading_bar (int start_num, int end_num);

        void UI_loading_preprocess();
        void UI_loading_recognition_one_staff(Recognition_staff_img* staff_recog);
};