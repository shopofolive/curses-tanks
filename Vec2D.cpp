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

Vec2D::Vec2D(double sx, double sy)
{
    x = sx;
    y = sy;
}

Vec2D::Vec2D(int sx, int sy)
{
    x = double(sx);
    y = double(sy);
}

Vec2D::Vec2D(const Vec2D& v)
{
    x = v.x;
    y = v.y;
}

Vec2D Vec2D::operator*(const double& scalar) const
{
    return Vec2D(x*scalar, y*scalar);
}

Vec2D Vec2D::operator*(const int scalar) const
{
    return Vec2D(x*scalar, y*scalar);
}

Vec2D Vec2D::operator+(const Vec2D &v) const
{
    return Vec2D(x+v.x, y+v.y);
}
