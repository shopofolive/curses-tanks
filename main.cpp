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

const double PI = 3.141592653589793238463;

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
		int mx = LINES;
		int my = COLS;


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
void DrawScreen(Ground & g, Player * players, int turn)
{
	erase();
	box(stdscr, 0, 0);
	g.Draw();
	players[0].Draw(g);
	players[1].Draw(g);
	players[0].DrawSettings(turn);
	players[1].DrawSettings(turn);
	refresh();
}

//v = pN
void Hit(Vec2D &v, Player * players, int turn, Ground &g)
{
    //all nine hit coordinates will be pushed here:
    vector<Vec2D> hits;
    //a containter variable that will take the coordinates of each hit and be pushed into the vector:
    Vec2D h;
    
    //define the vector:
    for (int i=-1;i<2;i++)
    {
        for(int j=-1;j<2;j++)
        {
            h.y = v.y + j;
            h.x = v.x + i;
            hits.push_back(h);
        }
    }
    
    for (size_t i = 0; i<hits.size(); i++)
    {
        //TODO: prevent hits from landing outside the ground vector
        int hit_line = hits.at(i).y;
        int hit_col = hits.at(i).x;
        move(hit_line, hit_col);
        if ((hit_line == g.ground.at(hit_col)) || (hit_line + 1 == g.ground.at(hit_col)))
        {
            refresh();
            addch('X');
        }
        
        //form a crater:
        if (hit_line == g.ground.at(hit_col))
            g.ground.at(hit_col - 1) = g.ground.at(hit_col - 1) + 1;
        
        //adjust to make sure the ground does not encroach on the border:
        if (g.ground.at(hit_col - 1) > LINES - 2)
            g.ground.at(hit_col - 1) = LINES - 2;
        
        //check if enemy tank is hit:
        if ((hit_line == g.ground.at(players[1 - turn].col) - 1) && (hit_col == players[1 - turn].col + 1))
        {
            //enemy tank is hit!!!
            players[1 - turn].hit = true;        }
        
        //check if self hit
        if ((hit_line == g.ground.at(players[turn].col) - 1) && (hit_col == players[turn].col + 1))
        {
            //my tank is hit!!!
            players[turn].hit = true;        }
        
        /*
        //for debugging purposes: print out a table showing the position of each hit and the position of enemy tank:
        move(int(i), 1);
        stringstream ss;
        ss << setw(4) << line << " " << col;
        addstr(ss.str().c_str());
        refresh();
        
        move(int(i), 20);
        ss = stringstream();
        ss << setw(4) << g.ground.at(players[1-turn].col) - 1 << " " << players[1-turn].col + 1 << " " << players[1 - turn].hit;
        addstr(ss.str().c_str());
        refresh();
         */
    }
    
    refresh();
	MySleep(500);
}

//http://www.iforce2d.net/b2dtut/projected-trajectory

void Shoot(Ground & g, Player * players, int turn, bool &keep_going){
	double angle = players[turn].angle / 180.0 * PI;
    double time_divisor = 5.0;
	Vec2D force(sin(angle) * players[turn].power * 0.2, cos(angle) * players[turn].power * 0.2);
    if (players[turn].s == RIGHT)
        force.x = -force.x;
    Vec2D gravity(0.0, -0.98);
    //the starting point is above the actual position of the tank, to prevent self-injury in the first moments of the shot;
    int p_col = players[turn].col;
    int p_line = LINES - g.ground.at(players[turn].col);
    if (turn == 0)
        p_col++;
    Vec2D p0(p_col,p_line);
    
    for (int i = 1; i < 50000; i++)
    {
        double di = i / time_divisor;
        Vec2D pN = p0 + force * di + gravity * (di * di + di) * 0.5;
        pN.y = LINES - pN.y;
           
        if (pN.x < 1 || pN.x >= COLS - 2)
            break;
        if (pN.y < 1) {
            MySleep(20);
            continue;
        }
		if (pN.y >= g.ground.at((int)pN.x))
		{
			Hit(pN, players, turn, g);
			MySleep(20);
			break;
		}
        
        move((int)pN.y - 1, (int)pN.x + 1);
        addch('*'); instead:
        refresh();
        MySleep(20);
		if ((players[1 - turn].hit) || (players[turn].hit))
		{
			keep_going = false;
		}
    }
}

void InitializeGame(Ground &g, Player *players)
{
    int turn = 0;
    g.InitializeGround();
    //make sure the tank is positioned not too close to either border, otherwise Shoot() will result in errors:
    players[0].Initialize(rand() % (COLS / 4) + 5, LEFT);
    players[1].Initialize(rand() % (COLS / 4) + 3 * COLS / 4 - 5, RIGHT);
    DrawScreen(g, players, turn);
}

void ProcessKeyboard(Ground &g, Player *players, int & turn, bool &keep_going)
{
    bool show_char = false;
    int c = getch();
    switch (c)
    {
        case 27:
            keep_going = false;
            break;
            
        case 's':
            players[turn].PowerDown();
            break;
            
        case 'w':
            players[turn].PowerUp();
            break;
            
        case 'e':
            players[turn].AngleUp();
            break;
            
        case 'c':
            players[turn].AngleDown();
            break;
            
        case 10:
        case KEY_ENTER:
#if defined(WIN32)
        case PADENTER:
#endif
            Shoot(g, players, turn, keep_going);
            turn = 1 - turn;
            break;
            
        default:
            show_char = true;
            break;
    }
}

int main(int argc, char * argv[])
{
	srand((unsigned int)time(nullptr));

	int turn = 0;
	bool keep_going = true;
	Ground g;
	Player players[2];

	initscr();
	noecho();
	keypad(stdscr, 1);
    curs_set(0);

	/*
    TitleScreen();
	while (getch() != 'G')
	{
		continue;
	}
     */

    InitializeGame(g, players);
    
	while (keep_going)
	{
        ProcessKeyboard(g, players, turn, keep_going);
        
        if ((!players[turn].hit) && (!players[1 - turn].hit))
        {
            DrawScreen(g, players, turn);
            /*if (show_char) { PrintMessage(0, 1, " ", c);}*/
        }
        else
        {
            if(players[turn].hit)
            {
                players[1 - turn].score++;
            }
            else
            {
                players[turn].score++;
            }

            if ((players[1 - turn].score < 3) && (players[turn].score < 3))
            {
                //erase current ground and players data and re-initialize game:
                g.ground.clear();
                InitializeGame(g, players);
                keep_going = true;
            }
            else
            {
                break;
            }
        }
    }
    //TODO: insert winner screen function here
	erase();
	echo();
	endwin();
	return 0;
}
