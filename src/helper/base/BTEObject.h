#pragma once
#include "Commands.h"
#include "Controls.h"
#include "FileHandler.h"
#include "SDLHandler.h"
#include "Settings.h"

class BTEObject
{
public:
    /**/
    BTEObject();
    virtual void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual void init(SDLHandler* sh, Controls* ctrls);
    virtual void init(SDLHandler* sh);
    virtual void init(Controls* ctrls);
    virtual ~BTEObject();
    virtual void destroy();
    /**/
    virtual void draw();
    virtual void tick();
    /**/
    int getID();
    int getType();
    bool exists();

    enum Type
    {
        null = -1,
        none = 0,

        GUIHandler = 100,
        GUI_window,
        GUI_button,
        GUI_textbox,
        GUI_tooltip,
    };

protected:
    /**/
    /**/
    void setType(int type);
    void setID(int id);
    void setTypeAndID(int p_type, int p_id);
    /**/
    /* MainLoop */
    SDLHandler* sdlHandler;
    FileHandler* fileHandler; Settings* settings;
    Controls* controls;

    /**/

private:
    int type = Type::none;
    int id = -1;
    bool existing = false;
};
