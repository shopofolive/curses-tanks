#pragma once
#include "ground.hpp"
#include <vector>

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

	int col;
	double power;
	double angle;
	bool hit;
    int score;
	Side s;
    bool is_shooting;
    std::vector<Vec2D> shots;

	static const int power_increment;
	static const double angle_increment;
};
