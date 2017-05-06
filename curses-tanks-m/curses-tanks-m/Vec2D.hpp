//
//  Vec2D.hpp
//  curses-tanks-m
//
//  Created by Dimitri Shapovalov on 5/2/17.
//  Copyright © 2017 Carthage College. All rights reserved.
//

#ifndef Vec2D_hpp
#define Vec2D_hpp

#include <cmath>

class Vec2D {
public:
    double x, y;
    
    Vec2D();
    Vec2D(double sx, double sy);
    Vec2D(int sx, int sy);
    
    Vec2D operator*(const double&) const;
    
    Vec2D operator*(const int) const;
    
    Vec2D operator+(const Vec2D&) const;
};


#endif
