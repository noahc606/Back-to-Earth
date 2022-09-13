#include "Color.h"

Color::Color():
Color::Color(255, 255, 255){}

Color::Color(uint32_t p_value)
{
    r = (p_value >> 24) & 0xff;
    g = (p_value >> 16) & 0xff;
    b = (p_value >>  8) & 0xff;
    a = (p_value >>  0) & 0xff;
}

Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b):
Color::Color(p_r, p_g, p_b, 255){}

Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    set(p_r, p_g, p_b, p_a);
}

Color::~Color(){}

uint32_t Color::getValue()
{
    return  256*256*256*r+
            256*256*g+
            256*b+
            a;
}

void Color::set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    r = p_r;
    g = p_g;
    b = p_b;
    a = p_a;
}

void Color::mod(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    r = r*p_r/255;
    g = g*p_g/255;
    b = b*p_b/255;
    a = a*p_a/255;
}
