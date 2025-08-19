#ifndef RECOGNITIOIN_0_ARRAY_TOOLS_H_INCLUDED
#define RECOGNITIOIN_0_ARRAY_TOOLS_H_INCLUDED
#endif // RECOGNITIOIN_0_ARRAY_TOOLS_H_INCLUDED

#define X_INDEX 0
#define Y_INDEX 1

void bubbleSort_line(int amount , short entry[][200] , bool entry2[][200],int index);
void bubbleSort_maybe_head(int amount , float entry[][200],int index);
void bubbleSort_note(int amount , int entry[][1000],int index);

void position_erase_bar(int& lines_count,short lines[][200],bool lines_dir[][200] ,int position);
void position_erase_note(int& note_count,int note[][1000],int position);
void position_erase(int& maybe_head_count,float maybe_head[][200],int position);
