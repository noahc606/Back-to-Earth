#pragma once
#include "TextOld.h"
#include "Texture.h"
#include "WindowComponent.h"

class Button : public WindowComponent
{
public:
    /**/
    Button();
    Button(Window* p_parentWindow, int p_x, int p_y, int p_width, std::string p_s, int p_id);
    Button(int x, int y, int width, std::string s, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    void destroy();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    /**/
    bool isHovering();
    bool isSelected();
    std::string getString();
    /**/
    void setString(std::string s);
    void unclick();
    void deselect();
    
protected:
    /* Button textures */
    Texture texBtn;                 //When mouse not hovering
    Texture texBtnHovering;         //When mouse is hovering
    Texture texTbxSelected;         //When textbox is selected
    Texture texBtnSelected;         //When button is selected

    int shineAnimation = -1;
    Texture texBtnShine0;           //Shine when mouse is hovering
    Texture texBtnShine1;           //Shine when mouse is hovering

    /* Button draw properties */
    bool hovering = false;
    bool clicked = false;
    bool selected = false;

    TextOld btnText;
    std::string btnString;

private:
};
