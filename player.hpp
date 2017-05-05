#pragma once
#include "ground.hpp"
#include <vector>
#include <time.h>

#if defined(WIN32)
#include "curses-tanks-m\curses-tanks-m\Vec2D.hpp"

#else
#include "Vec2D.hpp"
#endif

enum Side
{
	LEFT,
	RIGHT
};

class Player
{
public:
	Player();
	void Draw(Ground & ground);
	void Initialize(int column, Side side);
	void DrawSettings(int turn);
	void PowerUp();
	void PowerDown();
	void AngleUp();
	void AngleDown();
    void DrawShots(int player, Ground & g, Player & other);
    void Shoot(Ground & ground, Player & other);
    void Hit(Vec2D &v, Player & other, Ground &g);

	int col;
	double power;
	double angle;
	bool hit;
    int score;
	Side s;
    bool is_shooting;
    std::vector<Vec2D> shots;
    clock_t shot_t0;
    size_t shots_length;
    std::vector<Vec2D> hits;

	static const int power_increment;
	static const double angle_increment;
};
