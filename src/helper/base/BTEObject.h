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
    virtual void init(SDLHandler* sh, FileHandler* fh);
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

        BTE_GUIHandler = 100,
            GUI_window,
            GUI_button, GUI_radiobutton, GUI_textbox, GUI_checkbox, GUI_slider, GUI_colorselect, GUI_saveselect,
            GUI_tooltip, GUI_icon, GUI_progressbar,

        BTE_TileMap = 200,
    };

protected:
    /**/
    /**/
    void setType(int type);
    void setSubType(int subtype);
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
