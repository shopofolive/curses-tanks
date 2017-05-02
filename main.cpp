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

//pN is v
void Hit(Vec2D &v, Player *players, int turn)
{
    vector<Vec2D> hits;
    Vec2D h;
    
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
        move(hits.at(i).y, hits.at(i).x);
        addch('X');
    }
    
    refresh();
	MySleep(1000);
}

//http://www.iforce2d.net/b2dtut/projected-trajectory

void Shoot(Ground & g, Player * players, int turn, bool &keep_going){
	double angle = players[turn].angle / 180.0 * PI;
    double time_divisor = 15.0;
	Vec2D force(sin(angle) * players[turn].power * 0.2, cos(angle) * players[turn].power * 0.2);
    if (players[turn].s == RIGHT)
        force.x = -force.x;
    Vec2D gravity(0.0, -0.98);
    Vec2D p0(players[turn].col,LINES - g.ground.at(players[turn].col));
    
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
			Hit(pN, players, turn);
			MySleep(100);
			break;
		}
        
        move((int)pN.y - 1, (int)pN.x + 1);
        addch('*'); instead:
        refresh();
        MySleep(20);
		if (players[turn].hit)
		{
			keep_going = false;
		}
    }
    
    /*
    double angle = players[turn].angle / 180.0 * PI;
	double y_component = sin(angle) * players[turn].power * 0.2; //1st component of force
	double x_component = cos(angle) * players[turn].power * 0.2;//2nd component of force
	
	double pNx;
	double pNy;
	double time_divisor = 15.0;
	
	if (players[turn].s == RIGHT)
		x_component = -x_component;

	double p0x = players[turn].col;
	double p0y = g.ground.at(players[turn].col);//shouldn't it be LINES - g.ground.at(players[turn].col); ??
	// higher ground numbers are lower altitudes (0 is first line, etc).
	p0y = LINES - p0y;
	for (int i = 1; i < 50000; i++)
	{
		double di = i / time_divisor;

		pNx = (int)(p0x + di * x_component);
        pNy = p0y + di * y_component + (di * di + di) * -9.8 / time_divisor / 1.5;
		pNy = (int)(LINES - pNy);
		if (pNx < 1 || pNx >= COLS - 2)
			break;
		if (pNy < 1) {
			MySleep(20);
			continue;
		}
	//	if (pNy >= LINES - 2)
	//		break;
		if (pNy > g.ground.at((int)pNx))
			break;

		move((int)pNy - 1, (int)pNx + 1);
		refresh();
		MySleep(20);
	}
    */
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

	/*
    TitleScreen();
	while (getch() != 'G')
	{
		continue;
	}
     */

	g.InitializeGround();
	players[0].Initialize(rand() % (COLS / 4), LEFT);
	players[1].Initialize(rand() % (COLS / 4) + 3 * COLS / 4 - 2, RIGHT);

	DrawScreen(g, players, turn);
	while (keep_going)
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
		DrawScreen(g, players, turn);
		if (show_char) {
			move(0, 1);
			stringstream ss;
			ss << setw(4) << c << " ";
			addstr(ss.str().c_str());
			refresh();
		}
		//checks if player turn.hit = true
		// if so, break
	}
	erase();
	// add winner screen
	addstr("Hit any key to exit");
	refresh();
	getch();
	echo();
	endwin();
	return 0;
}
