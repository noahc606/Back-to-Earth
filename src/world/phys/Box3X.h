#pragma once
#include "Real.h"
#include "Vec3X.h"

template <typename T> class Box3X
{
public:
    Box3X(){}

/*
    Constructs a box with all faces parallel to their respective X, Y, Z axes.
    The box is defined by two points (x1, y1, z1) and (x2, y2, z2).

    Points Corner 1 (c1) and Corner 2 (c2) are assigned so that c1.x<=c2.x, c1.y<=c2.y, and c1.z<=c2.z.
*/
    Box3X(T x1, T y1, T z1, T x2, T y2, T z2) {
        c1.x = std::min(x1, x2);    c2.x = std::max(x1, x2);
        c1.y = std::min(y1, y2);    c2.y = std::max(y1, y2);
        c1.z = std::min(z1, z2);    c2.z = std::max(z1, z2);
    }

/*
    Does this box contain point 'p' (may be on edges/faces)?
*/
    bool contains(Vec3X<T> p) {
        return (
            c1.x<=p.x && p.x<=c2.x &&
            c1.y<=p.y && p.y<=c2.y &&
            c1.z<=p.z && p.y<=c2.z
        );
    }

/*
    Does this box completely contain box 'b' (may be on edges/faces)?
*/
    bool contains(Box3X<T> b) {
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
        Does this box at all intersect box 'b' (vertices/edges/faces count)?
    */
    bool intersects(Box3X<T> b) {
        return (
            c1.x<=b.c2.x && b.c1.x<=c2.x &&
            c1.y<=b.c2.y && b.c1.y<=c2.y &&
            c1.z<=b.c2.z && b.c1.z<=c2.z
        );
    }

    /*
        Get the intersection between this box and box 'b'.
        If there is no intersection, return Box(0, 0, 0, 0, 0, 0);
    */
    Box3X<T> intersection(Box3X<T> b) {
        if(!intersects(b)) {
            return Box3X(0, 0, 0, 0, 0, 0);
        }

        T rx1 = c1.x, rx2 = b.c2.x;
        T ry1 = c1.y, ry2 = b.c2.y;
        T rz1 = c1.z, rz2 = b.c2.z;
        if(b.c1.x>c1.x) rx1 = b.c1.x;
        if(b.c2.x>c2.x) rx2 = c2.x;
        if(b.c1.y>c1.y) ry1 = b.c1.y;
        if(b.c2.y>c2.y) ry2 = c2.y;
        if(b.c1.z>c1.z) rz1 = b.c1.z;
        if(b.c2.z>c2.z) rz2 = c2.z;

        return Box3X(rx1, ry1, rz1, rx2, ry2, rz2);
    }

    /*
        Same as intersects(), but vertices/edges/faces DON'T count
    */
    bool collides(Box3X<T> b) {
        return (
            c1.x<b.c2.x && b.c1.x<c2.x &&
            c1.y<b.c2.y && b.c1.y<c2.y &&
            c1.z<b.c2.z && b.c1.z<c2.z
        );
    }

    /**
        ==: 'left' is the same as 'right'
    */
    bool operator==( Box3X<T> other ) {
        return (
            c1.x==other.c1.x &&
            c1.y==other.c1.y &&
            c1.z==other.c1.z &&
            c2.x==other.c2.x &&
            c2.y==other.c2.y &&
            c2.z==other.c2.z
        );
    }

    Vec3X<T> c1; //Corner 1
    Vec3X<T> c2; //Corner 2
protected:

private:

};
