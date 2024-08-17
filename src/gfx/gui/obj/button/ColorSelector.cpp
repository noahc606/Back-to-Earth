#include "ColorSelector.h"
#include "TextureBuilder.h"
#include "Log.h"

ColorSelector::ColorSelector(Window* parentWindow, int x, int y, NCH_Color col, int id)
: Button::Button(parentWindow, x, y, 0, "", id)
{
	setSubType(BTEObject::Type::GUI_colorselect);

    color = col;
    auto hsv = col.getHSV();
    hue = hsv[0];
    sat = hsv[1];
    val = hsv[2];

	btnInitString = "";
    width = 32;
    height = 32;
}

ColorSelector::ColorSelector(Window* parentWindow, int x, int y, int id):
ColorSelector(parentWindow, x, y, NCH_Color(), id){}

void ColorSelector::init(SDLHandler* sh, Controls* ctrls)
{
	GUI::init(sh, ctrls);

	texBtn.init(sdlHandler);
    texSelectorEdges.init(sdlHandler);
    buildTexes();

	onWindowUpdate();

    texCrosshair.init(sdlHandler, 9, 9, 2);
}

void ColorSelector::destroy()
{
    Button::destroy();
    texCrosshair.destroy();
    texSelectorEdges.destroy();
}

void ColorSelector::buildTexes()
{
    // Colorselector window opener (looks like a + sign)
    texBtn.clear();
    texBtn.setTexDimensions(16, 16);
    texBtn.setDrawScale(2);
    texBtn.lock();
    texBtn.blit(TextureLoader::GUI_button, 0, 112);
    texBtn.blit(TextureLoader::GUI_button, 51, 129);
    texBtn.blit(TextureLoader::GUI_button, 0, 129);

    TextureBuilder tb(sdlHandler);

    texSelectorEdges.setDrawScale(2);
    WindowData wd(4, 4);
    wd.setPanelData(0, "aaaa");
    wd.setPanelData(1, "aaaa");
    wd.setPanelData(2, "aaaa");
    wd.setPanelData(3, "aaaa");
    wd.setPanelColor('a', NCH_Color(0, 0, 0, 0));
    tb.buildWindow(texSelectorEdges, &wd, 128, 128);
}

void ColorSelector::setSelectorWindow(Window* sw) { selectorWindow = sw; }

void ColorSelector::onWindowUpdate()
{
	Button::onWindowUpdate();

    texBtn.setDrawPos(sX, sY);
}

void ColorSelector::draw()
{
    //Draw ColorSelector Button
    if( drawState.first!=drawStateLast.first || drawState.second!=drawStateLast.second ) {
        texBtn.lock();
        
        if(!selected) {
            if(!hovering) {
                texBtn.blit(TextureLoader::GUI_button, 0, 112);     //Base button
                texBtn.blit(TextureLoader::GUI_button, 51, 129);    //Multicolored square
                texBtn.blit(TextureLoader::GUI_button, 0, 129);     //Add black crosshair (nothing)
            } else {
                texBtn.blit(TextureLoader::GUI_button, 17, 112);    //Hovering button
                texBtn.blit(TextureLoader::GUI_button, 51, 129);    //Multicolored square
                texBtn.blit(TextureLoader::GUI_button, 17, 129);    //Add blue crosshair
            }
        } else {
            texBtn.blit(TextureLoader::GUI_button, 34, 112);        //Selected button
            texBtn.blit(TextureLoader::GUI_button, 51, 129);        //Multicolored square
            texBtn.blit(TextureLoader::GUI_button, 34, 129);        //Add white crosshair
        }

        drawStateLast.first = hovering;
        drawStateLast.second = selected;
    }
    texBtn.draw();

    //Hue update
    if(lastHue!=hue && SDL_GetTicks64()>lastHueUpdate+100) {
        color.setFromHSV(hue, sat, val);

        if(selectorWindow!=nullptr) {
            selectorWindow->updateColorSelectorUI(hue);
        }

        lastHue = hue;
        lastHueUpdate = SDL_GetTicks64();
    }

    //Sat or val update
    if(lastSat!=sat || lastVal!=val) {
        lastSat = sat;
        lastVal = val;
        color.setFromHSV(hue, sat, val);

        //Update crosshair texture:
        //Horizontal + vertical bars built from pixels whose color is inverted from background
        texCrosshair.clear();
        for(int i = 0; i<9; i++) {
            NCH_Color pxCol;
            pxCol.setFromHSV(360-hue, 100-sat, 100-val);

            texCrosshair.pixel(i, 4, pxCol.getRGBA());
            texCrosshair.pixel(4, i, pxCol.getRGBA());
        }
    }
}

void ColorSelector::drawExtras()
{
    //Crosshair selector
    int dpx = colAreaX+std::round(sat*128.0/100.0)*2-8;
    if(dpx>colAreaX+246) { dpx = colAreaX+246; }
    int dpy = colAreaY+std::round(val*128.0/100.0)*2-8;
    if(dpy>colAreaY+246) { dpy = colAreaY+246; }

    texCrosshair.setDrawPos(dpx, dpy);
    texCrosshair.draw();

    //Edges of sat-val selection window
    texSelectorEdges.setDrawPos(colAreaX-8, colAreaY-8);
    texSelectorEdges.draw();
}

void ColorSelector::tick()
{
    Button::tick();

    if(controls!=nullptr && selected) {
        int mX = controls->getMouseX();
        int mY = controls->getMouseY();
        if( mX>=colAreaX-4 && mX<=colAreaX+260 ) {
            if( mY>=colAreaY-4 && mY<=colAreaY+260 ) {
                if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
                    setSatValFromXY(mX-colAreaX, mY-colAreaY);
                }
            }
        }
    }

    drawState.first = hovering;
    drawState.second = selected;
}

void ColorSelector::updateColAreaXY(int colAreaX, int colAreaY)
{
    ColorSelector::colAreaX = colAreaX;
    ColorSelector::colAreaY = colAreaY;
}

double ColorSelector::getHue() { return hue; }
double ColorSelector::getSat() { return sat; }
double ColorSelector::getVal() { return val; }
bool ColorSelector::justClicked() { return clicked; }

void ColorSelector::unclick() { clicked = false; }
void ColorSelector::deselect() { selected = false; }
void ColorSelector::setSatValFromXY(int selX, int selY)
{
    if(selX<0) { selX = 0; }
    if(selX>256) { selX = 256; }
    if(selY<0) { selY = 0; }
    if(selY>256) { selY = 256; }


    sat = std::floor(((selX)/2.0)*100.0/128.0);
    val = std::floor(((selY)/2.0)*100.0/128.0);
}
void ColorSelector::setHue(double newHue) { hue = newHue; }
void ColorSelector::setSat(double newSat) { sat = newSat; }
void ColorSelector::setVal(double newVal) { val = newVal; }