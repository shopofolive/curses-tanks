#if defined(WIN32)
#include "curses.h"
#else
#include <curses.h>
#include <cstdlib>
#endif

#include "ground.hpp"

//when ground is initialized, determine approximately how high from the bottom of the screen it's going to be positioned
int base_height_divisor = 8;

void Ground::InitializeGround()
{
	//height of every cell of ground:
    int current_height = LINES - LINES / base_height_divisor;

	for (int i = 1; i < COLS - 1; i++) {
		//if r below is > 1600 (i.e., 84% probability), this cell's height is same as that of the last cell:
        int h = current_height;
		//probability:
        int r = rand() % 10000;
		//8% probability of...
        if (r < 800) {
			//...this cell's height being 1 LINE higher than that of the previous cell:
            current_height++;
			//adjust to make sure the ground is no more than 1 LINE higher than the low border:
            if (current_height >= LINES - 2)
				current_height = LINES - 2;
		}
		//16% probability of...
        else if (r < 1600) {
			//...this cell's height being 1 LINE lower than that of the previous cell:
            current_height--;
			if (current_height < 1)
				current_height = 1;
		}
		ground.push_back(h);
	}
}

void Ground::Draw()
{
	for (size_t i = 0; i < ground.size(); i++) {
		//crucially, since the counter starts at 0 yet the ground starts at COL 1, i has to be adjusted by 1:
        mvaddch(ground.at(i), i + 1, ACS_CKBOARD);
        
        for (int j = 0; j < LINES - ground.at(i) - 1; j++)
        {
            move(ground.at(i) + j, int(i + 1));
            addch(ACS_CKBOARD);
        }
	}
}
