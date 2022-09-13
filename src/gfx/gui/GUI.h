#pragma once
#include "BTEObject.h"


class GUI : public BTEObject
{
public:
    /**/
    GUI();
    virtual void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual void init(SDLHandler* sh, Controls* ctrls);
    virtual void init(SDLHandler* sh);
    virtual ~GUI();
    virtual void destroy();
    /**/
    virtual void draw();
    virtual void tick();
    /**/
    bool isUserGUI();

protected:

private:
    void setUserGUI();
    bool userGUI = true;
};
