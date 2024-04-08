#include "Color.h"
#include <algorithm>
#include <cmath>
#include "DebugScreen.h"
#include "Log.h"

Color::Color():
Color::Color(255, 255, 255){}

Color::Color(uint32_t p_value)
{
	set(p_value);
}

Color::Color(std::string p_value)
{
	try {
		uint32_t num = std::stoi(p_value);
		set(num);
	} catch(...) {
		Log::error(__PRETTY_FUNCTION__, "Failed to parse string as a color", "setting RGBA=(0,0,0,0)");
	}
}


Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b):
Color::Color(p_r, p_g, p_b, 255){}

Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    set(p_r, p_g, p_b, p_a);
}

Color::~Color(){}

void Color::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "Color(rgba)=(" << (int)r << ", " << (int)g << ", " << (int)b << ", " << (int)a << ")";
	DebugScreen::newLine(ss);
}

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
 * H = [0-360)
 * S = [0-100]
 * V = [0-100]
 */
std::tuple<double, double, double> Color::toHSV()
{
	double rp = ((double)r)/255.0;
	double gp = ((double)g)/255.0;
	double bp = ((double)b)/255.0;
	double cmax = std::max(rp, std::max(gp, bp));
	double delta = cmax-std::min(rp, std::min(gp, bp));
	
	//Hue
	double h;
	if(cmax==rp) {
		h = 60.0*((gp-bp)/delta+0.0);
	} else if(cmax==gp) {
		h = 60.0*((bp-rp)/delta+2.0);
	} else {
		h = 60.0*((rp-gp)/delta+4.0);
	}
	if( h<0.0 ) {
		h += 360.0;
	}
	
	//Saturation
	double s;
	if(cmax==0) {
		s = 0;
		//H is Nan when max is zero
	} else {
		s = 100.0*delta/cmax;
	}
	
	//Value
	double v = 100.0*cmax;
	
	return std::make_tuple(h, s, v);
}

/**
 * Return the 32bit RGBA value, interpreted as a base 10 number, as a string.
 */
std::string Color::toString()
{
	std::stringstream ss; ss << getRGBA();
	return ss.str();
}

/*
 * Return a new color that is the weighted average between two colors. 'weight' is a value from 0.0 to 1.0.
 * 
 * A weight close to 0.0 would return a color "closer to" this object.
 * A weight close to 1.0 would return a color "closer to" the specified color (within the parameters).
 */ 
Color Color::getInterpolColor(uint8_t p_r, uint8_t p_g, uint8_t p_b, double weight)
{
	uint8_t r1 = r; uint8_t g1 = g; uint8_t b1 = b;
	uint8_t r2 = p_r; uint8_t g2 = p_g; uint8_t b2 = p_b;
	
	double dR = ((double)(r1-r2))*weight;
	double dG = ((double)(g1-g2))*weight;
	double dB = ((double)(b1-b2))*weight;
	
	Color res;
	
	return Color(r1-dR, g1-dG, b1-dB);
}
Color Color::getInterpolColor(Color& c, double weight)
{
	return getInterpolColor(c.r, c.g, c.b, weight);
}

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

void Color::set(uint32_t p_rgba)
{
    r = (p_rgba>>24)&0xFF;
    g = (p_rgba>>16)&0xFF;
    b = (p_rgba>> 8)&0xFF;
    a = (p_rgba>> 0)&0xFF;
}

void Color::set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
{
    r = p_r;
    g = p_g;
    b = p_b;
    a = p_a;
}

/**
	Take in a uint32_t number represented as a string, and set the rgba to that number.
*/
void Color::setFromDecimalStr(std::string decimal)
{
	uint32_t rgba = std::stoul(decimal);
	set(rgba);
}

/**
   Given 3 values H:[0,360); S:[0,100]; V:[0:100]: Set this color from that HSV triple.
   Opacity is set to 255 as HSV has no transparency.
*/
void Color::setFromHSV(double h, double s, double v)
{
	
    if(h>360 || h<0) { Log::warn(__PRETTY_FUNCTION__, "Hue should be within [0, 360] (currently %f)", h); }
    if(s>100 || s<0) { Log::warn(__PRETTY_FUNCTION__, "Saturation should be within [0, 100] (currently %f)", s); }
    if(v>100 || v<0) { Log::warn(__PRETTY_FUNCTION__, "Value should be within [0, 100] (currently %f)", v); }

	if(h==360) h = 0;

    s = s/100.0;
    v = v/100.0;
    float c = s*v;
    float x = c*(1-std::abs(std::fmod(h/60.0, 2)-1));
    float m = v-c;
    float sr, sg, sb;

    if(h>=0 && h<60)            { sr = c; sg = x; sb = 0; }
    else if(h>=60 && h<120)     { sr = x; sg = c; sb = 0; }
    else if(h>=120 && h<180)    { sr = 0; sg = c; sb = x; }
    else if(h>=180 && h<240)    { sr = 0; sg = x; sb = c; }
    else if(h>=240 && h<300)    { sr = x; sg = 0; sb = c; }
    else                        { sr = c; sg = 0; sb = x; }

    uint8_t r = std::round((sr+m)*255.0);
    uint8_t g = std::round((sg+m)*255.0);
    uint8_t b = std::round((sb+m)*255.0);

    set(r, g, b, 255);
}

Color& Color::operator=(const Color& other)
{
    set(other.r, other.g, other.b, other.a);
    return *this;
}