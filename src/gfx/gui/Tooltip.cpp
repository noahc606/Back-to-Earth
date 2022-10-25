#include "Tooltip.h"

Tooltip::Tooltip(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_id)
: WindowComponent::WindowComponent(p_parentWindow, p_x, p_y)
{
    setType(BTEObject::Type::GUI_tooltip);
    setID(p_id);

    text = p_text;
}
Tooltip::~Tooltip(){}

void Tooltip::init(SDLHandler* sh)
{
    BTEObject::init(sh, nullptr, nullptr);

    Text t; t.init(sh);
    t.setString(text);
    width = t.getWidth()/2;

    onWindowUpdate(false);
}

void Tooltip::destroy()
{
    BTEObject::destroy();
}

void Tooltip::draw()
{
    Text::draw(sdlHandler, text, sX, sY, 2);
}

void Tooltip::tick()
{

}

void Tooltip::onWindowUpdate(bool preventInvalidTPos)
{
    translateSPos();
}
