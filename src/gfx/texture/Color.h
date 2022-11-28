#pragma once
#include <cstdint>
#include <SDL.h>

class Color
{
public:
    /**/
    Color();
    Color(uint32_t p_value);
    Color(uint8_t p_r, uint8_t p_g, uint8_t p_b);
    Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    virtual ~Color();
    /**/
    static uint32_t getRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static uint32_t getRGBA(uint8_t r, uint8_t g, uint8_t b);
    uint32_t getRGBA();
    static uint32_t getRGB(uint8_t r, uint8_t g, uint8_t b);
    static uint32_t getRGB(uint32_t rgba);
    uint32_t getRGB();
    static uint32_t getA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static uint32_t getA(uint32_t rgba);
    uint32_t getA();

    static uint32_t getBlendedValue(SDL_BlendMode bm, uint32_t src, uint32_t dst);
    uint32_t getBlendedValue(SDL_BlendMode bm, uint32_t src);
    /**/
    void set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void mod(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void mod(Color& c);
    /**/
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

protected:

private:

};
