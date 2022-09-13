#pragma once
#include "Real.h"
#include "Point3D.h"

class Box3D
{
public:
    Box3D();
    Box3D(Real x1, Real y1, Real z1, Real x2, Real y2, Real z2);

    Point3D c1; //Corner 1
    Point3D c2; //corner 2

    bool inside(Box3D& b);
    bool intersects(Box3D& b);
    bool contains(Box3D& b);

    /**
        ==: 'left' is the same as 'right'
    */
    bool operator==( Box3D& other );
protected:

private:

};
