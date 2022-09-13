#pragma once
#include <cstdint>

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
    uint32_t getValue();
    /**/
    void set(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    void mod(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);
    /**/
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

protected:

private:

};
