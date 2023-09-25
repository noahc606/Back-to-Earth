#include "Color.h"

Color::Color():
Color::Color(255, 255, 255){}

Color::Color(uint32_t p_value)
{
    r = (p_value>>24)&0xFF;
    g = (p_value>>16)&0xFF;
    b = (p_value>> 8)&0xFF;
    a = (p_value>> 0)&0xFF;
}

Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b):
Color::Color(p_r, p_g, p_b, 255){}

Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    set(p_r, p_g, p_b, p_a);
}

Color::~Color(){}

uint32_t Color::getRGBA(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    return  16777216*p_r+
            65536*p_g+
            256*p_b+
            p_a;
}
uint32_t Color::getRGBA(uint8_t p_r, uint8_t p_g, uint8_t p_b) { return getRGBA(p_r, p_g, p_b, 255); }
uint32_t Color::getRGBA() { return getRGBA(r, g, b, a); }

uint32_t Color::getRGB(uint8_t p_r, uint8_t p_g, uint8_t p_b)
{
    return  65536*p_r+
            256*p_g+
            p_b;
}
uint32_t Color::getRGB(uint32_t p_rgba) { return p_rgba>>8; }
uint32_t Color::getRGB() { return getRGB(r, g, b); }

uint32_t Color::getA(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a) { return p_a; }
uint32_t Color::getA(uint32_t p_rgba) { return p_rgba&0xFF; }
uint32_t Color::getA() { return a; }

/**
    Additive color blending.
    Destination color (current) is mixed with source color (parameters).
*/
void Color::add(uint8_t sr, uint8_t sg, uint8_t sb, uint8_t sa)
{
    r = sr*sa+r;
    g = sg*sa+g;
    b = sb*sa+b;
}
void Color::add(Color& c) { add(c.r, c.g, c.b, c.a); }

void Color::blend(uint8_t sr, uint8_t sg, uint8_t sb, uint8_t sa)
{
    r = sr*sa/255+r*(255-a)/255;
    g = sg*sa/255+g*(255-a)/255;
    b = sb*sa/255+b*(255-a)/255;
    a = sa+       a*(255-a)/255;
}
void Color::blend(Color& c) { blend(c.r, c.g, c.b, c.a); }

void Color::mod(uint8_t sr, uint8_t sg, uint8_t sb, uint8_t sa)
{
    r = sr*r/255;
    g = sg*g/255;
    b = sb*b/255;
    a = sa*a/255;
}
void Color::mod(Color& c) { mod(c.r, c.g, c.b, c.a); }

void Color::set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    r = p_r;
    g = p_g;
    b = p_b;
    a = p_a;
}

Color& Color::operator=(const Color& other)
{
    set(other.r, other.g, other.b, other.a);
    return *this;
}
