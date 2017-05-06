//
//  Vec2D.cpp
//  curses-tanks-m
//
//  Created by Dimitri Shapovalov on 5/2/17.
//  Copyright © 2017 Carthage College. All rights reserved.
//  https://codereview.stackexchange.com/questions/26608/review-of-2d-vector-class

#include "Vec2D.hpp"
#include <cmath>


Vec2D::Vec2D()
{
    x = 0.0;
    y = 0.0;
}

//sometimes two doubles may go here:
Vec2D::Vec2D(double sx, double sy)
{
    x = sx;
    y = sy;
}

//sometimes two ints may go here:
Vec2D::Vec2D(int sx, int sy)
{
    x = double(sx);
    y = double(sy);
}

//operator overload for * in case of two doubles
Vec2D Vec2D::operator*(const double& scalar) const
{
    return Vec2D(x*scalar, y*scalar);
}

//operator overload for * in case of two ints
Vec2D Vec2D::operator*(const int scalar) const
{
    return Vec2D(x*scalar, y*scalar);
}

//operator overload for +
Vec2D Vec2D::operator+(const Vec2D &v) const
{
    return Vec2D(x+v.x, y+v.y);
}
