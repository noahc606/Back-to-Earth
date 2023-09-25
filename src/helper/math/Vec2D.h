#pragma once

class Vec2D
{
public:
    Vec2D(double angle, double length);
    virtual ~Vec2D();

protected:
    double cX; double cY;
    double length;

private:
};
