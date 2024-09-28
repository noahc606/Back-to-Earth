#include "ControlBinding.h"
#include <nch/cpp-utils/log.h>
#include <sstream>
#include <string>

ControlBinding::ControlBinding(){}

ControlBinding::ControlBinding(int p_type, int p_id)
{
    switch( p_type ) {
        case Type::KEY: {
            key = p_id;
        } break;
        case Type::KEYBOARD_ACTION: {
            keyboardAction = p_id;
        } break;
        case Type::TEXT_INPUT: {
            std::stringstream ss; ss << p_id;
            textInput = ss.str();
        } break;
        case Type::MOUSE_BUTTON: {
            mouseButton = p_id;
        } break;
        case Type::JOYSTICK_BUTTON: {
            joystickButton = p_id;
        } break;
        case Type::JOYSTICK_ACTION: {
            joystickAction = p_id;
        } break;
    }
}

ControlBinding::ControlBinding(int p_type, std::string p_id)
{
    switch( p_type ) {
        case Type::TEXT_INPUT: {
            std::stringstream ss; ss << p_id;
            textInput = ss.str();
        } break;
        default: {
            std::stringstream ss;
            ss << "ControlBinding type " << p_type << " ";
            ss << "and string ID \"" << p_id << "\" ";
            ss << "are not compatible!";
            nch::Log::warn(__PRETTY_FUNCTION__, ss.str(), "setting every ControlBinding property to defaults");

            reset();
        } break;
    }
}

ControlBinding::ControlBinding(std::string id)
{
    //Try to parse 'id' as a number. If it is, assume that it is referring to an SDL_Keycode
    try {
        int kc = std::stoi(id);
        keyboardAction = kc;
    // If we could not parse 'id' as a number, it must be something else
    } catch (...) {
        if( id=="MOUSE_LEFT" ) {
            mouseButton = MOUSE_LEFT;
        }
        if( id=="MOUSE_MIDDLE" ) {
            mouseButton = MOUSE_MIDDLE;
        }
        if( id=="MOUSE_RIGHT" ) {
            mouseButton = MOUSE_RIGHT;
        }
    }
}

ControlBinding::~ControlBinding(){}

bool ControlBinding::operator==(ControlBinding& other)
{
    return (
        key            ==other.key            &&
        keyboardAction ==other.keyboardAction &&
        textInput      ==other.textInput      &&
        mouseButton    ==other.mouseButton    &&
        joystickButton ==other.joystickButton &&
        joystickAction ==other.joystickAction
    );
}

int ControlBinding::getType()
{
    int numSet = 0;
    int setID = -1;

    if( key           !=-1 ) { numSet++; setID = Type::KEY;             }
    if( keyboardAction!=-1 ) { numSet++; setID = Type::KEYBOARD_ACTION; }
    if( textInput     !="" ) { numSet++; setID = Type::TEXT_INPUT;      }
    if( mouseButton   !=-1 ) { numSet++; setID = Type::MOUSE_BUTTON;    }
    if( joystickButton!=-1 ) { numSet++; setID = Type::JOYSTICK_BUTTON; }
    if( joystickAction!=-1 ) { numSet++; setID = Type::JOYSTICK_ACTION; }

    if( numSet==0 ) {
        return Type::NOTHING;
    } else
    if( numSet==1 ) {
        return setID;
    }
    return Type::MULTIPLE;
}

std::string ControlBinding::toString()
{
    std::stringstream ss;

    switch( getType() ) {
        case KEY:               { ss << "Key: " << key;                         } break;
        case KEYBOARD_ACTION:   { ss << "Keyboard Action: " << keyboardAction;  } break;
        case TEXT_INPUT:        { ss << "Text Input: " << textInput;            } break;
        case MOUSE_BUTTON:      { ss << "Mouse Button: " << mouseButton;        } break;
        case JOYSTICK_BUTTON:   { ss << "Joystick Button: " << joystickButton;  } break;
        case JOYSTICK_ACTION:   { ss << "Joystick Action: " << joystickAction;  } break;
        default:                { ss << "Unset Type";                           } break;
    }

    return ss.str();
}

std::string ControlBinding::toCtrlString()
{
    if( keyboardAction!=-1 ) {
        return ControlBinding::getKeyName(keyboardAction);
    }

    return "Unknown Input Type";
}

std::string ControlBinding::getKeyName(SDL_Keycode kc)
{
    std::stringstream ss;
    ss << SDL_GetKeyName(kc);
    return ss.str();
}

void ControlBinding::reset()
{
    key = -1;
    keyboardAction = -1;
    textInput = "";
    mouseButton = -1;
    joystickButton = -1;
    joystickAction = -1;
}

ControlBinding& ControlBinding::operator+=(const ControlBinding& other)
{
    if( key           ==-1 ) key = other.key;
    if( keyboardAction==-1 ) keyboardAction = other.keyboardAction;
    if( textInput     =="" ) textInput = other.textInput;
    if( mouseButton   ==-1 ) mouseButton = other.mouseButton;
    if( joystickButton==-1 ) joystickButton = other.mouseButton;
    if( joystickAction==-1 ) joystickAction = other.mouseButton;

    return *this;
}

ControlBinding& ControlBinding::operator=(const ControlBinding& other)
{
    key = other.key;
    keyboardAction = other.keyboardAction;
    textInput = other.textInput;
    mouseButton = other.mouseButton;
    joystickButton = other.joystickButton;
    joystickAction = other.joystickAction;

    return *this;
}
