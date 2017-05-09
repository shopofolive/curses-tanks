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
#include <SDL2/SDL.h>
#include <SDL2_Mixer/SDL_Mixer.h>
#include <unistd.h>

using namespace std;

//defined in ground.cpp:
extern int base_height_divisor;

void PrintMessage(string s)
{
    stringstream ss;
    ss << s;
    move(1,1);
    addstr(ss.str().c_str());
    refresh();
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


//print title screen:
void TitleScreen()
{
    erase();
    stringstream ss;
    
    
    ss  << " TTTTTTTTTTTTTTT   AAAAAAAAAAA   NNNNNNN    NNNNN  KKKKK   KKKKK  SSSSSSSSSS" << endl;
    ss  << " TTTTTTTTTTTTTTT  AAAAAAAAAAAAA  NNNNNNNN   NNNNN  KKKKK  KKKKK  SSSSSSSSSSSS" << endl;
    ss  << " TTTTTTTTTTTTTTT  AAAAA   AAAAA  NNNNNNNNN  NNNNN  KKKKK KKKKK   SSSSS  SSSSS" << endl;
    ss  << "      TTTTT       AAAAA   AAAAA  NNNNNNNNNN NNNNN  KKKKKKKKKK     SSSSSSS  " << endl;
    ss  << "      TTTTT       AAAAAAAAAAAAA  NNNNN NNNNNNNNNN  KKKKKKKKKK        SSSSSSSS " << endl;
    ss << "      TTTTT       AAAAAAAAAAAAA  NNNNN  NNNNNNNNN  KKKKK KKKKK   SSSSS  SSSSS" << endl;
    ss  << "      TTTTT       AAAAA   AAAAA  NNNNN   NNNNNNNN  KKKKK  KKKKK  SSSSSSSSSSSS" << endl;
    ss << "      TTTTT       AAAAA   AAAAA  NNNNN    NNNNNNN  KKKKK   KKKKK  SSSSSSSSSS" << endl;
    ss  << " ------------------------------------------------------------------------------" << endl;
    ss << endl << endl << endl;
    
    move(5,0);
    addstr(ss.str().c_str());
    refresh();
    MySleep(2000);
}

void InstructionScreen()
{
    erase();
    stringstream ss;
    
    ss << '\t' << "        BE THE FIRST TO REACH 3 POINTS!  " << endl << endl;
    ss << '\t' << "-----------------------------------------" << endl;
    ss << '\t' << "|               CONTROLS:               |" << endl;
    ss << '\t' << "-----------------------------------------" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|  Player 1:                 Player 2:  |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|  q         INCREASE ANGLE          ]  |" << endl;
    ss << '\t' << "|  a         DECREASE ANGLE          '  |" << endl;
    ss << '\t' << "|  w         INCREASE POWER          [  |" << endl;
    ss << '\t' << "|  s         DECREASE ANGLE          ;  |" << endl;
    ss << '\t' << "|  z         MOVE LEFT               .  |" << endl;
    ss << '\t' << "|  x         MOVE RIGHT              /  |" << endl;
    ss << '\t' << "|  [space]   SHOOT             [enter]  |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "|                                       |" << endl;
    ss << '\t' << "-----------------------------------------" << endl;
    ss << endl << endl;
    ss << '\t' << "           PRESS ANY KEY TO START        " << endl;
    
    addstr(ss.str().c_str());
    refresh();
}


void PlayerOneWins()
{
    erase();

    stringstream ss;
    
    //will center TANKS vertically
    int numOfEndls = (LINES - 22) / 2;
    //askii art title screen
    
    for (int i = 0; i < numOfEndls; i++)
    {
        ss << endl;
    }
    ss << '\t' << "PPPPP   LL       AAAA   YY  YY  EEEEE  RRRRR      11 " << endl;
    ss << '\t' << "PP  PP  LL      AA  AA   YYYY   EE     RR  RR   1111" << endl;
    ss << '\t' << "PPPPP   LL      AAAAAA    YY    EEEEE  RRRRR      11" << endl;
    ss << '\t' << "PP      LL      AA  AA    YY    EE     RR RR      11" << endl;
    ss << '\t' << "PP      LLLLLL  AA  AA    YY    EEEEE  RR  RR    1111" << endl << endl;
    
    ss << '\t' << "WW    WW  IIII  NN  NN   SSSS   !!" << endl;
    ss << '\t' << "WW    WW   II   NNN NN  SSS     !! " << endl;
    ss << '\t' << "WW WW WW   II   NNNNNN    SSS   !!" << endl;
    ss << '\t' << "WWWWWWWW   II   NN NNN  S  SSS  " << endl;
    ss << '\t' << "WW WW WW  IIII  NN  NN   SSSS   !! " << endl << endl;
    
    ss << '\t' << "PRESS Q TO QUIT " << endl << endl;
    ss << '\t' << "PRESS R TO RESTART " << endl;
    
    move(1,1);
    addstr(ss.str().c_str());
    refresh();
}

