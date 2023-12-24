#pragma once
#include <cstdint>
#include <tuple>
#include <SDL2/SDL.h>
#include "Loggable.h"

class Color : public Loggable
{
public:
    /**/
    Color();
    Color(uint32_t p_value);
    Color(uint8_t p_r, uint8_t p_g, uint8_t p_b);
    Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    virtual ~Color();
    /**/
	void putInfo(std::stringstream& ss, int& tabs);
    static uint32_t getRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static uint32_t getRGBA(uint8_t r, uint8_t g, uint8_t b);
    uint32_t getRGBA();
    static uint32_t getRGB(uint8_t r, uint8_t g, uint8_t b);
    static uint32_t getRGB(uint32_t rgba);
    uint32_t getRGB();
    static uint32_t getA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static uint32_t getA(uint32_t rgba);
    uint32_t getA();
	std::tuple<double, double, double> toHSV();
	/**/
	
	/* Color setting & combining */
	//Linear interpolation between two colors
	Color getInterpolColor(uint8_t p_r, uint8_t p_g, uint8_t p_b, double weight);
	Color getInterpolColor(Color& c, double weight);
	//Additional blending: formulas found at https://wiki.libsdl.org/SDL2/SDL_BlendMode
	void add(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void add(Color& c);
    void blend(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void blend(Color& c);
    void mod(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void mod(Color& c);
    void set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    Color& operator=( const Color& other );
	
    /**/
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

protected:

private:

};
