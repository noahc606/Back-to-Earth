#pragma once
#include <string>

class Real
{
public:
    Real();
    Real(int64_t integer);
    Real(int integer);
    Real(double number);
    Real(int64_t integer, double decimal);

    bool isInteger() const;

    int64_t getInteger() const;
    double getDecimal() const;
    std::string getString() const;

    Real& abs();
    int64_t floor();
    int64_t ceil();

    Real operator+ (int other);
    Real operator- (int other);

    Real operator- ( const Real& other );
    Real operator+ ( const Real& other );

    void operator= ( const Real& other );
    void operator= ( const int64_t& other );
    void operator= ( const int& other );
    void operator= ( const double& other );

    bool operator==( Real& other );
    bool operator< ( Real& other );
    bool operator>=( Real& other );
    bool operator> ( Real& other );
    bool operator<=( Real& other );

    int64_t integer = 0;
    double decimal = 0.0;
protected:

private:
};
