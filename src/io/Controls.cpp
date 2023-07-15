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
    reloadBindings(settings);
}
Controls::~Controls(){}

/**/

void Controls::trackEvents(SDL_Event p_e)
{
    // Process keyboard, mouse buttons, and mousewheel input
    switch(p_e.type)
    {
        case SDL_KEYDOWN:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_JOYBUTTONDOWN:
        {
            if(p_e.key.repeat == 0) {
                eventPressed(p_e);
            }
        } break;

        case SDL_KEYUP:
        case SDL_MOUSEBUTTONUP:
        case SDL_JOYBUTTONUP:
        {
            eventReleased(p_e);
        } break;

        case SDL_MOUSEWHEEL: {
            eventMouseWheel(p_e);
        } break;
    }

    // Process special input:
    // Text input:          regular typing, a phone popup keyboard, etc.
    // Keyboard actions:    Arrow keys, backspace/delete, home/end, return, etc.
    // Controller input:    Anything done on joystick/controller: dpads, direction sticks, buttons, etc.
    eventSpecialInput(p_e);

}

void Controls::draw()
{
    SDL_GetMouseState(&mouseX, &mouseY);
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

const ControlBinding& Controls::getSpecialInput() { return cbSpecialInput; }

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

void Controls::reloadBindings(Settings* settings)
{
    //Clear 'cbIndices' and 'ctrlBindings'
    cbIndices.clear();
    ctrlBindings.clear();
    //Add currently configured control bindings to 'cbIndices' and 'ctrlBindings'
    for(Settings::t_kvPair kvp : settings->getKvMap(Settings::controls)) {
        cbIndices.push_back(kvp.first);
        ctrlBindings.push_back(getControlBindingFromString(kvp.second));
    }

    //Clear all control states.
    ctrlsPressed.clear();
    ctrlsHeld.clear();
    ctrlsReleased.clear();
    lastCtrlPress.clear();
    lastCtrlRelease.clear();
    //Set all control states to false.
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        ctrlsPressed.push_back(false);
        ctrlsHeld.push_back(false);
        ctrlsReleased.push_back(false);

        lastCtrlPress.push_back(false);
        lastCtrlRelease.push_back(false);
    }
}

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

void Controls::resetSpecialInput(std::string methodName)
{
    if( cbSpecialInput.getType()==ControlBinding::NOTHING ) {
        std::stringstream ss;
        ss << "Tried to reset special input that is already reset";
        Log::warn(methodName, ss.str());
    }

    cbSpecialInput.reset();
}

/**/

void Controls::eventPressed(SDL_Event e)
{
    //Get info from event
    SDL_Keycode key = e.key.keysym.sym;
    uint8_t mouseButton = e.button.button-1;
    uint8_t joystickButton = e.jbutton.button;

    //Set appropriate element in ctrlsPressed to true.
    //Allows for outside access to whether a key is currently held down.
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if(
            (ctrlBindings[i].getType()==ControlBinding::KEY              && key           ==ctrlBindings[i].key           ) ||
            (ctrlBindings[i].getType()==ControlBinding::MOUSE_BUTTON     && mouseButton   ==ctrlBindings[i].mouseButton   ) ||
            (ctrlBindings[i].getType()==ControlBinding::JOYSTICK_BUTTON  && joystickButton==ctrlBindings[i].joystickButton)
        ) {
            ctrlsPressed[i] = true;
            ctrlsHeld[i] = true;

            lastCtrlPress[i] = SDL_GetTicks();
        }
    }
}

void Controls::eventReleased(SDL_Event e)
{
    //Get info from event
    SDL_Keycode key = e.key.keysym.sym;
    uint8_t mouseButton = e.button.button-1;
    uint8_t joystickButton = e.jbutton.button;

    //Set appropriate elements in the arrays to indicate that that control was released.
    //Allows for outside access to whether a key was released
    for(int i = 0; i<(int)cbIndices.size(); i++) {
        if(
            (ctrlBindings[i].getType()==ControlBinding::KEY              && key           ==ctrlBindings[i].key           ) ||
            (ctrlBindings[i].getType()==ControlBinding::MOUSE_BUTTON     && mouseButton   ==ctrlBindings[i].mouseButton   ) ||
            (ctrlBindings[i].getType()==ControlBinding::JOYSTICK_BUTTON  && joystickButton==ctrlBindings[i].joystickButton)
        ) {
            ctrlsPressed[i] = false;
            ctrlsHeld[i] = false;
            ctrlsReleased[i] = true;

            lastCtrlRelease[i] = SDL_GetTicks();
        }
    }
}

void Controls::eventMouseWheel(SDL_Event e)
{
    lastWheelEvent = SDL_GetTicks();
    wheel += e.wheel.y;
}

void Controls::eventSpecialInput(SDL_Event e)
{
    ControlBinding cb;

    /** Inputting: general text */
    if( e.type==SDL_TEXTINPUT ) {
        /* Basic keyboard input */
        //Any keystroke that can be represented as a string (numbers, symbols, letters);
        std::stringstream ss;
        ss << e.text.text;

        cbSpecialInput.textInput = ss.str();
    }

    /** Inputting: any keyboard action */
    if( e.type==SDL_KEYDOWN ) {
        //Get keyboard state
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        SDL_Keycode kc = e.key.keysym.sym;

        /* Keys on keyboard */
        cbSpecialInput.keyboardAction = kc;

        /* Special keyboard input */
        //If holding ctrl + v
        if( state[SDL_SCANCODE_V] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) ) {
            //Paste clipboard text into kbInfo.inputString
            std::stringstream ss;
            ss << SDL_GetClipboardText();
            cbSpecialInput.textInput = ss.str();
        }
    }

    /** Inputting: Joysticks/Controllers, or anything else */
    //Buttons with an on/off state
    if( e.type==SDL_JOYBUTTONDOWN ) {
        cbSpecialInput.joystickButton = e.jbutton.button;
    }
    //Joystick
    ControlBinding cbNew;
    cbNew.joystickAction = cbNew.UNKNOWN_ACTION;
    if( e.type==SDL_JOYAXISMOTION ) {
        // Axis ID 0 (Stick A, left/right)
        if( e.jaxis.axis==0 ) {
            if( e.jaxis.value<-8000 ) { cbNew.joystickAction = cbNew.JS_STICK_A_LEFT; }
            if( e.jaxis.value>8000  ) { cbNew.joystickAction = cbNew.JS_STICK_A_RIGHT; }
        } else
        // Axis ID 1 (Stick A, up/down)
        if( e.jaxis.axis==1 ) {
            if( e.jaxis.value<-8000 ) { cbNew.joystickAction = cbNew.JS_STICK_A_DOWN; }
            if( e.jaxis.value>8000  ) { cbNew.joystickAction = cbNew.JS_STICK_A_UP; }
        }

        if(e.jaxis.axis==2 || e.jaxis.axis==3) { cbNew.joystickAction += 4; }
    }
}
