#include "Controls.h"
#include <iostream>
#include <sstream>
#include <string>
#include "FileHandler.h"
#include "Log.h"
#include "Main.h"
#include "MainLoop.h"

/**/

Controls::Controls(){}
void Controls::init(Settings* settings)
{
    //Store configured control bindings
    for(Settings::t_kvPair kvp : settings->getKvMap(Settings::controls)) {
        cbIndices.push_back(kvp.first);
        ctrlBindings.push_back(getControlBindingFromString(kvp.second));
    }

    //Set all control states to false.
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        ctrlsPressed.push_back(false);
        ctrlsHeld.push_back(false);
        ctrlsReleased.push_back(false);

        lastCtrlPress.push_back(false);
        lastCtrlRelease.push_back(false);
    }
}
Controls::~Controls(){}

/**/

void Controls::trackEvents(SDL_Event p_e)
{
    switch(p_e.type)
    {
        case SDL_KEYDOWN: case SDL_MOUSEBUTTONDOWN: {
            if(p_e.key.repeat == 0) {
                eventPressed(p_e);
            }
        } break;

        case SDL_KEYUP: case SDL_MOUSEBUTTONUP: {
            eventReleased(p_e);
        } break;

        case SDL_MOUSEWHEEL: {
            eventMouseWheel(p_e);
        } break;
    }

    switch(p_e.type)
    {
        case SDL_KEYDOWN: case SDL_TEXTINPUT: {
            eventTextInput(p_e);
        } break;
    }
}

void Controls::tick()
{
    SDL_GetMouseState(&mouseX, &mouseY);

    int ms = SDL_GetTicks();
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if( ms-lastCtrlPress[i]>100 ) {
            ctrlsPressed[i] = false;
        }

        if( ms-lastCtrlRelease[i]>100 ) {
            ctrlsReleased[i] = false;
        }
    }

    if( ms-lastWheelEvent>100) {
        wheel = 0;
    }
}

/**/

int Controls::getMouseX() { return mouseX; }
int Controls::getMouseY() { return mouseY; }
int Controls::getMouseWheel() { return wheel; }

bool Controls::is(std::string action, std::string id)
{
    bool foundAction = true;
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if( id==cbIndices[i] ) {
            if(action=="held") return ctrlsHeld[i];
            if(action=="pressed") return ctrlsPressed[i];
            if(action=="released") return ctrlsReleased[i];

            foundAction = false;
        }
    }

    std::stringstream ss;
    if(!foundAction) {
        ss << "Action \"" << action << "\" doesn't exist";
    } else {
        ss << "Control binding '" << id << "' doesn't exist";
    }
    Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning false");
    return false;
}

bool Controls::isPressed(std::string id) { return is("pressed", id); }
bool Controls::isHeld(std::string id) { return is("held", id); }
bool Controls::isReleased(std::string id) { return is("released", id); }

const Controls::KeyboardInput& Controls::getKeyboardInput() { return kbInfo; }

ControlBinding Controls::getControlBindingFromString(std::string s)
{
    //Check if string is an integer.
    bool isInt = true;
    for(char c : s) {
        if( !( c>='0' && c<='9' ) ) {
            isInt = false;
            break;
        }
    }

    if(isInt) {
        return ControlBinding( ControlBinding::KEY, std::stoi(s) );
    } else {
        return ControlBinding( s );
    }
}

/**/

void Controls::stop(std::string action, std::string id, std::string methodName)
{
    bool foundAction = false;
    bool value = false;
    for( int i = 0; i<(int)cbIndices.size(); i++ ) {
        if( id==cbIndices[i] ) {
            if(action=="press") {
                value = ctrlsPressed[i];
                ctrlsPressed[i] = false;
                foundAction = true;
            }

            if(action=="release") {
                value = ctrlsReleased[i];
                ctrlsReleased[i] = false;
                foundAction = true;
            }
        }
    }

    if( !foundAction ) {
        std::stringstream ss;
        ss << "Action \"" << action << "\" doesn't exist";
        Log::warn( __PRETTY_FUNCTION__, ss.str() );
        return;
    }

    if(!value) {
        std::stringstream ss;
        ss << "Tried to stop " << action << "ing keybind ID '" << id << "' that is not " << action << "ed";
        Log::warn( __PRETTY_FUNCTION__, ss.str() );
    }
}

void Controls::stopPress(std::string id, std::string methodName) { stop("press", id, methodName ); }
void Controls::stopRelease(std::string id, std::string methodName) { stop("release", id, methodName ); }

