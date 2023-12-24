#include "Box3D.h"

Box3D::Box3D() : Box3D(0, 0, 0, 0, 0, 0)
{}

Box3D::Box3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
    c1.x = x1;
    c1.y = y1;
    c1.z = z1;
    c2.x = x1;
    c2.y = y1;
    c2.z = z1;
}

bool Box3D::inside(Box3D &b)
{
    return (
        c1.x>=b.c1.x &&
        c1.y>=b.c1.y &&
        c1.z>=b.c1.z &&
        c2.x<=b.c2.x &&
        c2.y<=b.c2.y &&
        c2.z<=b.c2.z
    );
}

bool Box3D::intersects(Box3D &b)
{
    //b.contains Box3D(x1, y1, z1, x1, y1, z1)
    return true;
}

bool Box3D::contains(Box3D &b)
{
    return (
        c1.x<=b.c1.x &&
        c1.y<=b.c1.y &&
        c1.z<=b.c1.z &&
        c2.x>=b.c2.x &&
        c2.y>=b.c2.y &&
        c2.z>=b.c2.z
    );
}

bool Box3D::operator==(Box3D &other)
{
    return (
        c1.x==other.c1.x &&
        c1.y==other.c1.y &&
        c1.z==other.c1.z &&
        c2.x==other.c2.x &&
        c2.y==other.c2.y &&
        c2.z==other.c2.z
    );
}
