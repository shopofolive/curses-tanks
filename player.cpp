#include <sstream>
#include <iomanip>
#include <vector>
#include "player.hpp"
#include <cmath>

#if defined(WIN32)
#include "curses.h"
#include "curses-tanks-m\curses-tanks-m\Vec2D.hpp"
#else
#include <curses.h>
#include <cstdlib>
#include "Vec2D.hpp"
#endif

using namespace std;

//defined in ground.cpp:
extern int base_height_divisor;

//constants needed to modify power and angle after a keyboard input has been entered
const int Player::power_increment = 1;
const double Player::angle_increment = 1;
const double PI = 3.141592653589793238463;

//constructor
Player::Player()
{
	s = LEFT;
	col = 0;
	angle = 60.0;
	power = 30.0;
	hit = false;
    score = 0;
    is_shooting = false;
    
	// bool hit <- will be at end of shoot function.
	// checks after each function whether it hit player
	// if yes, trigger winner screen.
}

//define col, s, hit (called from InitializeGame() in main)
void Player::Initialize(int column, Side side)
{
	col = column;
	s = side;
    hit = false;
}

//place the tank (called from DrawScreen in main)
void Player::Draw(Ground & g)
{
	mvaddch(g.ground.at(col) - 1, col + 1, '@');
}

//called from ProcessKeyboard() in main
void Player::PowerUp()
{
	power += power_increment;
    if (power >= 100)
		power = 100;
}

//called from ProcessKeyboard in main
void Player::PowerDown()
{
	power -= power_increment;
	if (power <= 0)
		power = power_increment;
}

//called from ProcessKeyboard() in main
void Player::AngleUp()
{
	angle += angle_increment;
	if (angle >= 90.0)
		angle = 90.0;
}

//called from ProcessKeyboard() in main
void Player::AngleDown()
{
	angle -= angle_increment;
	if (angle < 0.0)
		angle = 0.0;
}

//called from DrawScreen in main
void Player::DrawSettings(int num)
{
	
	//position of Player 0 info
    int starting_column = 2;
	int line = 1;
	char player = (s == LEFT) ? '1' : '2';
	stringstream ss;

	//position of Player 1 info
    if (s == RIGHT)
		starting_column = COLS - 18;

	//print player info
    ss << setw(10) << left << "Player:" << player;
	mvaddstr(line++, starting_column, ss.str().c_str());

	ss = stringstream();
	ss << setw(10) << left << "Angle: " << setw(6) << angle;
	mvaddstr(line++, starting_column, ss.str().c_str());
	
	ss = stringstream();
	ss << setw(10) << left << "Power: " << setw(6) << power;
	mvaddstr(line++, starting_column, ss.str().c_str());
    
    ss = stringstream();
    ss << setw(10) << left << "Score: " << setw(6) << score;
    mvaddstr(line++, starting_column, ss.str().c_str());
}

//prints the trajectory of a shot, checks for hits and prints them if any,
//called from InitializeGame() in main
void Player::DrawShots(int player, Ground & g, Player & other)
{
    float time_elapsed;
    clock_t now;
    now = clock();
    time_elapsed = float( clock() - shot_t0 ) /  CLOCKS_PER_SEC;


    if (is_shooting)
    {
        //print trajectory:
        for (size_t s = 0; s< shots.size() - shots_length;s++)
        {
            if (shots.at(s).y > 0)
            {
                move(shots.at(s).y, shots.at(s).x);
                addch('*');
            }
        }
        
        if (shots_length > 0)
        {
            //delete the first element in the vector:
            if (time_elapsed > 0.02)
            {
                shots_length--;
                shot_t0 = clock();
            }
            
        }
        else
        {
            shots.clear();
            //print hits:
            if (time_elapsed < 0.7)
            {
                for (size_t s = 0; s< hits.size();s++)
                {
                    move(hits.at(s).x, hits.at(s).y);
                    addch('X');
                    
                }
            }
            else
            {
                //form a crater:
                for (size_t s = 0; s< hits.size();s++)
                {
                    int hit_line = hits.at(s).x;
                    int hit_col = hits.at(s).y;
                    
                    if (hit_line == g.ground.at(hit_col - 1))
                        g.ground.at(hit_col - 1) = g.ground.at(hit_col - 1) + 1;
                    
                    //adjust to make sure the ground does not encroach on the border:
                    if (g.ground.at(hit_col - 1) > LINES - 2)
                        g.ground.at(hit_col - 1) = LINES - 2;
                    
                    
                    //check if enemy tank is hit:
                    if ((hit_line == g.ground.at(other.col) - 1) && (hit_col == other.col + 1))
                    {
                        //enemy tank is hit!!!
                        other.hit = true;
                        flash();
                    }
                    
                    //check if self hit
                    if ((hit_line == g.ground.at(col) - 1) && (hit_col == col + 1))
                    {
                        //my tank is hit!!!
                        hit = true;
                        flash();
                    }
                }
                hits.clear();
                is_shooting = false;            }
        }
    }
}

