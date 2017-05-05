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

extern int base_height_divisor;
extern int max_height_divisor;

const int Player::power_increment = 1;
const double Player::angle_increment = 1;
const double PI = 3.141592653589793238463;

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

void Player::Initialize(int column, Side side)
{
	col = column;
	s = side;
    hit = false;
}

void Player::Draw(Ground & g)
{
	mvaddch(g.ground.at(col) - 1, col + 1, '@');
}

void Player::PowerUp()
{
	power += power_increment;
    if (power >= 100)
		power = 100;
}

void Player::PowerDown()
{
	power -= power_increment;
	if (power <= 0)
		power = power_increment;
}

void Player::AngleUp()
{
	angle += angle_increment;
	if (angle >= 90.0)
		angle = 90.0;
}

void Player::AngleDown()
{
	angle -= angle_increment;
	if (angle < 0.0)
		angle = 0.0;
}

void Player::DrawSettings(int num)
{
	
	int starting_column = 2;
	int line = 1;
	char player = (s == LEFT) ? '1' : '2';
	stringstream ss;

	if (s == RIGHT)
		starting_column = COLS - 18;

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

void Player::DrawShots(int player, Ground & g, Player & other)
{
    float time_elapsed;
    clock_t now;
    now = clock();
    time_elapsed = float( clock() - shot_t0 ) /  CLOCKS_PER_SEC;


    if (is_shooting)
    {
        
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

void Player::Hit(Vec2D &v, Player & other, Ground &g)
{
    //all nine hit coordinates will be pushed here:
    vector<Vec2D> hts;
    //a containter variable that will take the coordinates of each hit and be pushed into the vector:
    Vec2D h;
    
    //define the vector:
    for (int i=-1;i<2;i++)
    {
        for(int j=-1;j<2;j++)
        {
            h.y = v.y + j;
            h.x = v.x + i;
            hts.push_back(h);
        }
    }
    
    for (size_t i = 0; i<hts.size(); i++)
    {
        //TODO: prevent hits from landing outside the ground vector
        int hit_line = hts.at(i).y;
        int hit_col = hts.at(i).x;
        
        if ((hit_line == g.ground.at(hit_col)) || (hit_line + 1 == g.ground.at(hit_col)))
        {
            Vec2D v(hit_line, hit_col);
            hits.push_back(v);
        }
    }
    
    refresh();
}



void Player::Shoot(Ground & g, Player & other)
{
    double angle = this->angle / 180.0 * PI;
    
    double time_divisor = 5.0;
    Vec2D force(sin(angle) * power * 0.2, cos(angle) * power * 0.2);
    if (s == RIGHT)
        force.x = -force.x;
    Vec2D gravity(0.0, -0.98);
    //the starting point is above the actual position of the tank, to prevent self-injury in the first moments of the shot;
    int p_col = col;
    int p_line = LINES - g.ground.at(col);
    if (s == LEFT)
        p_col++;
    Vec2D p0(p_col,p_line);
    
    for (int i = 1; i < 50000; i++)
    {
        double di = i / time_divisor;
        Vec2D pN = p0 + force * di + gravity * (di * di + di) * 0.5;
        pN.y = LINES - pN.y;
        
        if (pN.x < 1 || pN.x >= COLS - 2)
            break;
        
        if (pN.y >= g.ground.at((int)pN.x))
        {
            Hit(pN, other, g);
            break;
        }
        
        //move((int)pN.y - 1, (int)pN.x + 1);
        //addch('*');
        Vec2D v((int)pN.x + 1, (int)pN.y - 1);
        shots.push_back(v);
        
        
    }
    is_shooting = true;
    shot_t0 = clock();
    shots_length = shots.size();
}
