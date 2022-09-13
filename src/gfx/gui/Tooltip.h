#pragma once
#include "WindowComponent.h"

class Tooltip : public WindowComponent
{
public:
    /**/
    Tooltip(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_id);
    virtual ~Tooltip();
    void init(SDLHandler* sh);
    void destroy();
    /**/
    void draw();
    void tick();
protected:

private:
    std::string text;
};
