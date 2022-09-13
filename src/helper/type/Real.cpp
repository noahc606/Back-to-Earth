#include "Real.h"
#include <cmath>
#include <math.h>
#include <iostream>
#include <sstream>

Real::Real() { *this = 0; }
Real::Real(Real::t_ll integer) { *this = integer; }
Real::Real(int integer) { *this = integer; }
Real::Real(double number) { *this = number; }
Real::Real(Real::t_ll integer, double decimal)
{
    Real::integer = integer;
    Real::decimal = decimal;
}

bool Real::isInteger() const { return decimal==0.0; }

Real::t_ll Real::getInteger() const { return integer; }
double Real::getDecimal() const { return decimal; }
std::string Real::getString() const
{
    std::stringstream ss1; ss1 << getInteger();

    std::stringstream ss2;
    std::stringstream temp; temp << getDecimal();
    if( temp.str().size()>1 ) {
        ss2 << temp.str().substr(2);
    } else {
        ss2 << "0";
    }

    return ss1.str()+"."+ss2.str();
}

Real& Real::abs()
{
    integer = std::abs(integer);
    return *this;
}

Real::t_ll Real::floor()
{
    if( decimal>0.0 ) {
        if(integer>=0) {
            return integer;
        }
        return integer-1;
    }
    return integer;
}

Real::t_ll Real::ceil()
{
    if( decimal>0.0 ) {
        if(integer>=0) {
            return integer+1;
        }
        return integer;
    }
    return integer;
}

Real Real::operator- ( const Real& other )
{
    Real temp = other;
    temp.integer = -temp.integer;

    return (*this)+temp;
}

Real Real::operator+ ( const Real& other )
{
    t_ll r1Int = (*this).getInteger();
    double r1Dec = (*this).getDecimal();
    t_ll r2Int = other.getInteger();
    double r2Dec = other.getDecimal();

    Real result;
    bool carry = false;

    //Calculate the int component
    result.integer = r1Int+r2Int-carry;

    //Calculate the decimal
    if( r1Int<0 ) {
        if( r2Int<0 ) {
            /** -r1, -r2 */
            //Calculate the decimal component and determine carry.
            result.decimal = r1Dec+r2Dec;
            if( result.decimal>=1.0 ) {
                carry = true;
                result.decimal--;
            }
            //Calculate the int component
            result.integer = r1Int+r2Int-carry;
        } else {
            /** -r1, +r2 */


            //Calculate the decimal component and determine carry.
            result.decimal = -r1Dec+r2Dec;
            if( result.decimal<0 ) {
                carry = true;
                result.decimal++;
            }

        }
    } else {
        if( r2Int<0 ) {
            /** +r1, -r2 */

        } else {
            /** +r1, +r2 */
            //Calculate the decimal component and determine carry.
            result.decimal = r1Dec+r2Dec;
            if( result.decimal>=1.0 ) {
                carry = true;
                result.decimal--;
            }
            //Calculate the int component
            result.integer += carry;
        }
    }

    //Return result
    return result;
}

bool Real::operator==(Real& other)
{
    return (
        integer==other.getInteger() &&
        decimal==other.getDecimal()
    );
}


void Real::operator=(const Real& other)
{
    decimal = other.getDecimal();
    integer = other.getInteger();
}
void Real::operator=(const t_ll& other)
{
    integer = other;
    decimal = 0.0;
}
void Real::operator=(const int& other)
{
    integer = other;
    decimal = 0.0;
}
void Real::operator=(const double& other)
{
    //Set int component
    integer = other;

    //Set decimal component
    if(other>=0) {
        decimal = other-std::floor(other);
    } else {
        decimal = -(other-std::ceil(other));
    }
}

bool Real::operator<(Real& other)
{
    if( integer!=other.getInteger() ) {
        return integer<other.getInteger();
    } else {
        if( integer>=0 ) {
            return decimal<other.getDecimal();
        } else {
            return decimal>other.getDecimal();
        }
    }
}

bool Real::operator>=(Real& other) { return !((*this)<other); }

bool Real::operator>(Real& other)
{
    if( integer!=other.getInteger() ) {
        return integer>other.getInteger();
    } else {

        if( integer>=0 ) {
            return decimal>other.getDecimal();
        } else {
            return decimal<other.getDecimal();
        }
    }
}

bool Real::operator<=(Real& other) { return !((*this)>other); }

/*
double Real::truncateDecimal(const double& other)
{
    return
}*/
