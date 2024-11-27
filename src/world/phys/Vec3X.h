#pragma once
#include "Real.h"

template <typename T> class Vec3X
{
public:
    Vec3X(){}
    Vec3X(T x, T y, T z) {
        Vec3X::x = x;
        Vec3X::y = y;
        Vec3X::z = z;
    }

    T x;
    T y;
    T z;
protected:

private:
};
