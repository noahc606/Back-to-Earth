#pragma once
#include "Real.h"
#include "Point3D.h"

class Box3D
{
public:
    Box3D();
    Box3D(double x1, double y1, double z1, double x2, double y2, double z2);

    Point3D c1; //Corner 1
    Point3D c2; //Corner 2

    bool contains(Point3D p);
    bool contains(Box3D b);
    bool intersects(Box3D b);
    bool intersectsStrict(Box3D b);

    /**
        ==: 'left' is the same as 'right'
    */
    bool operator==( Box3D other );
protected:

private:

};
