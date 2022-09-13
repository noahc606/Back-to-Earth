#pragma once
#include <SDL.h>
#include <string>

class ControlBinding
{
public:
    ControlBinding();
    ControlBinding(int type, int id);
    ControlBinding(std::string id);
    virtual ~ControlBinding();

    enum MiscButton {
        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,
    };

    enum Type {
        NOTHING,
        KEY,
        MISC_BUTTON,
    };
    int type;

    SDL_Keycode key;
    int miscButton;

protected:
private:
};