//pushes the coordinates of hit spots into player.hits, so they could be printed when DrawShots() is invoked
//called from Shoot()
void Player::Hit(Vec2D &v, Player & other, Ground &g)
{
    //a vector that will contain the coordinates of the nine spots surrounding the spot where the shot landed:
    vector<Vec2D> hts;
    //a temp vector containing a pair of coordinates to be pushed into hts<Vec2D>:
    Vec2D h;
    
    //push the surrounding spots into hts:
    for (int i=-1;i<2;i++)
    {
        for(int j=-1;j<2;j++)
        {
            h.y = v.y + j;
            h.x = v.x + i;
            hts.push_back(h);
        }
    }
    
    //now push only those spots into player.hits that were below
    for (size_t i = 0; i<hts.size(); i++)
    {
        //define a temporary pair of int values to be pushed into player.hits:
        int hit_line = hts.at(i).y;
        int hit_col = hts.at(i).x;
        
        //if a hit spot is within acceptable range (this is to avoid an out-of-range error)
        //TODO: prevent player 0 from landing hits outside the vector:
        if (hit_col > 0 && hit_col < COLS - 5)
        {
            //and if the hit spot = a ground cell or the cell right above it:
            if ((hit_line == g.ground.at(hit_col)) || (hit_line + 1 == g.ground.at(hit_col)))
            {
                //push a Vec2D with these coordinates into player.hits:
                Vec2D v(hit_line, hit_col);
                hits.push_back(v);
            }
        }
    }
    
    refresh();
}

//called from ProcessKeyboard() in main
void Player::Shoot(Ground & g, Player & other)
{
    //define angle:
    double angle = this->angle / 180.0 * PI;
    //define how frequently each timestep occurs:
    double time_divisor = 5.0;
    //define force (with an arbitrary multiplier 0.2)
    Vec2D force(sin(angle) * power * 0.2, cos(angle) * power * 0.2);
    if (s == RIGHT)
        force.x = -force.x;
    //define gravity
    //because the second half of the equation below applies only for pN.y, x's gravity is 0 to annul those calculations:
    Vec2D gravity(0.0, -0.98);
    //the starting point of the shot trajectory:
    int p_col = col;
    int p_line = LINES - g.ground.at(col);
    
    //for Player 0, p_col must be adjusted to ensure that the shot begins one spot up and to the right of the tank:
    if (s == LEFT)
        p_col++;
    //vector containing the coordinates of the starting point:
    Vec2D p0(p_col,p_line);
    
    for (int i = 1; i < 50000; i++)
    {
        double di = i / time_divisor;
        //calculate the trajectory of the shot for both x and y simultaneously
        Vec2D pN = p0 + force * di + gravity * (di * di + di) * 0.5;
        //flip pN.y because in a ncurses's window y increases in the opposite direction compared to the Cartesian space:
        pN.y = LINES - pN.y;
        
        //if the shot trajectory crosses the left or right border:
        if (pN.x < 1 || pN.x >= COLS - 2)
            break;
        
        //if the shot trajectory hits a ground cell:
        if (pN.y >= g.ground.at((int)pN.x))
        {
            //delegate the task of pushing the coordinates of each hit spot into player.hits:
            Hit(pN, other, g);
            break;
        }
        
        //push the coordinates of each trajectory cell into player.shots so they can be printed later:
        Vec2D v((int)pN.x + 1, (int)pN.y - 1);
        shots.push_back(v);
        
        
    }
    //turn on is_shooting so the trajectory can start to be printed:
    is_shooting = true;
    //define the initial time of the shooting:
    shot_t0 = clock();
    //this value is needed to incrementally print the trajectory, animation style, in DrawShots():
    shots_length = shots.size();
}
