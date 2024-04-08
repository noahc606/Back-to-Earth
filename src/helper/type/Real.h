#pragma once
#include <cstdint>
#include <string>

/**
    - An implementation of a "real number" type that focuses on accuracy and precision rather than speed.
    Includes some common functions (abs, floor, ceil) and math operators (+, -, *, divide).

    - When speed is really important you should use the usual ints/doubles.

    - Improved accuracy on systems that support long doubles:
    (product of Reals)        1e18*1.01 = 10100...000.208008
    printf("%s\n", (Real(1000000000000000000)*Real(1, 0.01)).toString().c_str());

    (product of long doubles) 1e18*1.01 = 10100...008.875000
    printf("%Lf\n", (long double)1000000000000000000*(long double)1.01);
 * 
*/

class Real
{
public:
    /* Constructors */
    Real();                                                 // Set value to 0.0
    Real(int64_t number);                                   // Set value to an integer
    Real(int number);                                       
    Real(int64_t integer, long double decimal);             // Set value to a decimal number (provide integer and also a double between 0-0.999...).

    /* Basic information getters */
    //int64_t iPart() const;                                // Return human-understandable integer portion of number
    int64_t iReal() const;                                  // Return stored integer portion (any int64_t) of number
    //double dPart() const;                                 // Return human-understandable decimal portion of number
    long double dReal() const;                              // Return stored decimal portion (between 0-0.999...) of number
    bool isInteger() const;                                 // Return whether number is an integer
    std::string toString() const;                           // Return string representation of number

    /* Math functions (one input) */
    Real abs() const;                                       // Return absolute value of the number
    Real floor() const;                                     // Return floor of the number
    Real ceil() const;                                      // Return ceiling of the number
    Real negation() const;                                  // Return number *-1
    Real inverse() const;
    Real nearestReal(long double impreciseVal) const;

    /* Math functions (two inputs) */
    Real timesFrac(long double frac);

    /* Math operators (two inputs) */
    template <typename T> Real operator+ (T other) const    // Return sum of this number and another number
    {
        Real o(other);

        //Add int and decimal parts
        int64_t iSum = i+o.iReal();
        double dSum = d+o.dReal();

        //Carry overflowing decimal to +1
        if(dSum>=1.0) {
            iSum++;
            dSum -= 1.0;
        }
    
        return Real(iSum, dSum);
    }
    template <typename T> Real operator- (T other) const    // Return difference
    {
        Real o(other);
        return (*this)+o.negation();
    }
    template <typename T> Real operator* (T other) const
    {
        //Store important values
        Real o(other);
        int64_t i1 = iReal();       int64_t i2 = o.iReal();
        long double d1 = dReal();   long double d2 = o.dReal();
        
        //Calculate (i1+d1)(i2+d2)
        Real fila(i1*i2, d1*d2);            // First (always integer) + Last (always <1.0)
        Real ou = Real(i1).timesFrac(d2);   // Outer
        Real in = Real(i2).timesFrac(d1);   // Inner

        //Return
        return fila+ou+in;
    }
    template <typename T> Real operator/ (T other) const
    {
        Real o(other);
        return (*this)*o.inverse();
    }

    /* Setters */
    template <typename T> void operator= ( T other )       // Set this number to another number
    {
        Real o(other);
        i = o.iReal();
        d = o.dReal();
    };
    template <typename T> void operator+= ( T other ) { (*this)=(*this)+other; };
    template <typename T> void operator-= ( T other ) { (*this)=(*this)-other; };
    template <typename T> void operator*= ( T other ) { (*this)=(*this)*other; };
    template <typename T> void operator/= ( T other ) { (*this)=(*this)/other; };

    /* Conditionals */
    template <typename T> bool operator==( T other ) const  //Is this num == to another number?
    {
        Real o(other);
        return ( i==o.iReal() && d==o.dReal() );
    };
    template <typename T> bool operator< ( T other ) const  // <  ?
    {
        Real o(other);

        if( i!=o.iReal() ) {
            return i<o.iReal();
        } else {
            if( i>=0 ) {
                return d<o.dReal();
            } else {
                return d>o.dReal();
            }
        }
    };
    template <typename T> bool operator>=( T other ) const  // >= ?
    { return !((*this)<other); };
    template <typename T> bool operator> ( T other ) const  // >  ?
    { return ((*this)<other && (*this)!=other ); };
    template <typename T> bool operator<=( T other ) const  // <= ?
    { return !((*this)>other); };


private:
    int64_t i = 0;                                          //Integer part of this real number. Can be any int64_t value.
    /*
        Decimal portion of this real number. Should be any double value from 0-0.999.
        With negative i values the "human-recognizable" value becomes different than the true value.
        For example, if i=-11, d=0.1 => human-recognizable value would be -10.9 since -11+0.1 = -10.9.
    */
    long double d = 0.0;

    long double validatedDecimal(long double val);
};