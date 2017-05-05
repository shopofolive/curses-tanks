//SYNCHRONOUS MOVEMENT

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>

#if defined(WIN32)
#include <Windows.h>
#include "curses.h"
#include "curses-tanks-m\curses-tanks-m\Vec2D.hpp"

#else
#include <curses.h>
#include <cstdlib>
#include <unistd.h>
#include "Vec2D.hpp"
#endif

#include "ground.hpp"
#include "player.hpp"

using namespace std;

extern int base_height_divisor;
extern int max_height_divisor;

void PrintMessage(int l, int c, string s, char = 32)
{
    move(l, c);
    stringstream ss;
    ss << setw(int(s.length())) << s << " ";
    addstr(ss.str().c_str());
    refresh();
}

void TitleScreen()
{
	erase();
	while (getch() == ERR)
	{
		stringstream ss;
		refresh();

		//will center TANKS vertically
		int numOfEndls = (LINES - 22) / 2;
		//askii art title screen

		for (int i = 0; i < numOfEndls; i++)
		{
			ss << endl;
		}
		ss << '\t' << "TTTTTTTTTTTTTTT   AAAAAAAAAAA   NNNNNNN    NNNNN  KKKKK   KKKKK  SSSSSSSSSS" << endl;
		ss << '\t' << "TTTTTTTTTTTTTTT  AAAAAAAAAAAAA  NNNNNNNN   NNNNN  KKKKK  KKKKK  SSSSSSSSSSSS" << endl;
		ss << '\t' << "TTTTTTTTTTTTTTT  AAAAA   AAAAA  NNNNNNNNN  NNNNN  KKKKK KKKKK   SSSSS  SSSSS" << endl;
		ss << '\t' << "     TTTTT       AAAAA   AAAAA  NNNNNNNNNN NNNNN  KKKKKKKKKK     SSSSSSS  " << endl;
		ss << '\t' << "     TTTTT       AAAAAAAAAAAAA  NNNNN NNNNNNNNNN  KKKKKKKKKK        SSSSSSSS " << endl;
		ss << '\t' << "     TTTTT       AAAAAAAAAAAAA  NNNNN  NNNNNNNNN  KKKKK KKKKK   SSSSS  SSSSS" << endl;
		ss << '\t' << "     TTTTT       AAAAA   AAAAA  NNNNN   NNNNNNNN  KKKKK  KKKKK  SSSSSSSSSSSS" << endl;
		ss << '\t' << "     TTTTT       AAAAA   AAAAA  NNNNN    NNNNNNN  KKKKK   KKKKK  SSSSSSSSSS" << endl;
		ss << '\t' << "------------------------------------------------------------------------------" << endl;
		ss << '\t' << " PRESS ENTER TO START" << endl;
		//waste_time();

		addstr(ss.str().c_str());
		refresh();
	}
}

void MySleep(int milliseconds)
{
#if defined(WIN32)
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}
void DrawScreen(Ground & g, Player * players)
{
	erase();
	box(stdscr, 0, 0);
	g.Draw();
	players[0].Draw(g);
	players[1].Draw(g);
    players[0].DrawShots(0, g, players[1]);
    players[1].DrawShots(1, g, players[0]);
    players[0].DrawSettings(0);
    players[1].DrawSettings(1);
    refresh();
}

void InitializeGame(Ground &g, Player *players)
{
    g.InitializeGround();
    //make sure the tank is positioned not too close to either border, otherwise Shoot() will result in errors:
    players[0].Initialize(rand() % (COLS / 4) + 5, LEFT);
    players[1].Initialize(rand() % (COLS / 4) + 3 * COLS / 4 - 5, RIGHT);
    DrawScreen(g, players);
}

void ProcessKeyboard(Ground &g, Player *players, bool &keep_going)
{
    nodelay(stdscr, 1);
    bool show_char = false;
    int c = getch();
    switch (c)
    {
        case 27:
            keep_going = false;
            break;
            
        //Player 0:
        case 's':
            players[0].PowerDown();
            break;
            
        case 'w':
            players[0].PowerUp();
            break;
            
        case 'q':
            players[0].AngleUp();
            break;
            
        case 'a':
            players[0].AngleDown();
            break;
            
        case 'z':
            if (players[0].col > 0)
                players[0].col--;
        break;
            
        case 'x':
            if (players[0].col < COLS - 3)
                players[0].col++;
            break;
            
        case 32:
            if (!players[0].is_shooting)
                players[0].Shoot(g, players[1]);
            break;
            
        
        //Player 1:
            
        case ';':
            players[1].PowerDown();
            break;
            
        case '[':
            players[1].PowerUp();
            break;
            
        case ']':
            players[1].AngleUp();
            break;
            
        case '\'':
            players[1].AngleDown();
            break;
            
        case '.':
            if (players[1].col > 0)
                players[1].col--;
            break;
            
        case '/':
            if (players[1].col < COLS - 3)
                players[1].col++;
            break;
            
        case 10:
        case KEY_ENTER:
#if defined(WIN32)
        case PADENTER:
#endif
            if (!players[1].is_shooting)
                players[1].Shoot(g, players[0]);
            break;
            
        case ERR:
            show_char = true;
            break;
    }
}

void ApplyChanges(Ground &g, Player *players, bool &keep_going)
{
    for (int i=0;i<2;i++)
    {
        //update score:
        if(players[i].hit)
        {
            players[1 - i].score++;
            //erase current ground and players data and re-initialize game:
            g.ground.clear();
            InitializeGame(g, players);
        }
        
        if (players[i].score >= 3)
        {
            keep_going = false;
        }
    }
}

int main(int argc, char * argv[])
{
	srand((unsigned int)time(nullptr));

	bool keep_going = true;
	Ground g;
	Player players[2];

	initscr();
	noecho();
	keypad(stdscr, 1);
    curs_set(0);
    
    nodelay(stdscr, 1);

    InitializeGame(g, players);
    
    //GAME LOOP:
	while (keep_going)
	{
        ProcessKeyboard(g, players, keep_going);
        ApplyChanges(g, players, keep_going);
        
        DrawScreen(g, players);
    }//END GAME LOOP
    
    //TODO: insert winner screen function here
	erase();
	echo();
	endwin();
	return 0;
}
