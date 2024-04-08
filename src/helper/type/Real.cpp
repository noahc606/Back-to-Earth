#include "Real.h"
#include <cmath>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <sstream>

/**/
Real::Real() { i = 0; d = 0.0; }
Real::Real(int64_t number) { i = number; d = 0.0; }
Real::Real(int number): Real((int64_t)number) {};
Real::Real(int64_t integer, long double decimal)
{
    i = integer;
    d = validatedDecimal(decimal);
}

/**/
int64_t Real::iReal() const { return i; }
long double Real::dReal() const { return d; }
bool Real::isInteger() const { return d==0.0; }
std::string Real::toString() const
{
    std::stringstream ss1;
    std::stringstream temp;
    if(i>=0||d==0.0) {
        ss1 << i;

        temp << std::fixed << d;
    } else {
        if(i==-1) {
            ss1 << "-";
        }
        ss1 << (i+1);
        temp << (1.0-d);
    }

    std::stringstream ss2;
    if( temp.str().size()>1 ) {
        ss2 << temp.str().substr(2);
    } else {
        ss2 << "0";
    }

    return ss1.str()+"."+ss2.str();
}

/**/
Real Real::abs() const {
    if(i>0) {
        return (*this);
    } else {
        return Real(-i, d);
    }

}
Real Real::floor() const
{
    if(d==0.0) { return Real(i); }   //Positive number OR d==0.0
    return Real(i);                  //Negative decimal number
}
Real Real::ceil() const
{
    if(d==0.0) { return Real(i); }  //Negative number OR d==0.0
    return Real(i+1);               //Positive decimal number
}
Real Real::negation() const
{
    int64_t iN = -i;
    
    double dN = d;
    if(d!=0) {
        iN--;
        dN = 1.0-d;
    }

    return Real(iN, dN);
}
Real Real::inverse() const
{
    if((*this)==0) {
        printf("Warning: 0 has no real inverse, returning INT64_MAX instead.");
        return INT64_MAX;
    }

    return nearestReal(1.0/((long double)i+d));
}
Real Real::nearestReal(long double impreciseVal) const
{
    int64_t ri;
    long double rd;

    if(impreciseVal>=0.0) {
        rd = impreciseVal-std::floor(impreciseVal);
        ri = impreciseVal;
    } else {
        rd = 1.0+(impreciseVal-std::ceil(impreciseVal));
        if(rd>=1.0) {
            ri = impreciseVal;
            rd -= 1.0;
        } else {
            ri = impreciseVal-1;
        }
    }

    return Real(ri, rd);
}

Real Real::timesFrac(long double frac)
{
    if(frac<0||frac>=1.0) {
        printf("Warning: Multiplication should have been used, returning the original number instead.");
        return (*this);
    }

    Real first = nearestReal((long double)i*frac);
    return Real(first+nearestReal(d*frac));
}

long double Real::validatedDecimal(long double val)
{
    bool warn = false;
    long double res;

    if(val>=1.0) {
        res = val-std::floor(val);
        warn = true;
    } else if(val<0) {
        res = 1.0+(val-std::ceil(val));
        warn = true;
    } else {
        res = val;
    }

    if(warn) {
        std::cout << "WARNING: input invalid decimal " << val << " which should be >=0 and <1 (truncated to " << res << ")\n";
    }
    return res;
}