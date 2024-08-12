#pragma once
#include "Real.h"

template <typename T> class Point3X
{
public:
    Point3X(){}
    Point3X(T x, T y, T z) {
        Point3X::x = x;
        Point3X::y = y;
        Point3X::z = z;
    }

    T x;
    T y;
    T z;
protected:

private:
};
