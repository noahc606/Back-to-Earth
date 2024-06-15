#include "Tooltip.h"

Tooltip::Tooltip(Window* p_parentWindow, int p_x, int p_y, std::string p_text, bool special, int p_id)
: WindowComponent::WindowComponent(p_parentWindow, p_x, p_y)
{
    setType(BTEObject::Type::GUI_tooltip);
    setID(p_id);

    Tooltip::special = special;
    text = p_text;
}
Tooltip::Tooltip(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_id)
: Tooltip::Tooltip(p_parentWindow, p_x, p_y, p_text, false, p_id){}

Tooltip::~Tooltip(){}

void Tooltip::init(SDLHandler* sh)
{
    BTEObject::init(sh, nullptr, nullptr);

    TextOld t; t.init(sh);
    t.setString(text);

    width = t.getWidth()/2;
    if(special) width *= 2;

    onWindowUpdate();
}

void Tooltip::destroy()
{
    BTEObject::destroy();
}

void Tooltip::draw()
{
    if(special) {
        double specialFlicker = ((double)SDL_GetTicks()*0.0002);
        if(specialFlicker>1) specialFlicker = 1;

        double sf = specialFlicker;
        double ed = 40*((double)(rand()%10)/10.0); // ED = Extra Darkness
        
        int r = 100*sf-ed;  if(r<0) r = 0; if(r>255) r = 255;
        int g = 100*sf-ed;  if(g<0) g = 0; if(g>255) g = 255;
        int b = 225*sf-ed;  if(b<0) b = 0; if(b>255) b = 255;
        
        TextOld::draw(sdlHandler, text, sX, sY, 4, Color(r, g, b), Color(0, 0, 0, 255), TextureLoader::GUI_FONT_robot);
    } else {
        TextOld::draw(sdlHandler, text, sX, sY, 2);
    }
}

void Tooltip::tick()
{
}

void Tooltip::onWindowUpdate()
{
    translateSPos();
}
