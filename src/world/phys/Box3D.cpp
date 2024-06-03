#include "Box3D.h"

Box3D::Box3D() : Box3D(0, 0, 0, 0, 0, 0)
{}

/*
    Constructs a box with all faces parallel to their respective X, Y, Z axes.
    The box is defined by two points (x1, y1, z1) and (x2, y2, z2).

    Points Corner 1 (c1) and Corner 2 (c2) are assigned so that c1.x<=c2.x, c1.y<=c2.y, and c1.z<=c2.z.
*/
Box3D::Box3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
    c1.x = std::min(x1, x2);    c2.x = std::max(x1, x2);
    c1.y = std::min(y1, y2);    c2.y = std::max(y1, y2);
    c1.z = std::min(z1, z2);    c2.z = std::max(z1, z2);
}

/*
    Does this box contain point 'p' (may be on edges/faces)?
*/
bool Box3D::contains(Point3D p)
{
    return (
        c1.x<=p.x && p.x<=c2.x &&
        c1.y<=p.y && p.y<=c2.y &&
        c1.z<=p.z && p.y<=c2.z
    );
}
/*
    Does this box completely contain box 'b' (may be on edges/faces)?
*/
bool Box3D::contains(Box3D b)
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

/*
    Does this box at all intersect box 'b' (edges/faces count)?
*/
bool Box3D::intersects(Box3D b)
{
    return (
        c1.x<b.c2.x && b.c1.x<c2.x &&
        c1.y<b.c2.y && b.c1.y<c2.y &&
        c1.z<b.c2.z && b.c1.z<c2.z
    );
}

bool Box3D::operator==(Box3D other)
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
