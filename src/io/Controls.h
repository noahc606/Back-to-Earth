#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include <vector>
#include "ControlBinding.h"
#include "Settings.h"

class Controls
{
public:
    /**/
    struct KeyboardInput {
        bool inputReceived = false;
        std::string inputString = "";
        bool inputSpecial = false;
    };
    /**/
    Controls();
    void init(Settings* settings);
    virtual ~Controls();
    /**/
    /* Track events and update */
    void trackEvents(SDL_Event p_e);
    void draw();
    void tick();
    /**/
    /* Mouse info */
    int getMouseX();
    int getMouseY();
    int getMouseWheel();
    /* Control pressed, held, or released */
    bool is(std::string action, std::string id);
    bool isPressed(std::string id);
    bool isHeld(std::string id);
    bool isReleased(std::string id);
    const ControlBinding& getSpecialInput();
    ControlBinding getControlBindingFromString(std::string s);
    /**/
    void reloadBindings(Settings* settings);
    /* Manually reset press, release, or the mouse wheel immediately after it happens (it will stop on its own after 100ms anyway) */
    void stop(std::string action, std::string id, std::string methodName);
    void stopPress(std::string methodName, std::string id);
    void stopRelease(std::string methodName, std::string id);
    void resetWheel(std::string methodName);
    void resetSpecialInput(std::string methodName);
    /**/
    /**/
private:
    /**/
    /**/
    /* Handle different tyes of events */
    void eventPressed(SDL_Event e);
    void eventReleased(SDL_Event e);
    void eventMouseWheel(SDL_Event e);
    void eventSpecialInput(SDL_Event e);
    /**/
    /* Store information about special input */
    ControlBinding cbSpecialInput;

    /* Mouse information */
    int mouseX = -1000;
    int mouseY = -1000;
    int wheel;
    uint64_t lastWheelEvent;
    /* Store which controls are pressed, held, or released, the last time they happened, and the keys/buttons/etc they correspond to (bindngs) */
    std::vector<bool> ctrlsPressed;
    std::vector<bool> ctrlsHeld;
    std::vector<bool> ctrlsReleased;
    std::vector<uint64_t> lastCtrlPress;
    std::vector<uint64_t> lastCtrlRelease;
    std::vector<ControlBinding> ctrlBindings;
    std::vector<std::string> cbIndices;
    /**/
};
