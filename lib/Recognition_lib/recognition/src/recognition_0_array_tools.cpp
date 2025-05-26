/*
這個程式的功能是：
做一些陣列運算的工具
    position_erase 可以刪除陣列裡指定位置的entry
    bubbleSort 排序
*/
#include <iostream>

#include "recognition_0_array_tools.h"

using namespace std;

void position_erase(int& maybe_head_count,float maybe_head[][200],int position)
{
    if(maybe_head_count > 0)
    {
        if(maybe_head_count == 1)
        {
            maybe_head_count--;
            return;
        }
        else
        {
            if(position == maybe_head_count -1) //刪最後一個
            {
                maybe_head_count--;
                return;
            }
            else
            {
                for(int i = position ; i < maybe_head_count-1 ; i++)
                {
                    maybe_head[0][i] = maybe_head[0][i+1];
                    maybe_head[1][i] = maybe_head[1][i+1];
                    maybe_head[2][i] = maybe_head[2][i+1];
                }
                maybe_head_count--;
                return;
            }
        }
    }
    else
    {
        cout<<"沒有線可以刪，沒有做動作"<<endl;
        return;
    }
}


void position_erase_line(int& lines_count,short lines[][200],bool lines_dir[][200] ,int position)
{
    if(lines_count > 0)
    {
        if(lines_count == 1)
        {
            lines_count--;
            return;
        }
        else
        {
            if(position == lines_count -1) //刪最後一個
            {
                lines_count--;
                return;
            }
            else
            {
                for(int i = position ; i < lines_count-1 ; i++)
                {
                    lines[0][i] = lines[0][i+1];
                    lines[1][i] = lines[1][i+1];
                    lines[2][i] = lines[2][i+1];
                    lines_dir[0][i] = lines_dir[0][i+1];
                    lines_dir[1][i] = lines_dir[1][i+1];
                }
                lines_count--;
                return;
            }
        }
    }
    else
    {
        cout<<"沒有線可以刪，沒有做動作"<<endl;
        return;
    }
}

void position_erase_note(int& note_count,int note[][1000],int position)
{
    if(note_count > 0)
    {
        if(note_count == 1)
        {
            note_count--;
            return;
        }
        else
        {
            if(position == note_count -1) //刪最後一個
            {
                note_count--;
                return;
            }
            else
            {
                for(int i = position ; i < note_count-1 ; i++)
                {
                    note[0][i] = note[0][i+1];
                    note[1][i] = note[1][i+1];
                    note[2][i] = note[2][i+1];
                    note[3][i] = note[3][i+1];
                    note[4][i] = note[4][i+1];
                }
                note_count--;
                return;
            }
        }
    }
    else
    {
        cout<<"沒有線可以刪，沒有做動作"<<endl;
        return;
    }
}



void bubbleSort_maybe_head(int amount , float entry[][200],int index)
{
	for(int i = 0 ; i < amount-1 ; i++)
	{
		for(int j = i+1 ; j < amount ; j++)
		{
			if(entry[index][i] > entry[index][j])
			{
				////////////// swap ///////////////
				float temp[3];
				temp [0]    = entry[0][i]; temp[1]     = entry[1][i]; temp[2]     = entry[2][i];
				entry[0][i] = entry[0][j]; entry[1][i] = entry[1][j]; entry[2][i] = entry[2][j];
				entry[0][j] = temp[0]    ; entry[1][j] = temp[1]    ; entry[2][j] = temp[2]    ;
			}
		}
	}
}


void bubbleSort_note(int amount ,int entry[][1000] , int index)
{
	for(int i = 0 ; i < amount-1 ; i++)
	{
		for(int j = i+1 ; j < amount ; j++)
		{
			if(entry[index][i] > entry[index][j])
			{
				////////////// swap ///////////////
				float temp[5];
				temp [0]    = entry[0][i]; temp[1]     = entry[1][i]; temp[2]     = entry[2][i]; temp[3]     = entry[3][i]; temp[4]     = entry[4][i];
				entry[0][i] = entry[0][j]; entry[1][i] = entry[1][j]; entry[2][i] = entry[2][j]; entry[3][i] = entry[3][j]; entry[4][i] = entry[4][j];
				entry[0][j] = temp[0]    ; entry[1][j] = temp[1]    ; entry[2][j] = temp[2]    ; entry[3][j] = temp[3]    ; entry[4][j] = temp[4]    ;
			}
		}
	}
}

void bubbleSort_line(int amount ,short entry[][200] , bool entry2[][200] , int index)
{
	for(int i = 0 ; i < amount-1 ; i++)
	{
		for(int j = i+1 ; j < amount ; j++)
		{
			if(entry[index][i] > entry[index][j])
			{
				////////////// swap ///////////////
				short temp[3];
				temp [0]    = entry[0][i]; temp[1]     = entry[1][i]; temp[2]     = entry[2][i];
				entry[0][i] = entry[0][j]; entry[1][i] = entry[1][j]; entry[2][i] = entry[2][j];
				entry[0][j] = temp[0]    ; entry[1][j] = temp[1]    ; entry[2][j] = temp[2]    ;

				bool temp2[2];
				temp2 [0]    = entry2[0][i]; temp2[1]     = entry2[1][i];
				entry2[0][i] = entry2[0][j]; entry2[1][i] = entry2[1][j];
				entry2[0][j] = temp2[0]    ; entry2[1][j] = temp2[1]    ;
			}
		}
	}
}

