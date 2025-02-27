#pragma once
#include <nch/cpp-utils/color.h>
#include <map>
#include <string>
#include "Texture.h"
#include "GUI.h"

class WindowData
{
public:
    /**/
    WindowData(int w, int h);
    WindowData(int w, int h, std::string upperPanel, std::string lowerPanel);
    virtual ~WindowData();
    /**/
    std::string getLowerPanel();
    std::string getUpperPanel();
    int getHeight();
    int getH64();
    int getWidth();
    int getW64();
    char getPanelData(int line, int x);
    nch::Color getPanelColor(char ch);
    nch::Color getPanelColor(int line, int x);
    nch::Color getBorderColor();
    int getSpecialType();
    GUI* getRelatedUI();
    std::string toString();
    /**/
    void setPanelData(int line, std::string data);
    void setPanelColor(char ch, const nch::Color& col);
    void setBorderColor(const nch::Color& col);
    void setSpecialType(int st, GUI* relatedUI);
    void setSpecialType(int st);
    void buildTex(Texture* windowTex);

    enum WindowSpecialType {
        NONE = 0,
        BACKGROUND = 1,
        COLOR_SELECTOR = 2,
    };

protected:

private:

    std::string lowerPanel = "";
    std::string upperPanel = "";
    int width = 0;
    int height = 0;
    int specialType = -1;
    GUI* relatedUI = nullptr;

    std::map<int, std::string> panelData;
    nch::Color borderColor;
    std::map<char, nch::Color> colors;
};