void PlayerTwoWins()
{
    erase();
    stringstream ss;
    
    
    //will center TANKS vertically
    int numOfEndls = (LINES - 22) / 2;
    //askii art title screen
    
    for (int i = 0; i < numOfEndls; i++)
    {
        ss << endl;
    }
    ss << '\t' << "PPPPP   LL       AAAA   YY  YY  EEEEE  RRRRR    22222" << endl;
    ss << '\t' << "PP  PP  LL      AA  AA   YYYY   EE     RR  RR  22   22" << endl;
    ss << '\t' << "PPPPP   LL      AAAAAA    YY    EEEEE  RRRRR       22" << endl;
    ss << '\t' << "PP      LL      AA  AA    YY    EE     RR RR     22" << endl;
    ss << '\t' << "PP      LLLLLL  AA  AA    YY    EEEEE  RR  RR  2222222" << endl << endl;
    
    ss << '\t' << "WW    WW  IIII  NN  NN   SSSS   !!" << endl;
    ss << '\t' << "WW    WW   II   NNN NN  SSS     !! " << endl;
    ss << '\t' << "WW WW WW   II   NNNNNN    SSS   !!" << endl;
    ss << '\t' << "WWWWWWWW   II   NN NNN  S  SSS  " << endl;
    ss << '\t' << "WW WW WW  IIII  NN  NN   SSSS   !! " << endl << endl;
    
    ss << '\t' << "PRESS Q TO QUIT " << endl << endl;
    ss << '\t' << "PRESS R TO RESTART " << endl;
    
    move(1,1);
    addstr(ss.str().c_str());
    refresh();
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
    for (int i=0;i<2;i++)
    {
        players[i].Draw(g);
        players[i].DrawShots(0, g, players[1 - i]);
        players[i].DrawSettings(i);
        players[i].Move();
    }
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
            //if the player isn't already moving and the tank is still within bounds:
            if (!players[0].is_moving && players[0].col > 0)
            {
                //this will be switched to false after the motion timer has expired
                players[0].is_moving = true;
                //start motion timer:
                players[0].move_t0 = clock();
                //move the tank:
                players[0].col--;
            }
        break;
            
        case 'x':
            //if the player isn't already moving and the tank is still within bounds:
            if (!players[0].is_moving && players[0].col < COLS - 3)
            {
                players[0].is_moving = true;
                players[0].move_t0 = clock();
                players[0].col++;
            }
            break;
            
        //space key:
        case 32:
            //invoke Shoot() only if the previous shot (if any) has been fully printed and processed:
            //this ensures that only 1 shot per each player is fired at a time:
            if (!players[0].is_shooting)
            {
                
                Mix_Music *shot0;
                shot0 = Mix_LoadMUS("/Users/dshapovalov/VirtualBox VMs/Shared Folder/curses-tanks/curses-tanks-m/curses-tanks-m/shot.mp3");
                //finding out the current directory:
                if (shot0) {
                    Mix_PlayMusic(shot0, 1);
                }
                else
                {
                    //for debugging:
                    PrintMessage(SDL_GetError());
                }
                
                players[0].Shoot(g, players[1]);
            }
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
            if (!players[1].is_moving && players[1].col > 0)
            {
                players[1].is_moving = true;
                players[1].move_t0 = clock();
                players[1].col--;
            }
            break;
            
        case '/':
            if (!players[1].is_moving && players[1].col < COLS - 3)
            {
                players[1].is_moving = true;
                players[1].move_t0 = clock();
                players[1].col++;
            }
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
            {
                
                Mix_Music *shot1;
                shot1 = Mix_LoadMUS("/Users/dshapovalov/VirtualBox VMs/Shared Folder/curses-tanks/curses-tanks-m/curses-tanks-m/shot.mp3");
                //finding out the current directory:
                if (shot1) {
                    Mix_PlayMusic(shot1, 1);
                }
                else
                {
                    //for debugging:
                    PrintMessage(SDL_GetError());
                }
                
                players[1].Shoot(g, players[0]);
            }
            break;
            
        //if not keyboard input is entered, proceed to the next iteration of the game loop
        case ERR:
            break;
    }
}

//update scores; quit or restart game:
void ApplyChanges(Ground &g, Player *players, bool &keep_going)
{
    if (players[0].hit || players[1].hit)
    {
        //update score:
        for (int i=0;i<2;i++)
        {
            if(players[i].hit)
            {
                players[1 - i].score++;
                break;
            }
        }
        
        //if neither player has scored 3 hits yet:
        if (players[0].score < 3 && players[1].score < 3)
        {
            flash();
            //erase current ground and players data and re-initialize game:
            g.ground.clear();
            InitializeGame(g, players);
        }
        else
        {
            if (players[0].score == 3)
            {
                PlayerOneWins();
            }
            else if (players[1].score == 3)
            {
                PlayerTwoWins();
            }
            //enable waiting for keyboard input after getch():
            nodelay(stdscr, 0);
            
            int c = getch();
            
            //to restart:
            if (c == 'r')
            {
                players[0].score = 0;
                players[1].score = 0;
                //erase current ground and players data and re-initialize game:
                g.ground.clear();
                InitializeGame(g, players);
                //disable waiting for keyboard input after getch() let the game loop continue spinning after each iteration:
                nodelay(stdscr, 1);
            }
            //to quit:
            else
            {
                //break out of the game loop:
                keep_going = false;
            }
        }
    }
}


int main(int argc, char * argv[])
{
    
    
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
    
    
    //SDL_Mixer stuff:
    Mix_Music *bkg;
    
    
    // Initialize music.
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        PrintMessage("unable to initialize SDL\n");
        exit(EXIT_FAILURE);
    }
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        PrintMessage("unable to initialize SDL_mixer\n");
        exit(EXIT_FAILURE);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        PrintMessage("unable to initialize audio\n");
        exit(EXIT_FAILURE);
    }
    
    TitleScreen();
    
    InstructionScreen();
    
    //play background music:
    Mix_AllocateChannels(2);
    
    getch();
    
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
    
    // Deinitialize Sound
    Mix_HaltMusic();
    Mix_FreeMusic(bkg);
    Mix_CloseAudio();
    Mix_Quit();
    
    
	return 0;
}
