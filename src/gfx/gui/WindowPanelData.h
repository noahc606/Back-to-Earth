#pragma once
#include <map>
#include <string>
#include "Color.h"
#include "Texture.h"

class WindowPanelData
{
public:
    WindowPanelData(int w, int h);
    WindowPanelData(int w, int h, std::string upperPanel, std::string lowerPanel);
    virtual ~WindowPanelData();

    std::string getLowerPanel();
    std::string getUpperPanel();
    int getHeight();
    int getWidth();
    char getPanelData(int line, int x);

    std::string toString();
    void setPanelData(int line, std::string data);
    void setPanelColor(char ch, Color& col);
    void buildTex(Texture* windowTex);

protected:

private:

    std::string lowerPanel = "";
    std::string upperPanel = "";
    int width = 0;
    int height = 0;

    std::map<int, std::string> panelData;

    std::map<char, Color> colors;
};
