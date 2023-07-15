#pragma once
#include <SDL.h>
#include <string>

class ControlBinding
{
public:
    ControlBinding();
    ControlBinding(int type, int id);
    ControlBinding(int type, std::string id);
    ControlBinding(std::string id);
    virtual ~ControlBinding();

    bool operator==( ControlBinding& other );
    int getType();
    std::string toString();
    std::string toCtrlString();
    static std::string getKeyName(SDL_Keycode kc);

    void reset();
    ControlBinding& operator+=( const ControlBinding& other );
    ControlBinding& operator=( const ControlBinding& other );

    enum SpecialID {
        UNKNOWN_ACTION = -1,

        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,

        // D-Pad Buttons
        JS_DPAD_UP,
        JS_DPAD_RIGHT,
        JS_DPAD_DOWN,
        JS_DPAD_LEFT,
        // Stick Directions
        JS_STICK_A_UP,
        JS_STICK_A_RIGHT,
        JS_STICK_A_DOWN,
        JS_STICK_A_LEFT,
        JS_STICK_B_UP,
        JS_STICK_B_RIGHT,
        JS_STICK_B_DOWN,
        JS_STICK_B_LEFT,
    };

    enum Type {
        NOTHING = -1,
        KEY,
        KEYBOARD_ACTION,
        TEXT_INPUT,
        MOUSE_BUTTON,
        JOYSTICK_BUTTON,
        JOYSTICK_ACTION,

        MULTIPLE,
    };

    /** ControlBinding identifiers */
    SDL_Keycode key = -1;       //ex: SDLK_0, SDLK_LSHIFT, SDLK_g
    int keyboardAction = -1;
    std::string textInput = "";
    int mouseButton = -1;
    int joystickButton = -1;
    int joystickAction = -1;

protected:
private:
};
