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
//#include <SDL2/SDL.h>
//#include <SDL2_Mixer/SDL_Mixer.h>

using namespace std;

//defined in ground.cpp:
extern int base_height_divisor;

//print title screen:
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

//delay game for int milliseconds:
void MySleep(int milliseconds)
{
#if defined(WIN32)
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

//print window:
void DrawScreen(Ground & g, Player * players)
{
	//clear window:
    erase();
    //
	//draw a border around the window:
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

//restart game after a player is hit:
void InitializeGame(Ground &g, Player *players)
{
    //re-initialize ground
    g.InitializeGround();
    //make sure the tank is positioned not too close to either border (adjust_value), otherwise Shoot() will result in errors:
    int adjust_value = 5;
    //position each tank anywhere within COLS/4 from either edge, + adjust_value
    players[0].Initialize(rand() % (COLS / 4) + adjust_value, LEFT);
    players[1].Initialize(rand() % (COLS / 4) + 3 * COLS / 4 - adjust_value, RIGHT);
    //print the screen
    DrawScreen(g, players);
}

//take in keyboard input:
void ProcessKeyboard(Ground &g, Player *players, bool &keep_going)
{
    //do not wait for keyboard to be entered when getch() is invoked:
    nodelay(stdscr, 1);
    
    //keyboard input:
    int c = getch();
    
    //assign action to entered key:
    switch (c)
    {
        //ESC = quit game:
        case 27:
            //this will break out of the game loop in main():
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
            //make sure the tank stays within left bounds:
            if (players[0].col > 0)
                players[0].col--;
        break;
            
        case 'x':
            //make sure the tank stays within right bounds:
            if (players[0].col < COLS - 3)
                players[0].col++;
            break;
            
        //space key:
        case 32:
            //invoke Shoot() only if the previous shot (if any) has been fully printed and processed:
            //this ensures that only 1 shot per each player is fired at a time:
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
        
        //enter key:
        case 10:
        case KEY_ENTER:
#if defined(WIN32)
        case PADENTER:
#endif
            //invoke Shoot() only if the previous shot (if any) has been fully printed and processed:
            //this ensures that only 1 shot per each player is fired at a time:
            if (!players[1].is_shooting)
                players[1].Shoot(g, players[0]);
            break;
            
        //if not keyboard input is entered, proceed to the next iteration of the game loop
        case ERR:
            break;
    }
}

//update scores:
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
        
        //if either player's score reaches 3, quit game:
        if (players[i].score >= 3)
        {
            keep_going = false;
        }
    }
}


int main(int argc, char * argv[])
{
    /*
    //SDL_Mixer stuff:
    Mix_Music *music;
    
    // Initialize music.
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "unable to initialize SDL\n");
        exit(EXIT_FAILURE);
    }
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        fprintf(stderr, "unable to initialize SDL_mixer\n");
        exit(EXIT_FAILURE);
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        fprintf(stderr, "unable to initialize audio\n");
        exit(EXIT_FAILURE);
    }
    Mix_AllocateChannels(1); // only need background music
    music = Mix_LoadMUS("goat.mp3");
    if (music) {
        Mix_PlayMusic(music, -1);
    }
    */
    
    //seed rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    //while this is true, the game loop will keep iterating:
	bool keep_going = true;
    
    //declare ground and create an array with 2 players:
	Ground g;
	Player players[2];

    //ncurses initial parameters:
    //initialize the window screen:
	initscr();
    //disable echoing characters:
	noecho();
    //make sure keypad keys are recognized:
	keypad(stdscr, 1);
    //make cursor invisible:
    curs_set(0);
    
    //disable waiting for keyboard input to be entered when getch() is invoked:
    nodelay(stdscr, 1);

    //initialize ground and players:
    InitializeGame(g, players);
    
    //GAME LOOP:
	while (keep_going)
	{
        //incorporate input from the keyboard:
        ProcessKeyboard(g, players, keep_going);
        
        //update any changes to players:
        ApplyChanges(g, players, keep_going);
        
        //reprint window:
        DrawScreen(g, players);
    }//END GAME LOOP
    
    //de-initialize ncurses:
    echo();
	endwin();
    
    /*
    // Deinitialize Sound
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();
    */
    
	return 0;
}
