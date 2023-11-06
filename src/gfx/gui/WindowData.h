#pragma once
#include <map>
#include <string>
#include "Color.h"
#include "Texture.h"

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
    int getWidth();
    char getPanelData(int line, int x);
    Color getPanelColor(char ch);
    Color getPanelColor(int line, int x);
    Color getBorderColor();
    std::string toString();
    /**/
    void setPanelData(int line, std::string data);
    void setPanelColor(char ch, const Color& col);
    void setBorderColor(const Color& col);
    void buildTex(Texture* windowTex);

protected:

private:

    std::string lowerPanel = "";
    std::string upperPanel = "";
    int width = 0;
    int height = 0;

    std::map<int, std::string> panelData;
    Color borderColor;
    std::map<char, Color> colors;
};