void Controls::resetWheel(std::string methodName)
{
    if( wheel==0 ) {
        std::stringstream ss;
        ss << "Tried to reset mouse wheel that already = 0";
        Log::warn(methodName, ss.str());
    }

    wheel = 0;
}

void Controls::resetKBInput(std::string methodName)
{
    if( !kbInfo.inputReceived ) {
        std::stringstream ss;
        ss << "Tried to reset keyboard input that hasn't even been received yet";
        Log::warn(methodName, ss.str());
    }

    /* Reset kbInput data */
    kbInfo.inputReceived = false;
    kbInfo.inputString = "";
    kbInfo.inputSpecial = false;
}

/**/

void Controls::eventPressed(SDL_Event e)
{
    //Get SDL_Keycode or other button from event
    SDL_Keycode kc = e.key.keysym.sym;
    uint8_t btn = e.button.button-1;

    //Set appropriate element in ctrlsPressed to true.
    //Allows for outside access to whether a key is currently held down.
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if( ctrlBindings[i].type==ControlBinding::KEY )
        {
            if( kc==ctrlBindings[i].key ) {
                ctrlsPressed[i] = true;
                ctrlsHeld[i] = true;

                lastCtrlPress[i] = SDL_GetTicks();
            }
        } else
        if( ctrlBindings[i].type==ControlBinding::MISC_BUTTON )
        {
            if( btn==ctrlBindings[i].miscButton ) {
                ctrlsPressed[i] = true;
                ctrlsHeld[i] = true;

                lastCtrlPress[i] = SDL_GetTicks();
            }
        }
    }
}

void Controls::eventReleased(SDL_Event e)
{
    //Get SDL_Keycode or other button from event
    SDL_Keycode kc = e.key.keysym.sym;
    uint8_t btn = e.button.button-1;

    //Set appropriate element in ctrlsPressed to true.
    //Allows for outside access to whether a key is currently held down.
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if( ctrlBindings[i].type==ControlBinding::KEY )
        {
            if( kc==ctrlBindings[i].key ) {
                ctrlsPressed[i] = false;
                ctrlsHeld[i] = false;
                ctrlsReleased[i] = true;

                lastCtrlRelease[i] = SDL_GetTicks();
            }
        } else
        if( ctrlBindings[i].type==ControlBinding::MISC_BUTTON )
        {
            if( btn==ctrlBindings[i].miscButton ) {
                ctrlsPressed[i] = false;
                ctrlsHeld[i] = false;
                ctrlsReleased[i] = true;

                lastCtrlRelease[i] = SDL_GetTicks();
            }
        }
    }
}

void Controls::eventMouseWheel(SDL_Event e)
{
    lastWheelEvent = SDL_GetTicks();
    wheel += e.wheel.y;
}

void Controls::eventTextInput(SDL_Event e)
{
    if( e.type==SDL_TEXTINPUT ) {
        /* Basic keyboard input */
        //Any keystroke that can be represented as a string (numbers, symbols, letters);
        std::stringstream ss;
        ss << e.text.text;

        kbInfo.inputReceived = true;
        kbInfo.inputString = ss.str();
        kbInfo.inputSpecial = false;

    } else
    if( e.type==SDL_KEYDOWN ) {
        //Get keyboard state
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        SDL_Keycode kc = e.key.keysym.sym;

        //Special keys (backspace, arrow keys, etc)
        switch(kc)
        {
            case SDLK_ESCAPE:
            case SDLK_BACKSPACE: case SDLK_DELETE:
            case SDLK_LEFT: case SDLK_RIGHT: case SDLK_UP: case SDLK_DOWN:
            case SDLK_HOME: case SDLK_END: {
                std::stringstream ss;
                ss << kc;
                kbInfo.inputReceived = true;
                kbInfo.inputString = ss.str();
                kbInfo.inputSpecial = true;
            } break;

            //Input newline
            case SDLK_RETURN: {
                kbInfo.inputReceived = true;
                kbInfo.inputString = "\n";
                kbInfo.inputSpecial = false;
            } break;
        }

        /* Special keyboard input */
        //If holding ctrl + v
        if( state[SDL_SCANCODE_V] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) ) {
            //Paste clipboard text into kbInfo.inputString
            std::stringstream ss;
            ss << SDL_GetClipboardText();
            kbInfo.inputReceived = true;
            kbInfo.inputString = ss.str();
            kbInfo.inputSpecial = false;
        }
    }
}
