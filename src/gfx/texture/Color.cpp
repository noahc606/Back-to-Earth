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
    Color blending formulas found at
        https://wiki.libsdl.org/SDL2/SDL_BlendMode
*/
uint32_t Color::getBlendedValue(SDL_BlendMode bm, uint32_t src, uint32_t dst )
{
    switch(bm) {
        case SDL_BLENDMODE_NONE: {
            return src;
        } break;

        case SDL_BLENDMODE_BLEND: {
            uint32_t srcA = getA(src);
            uint32_t dstA = getA(dst);

            uint32_t resRGB = getRGB(src)*srcA/0xFF+getRGB(dst)*(0xFF-srcA)/0xFF;
            uint32_t resA = srcA+dstA*(0xFF-srcA)/0xFF;

            return 256*resRGB+resA;
        } break;

        case SDL_BLENDMODE_ADD: {
            uint32_t resRGB = getRGB(src)*getA(src)/0xFF+getRGB(dst);

            return 256*resRGB+getA(dst);
        } break;

        case SDL_BLENDMODE_MOD: {
            uint32_t resRGB = getRGB(src)*getRGB(dst)/0xFF;

            return 256*resRGB+getA(dst);
        } break;

        default: {
            return src;
        }
    }
}
uint32_t Color::getBlendedValue(SDL_BlendMode bm, uint32_t src) { return getBlendedValue(bm, src, getRGBA()); }

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

void Color::mod(Color& c) { mod(c.r, c.g, c.b, c.a); }
