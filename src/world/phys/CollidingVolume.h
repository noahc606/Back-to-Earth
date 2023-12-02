#pragma once
#include <vector>
#include "Box3D.h"
#include "Defs.h"

class CollidingVolume
{
public:
    /**/
    /**/
    CollidingVolume();
    virtual ~CollidingVolume();
    /**/

    /**/
    void box(double xPos, double yPos, double zPos, double length, double width, double height);

protected:

private:
    std::vector<Box3D> boxes;
    Point3D center;
};
