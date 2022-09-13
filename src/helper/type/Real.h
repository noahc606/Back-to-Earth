#pragma once
#include <string>

class Real
{
public:
    typedef long long t_ll;

    Real();
    Real(t_ll integer);
    Real(int integer);
    Real(double number);
    Real(t_ll integer, double decimal);

    bool isInteger() const;

    t_ll getInteger() const;
    double getDecimal() const;
    std::string getString() const;

    Real& abs();
    t_ll floor();
    t_ll ceil();

    Real operator- ( const Real& other );
    Real operator+ ( const Real& other );

    void operator= ( const Real& other );
    void operator= ( const t_ll& other );
    void operator= ( const int& other );
    void operator= ( const double& other );

    bool operator==( Real& other );
    bool operator< ( Real& other );
    bool operator>=( Real& other );
    bool operator> ( Real& other );
    bool operator<=( Real& other );

    t_ll integer = 0;
    double decimal = 0.0;
protected:

private:
};
