#include "ColorSelector.h"

ColorSelector::ColorSelector(Window* parentWindow, int x, int y, Color col, int id)
: Button::Button(parentWindow, x, y, 0, "", id)
{
	setSubType(BTEObject::Type::GUI_colorselect);

	btnString = "";
    width = 32;
    height = 32;
}

ColorSelector::ColorSelector(Window* parentWindow, int x, int y, int id):
ColorSelector(parentWindow, x, y, Color(), id){}

void ColorSelector::init(SDLHandler* sh, Controls* ctrls)
{
	GUI::init(sh, ctrls);

	texBtn.init(sdlHandler);
	buildTexes();

	onWindowUpdate();

    texCrosshair = new Texture();
    texCrosshair->init(sdlHandler, 11, 11, 2);
}

void ColorSelector::buildTexes()
{
    texBtn.clear();
    texBtn.setTexDimensions(16, 16);
    texBtn.setDrawScale(2);
    texBtn.lock();
    texBtn.blit(TextureLoader::GUI_button, 0, 112);
    texBtn.blit(TextureLoader::GUI_button, 51, 129);
    texBtn.blit(TextureLoader::GUI_button, 0, 129);
}

void ColorSelector::onWindowUpdate()
{
	Button::onWindowUpdate();

    texBtn.setDrawPos(sX, sY);
}

void ColorSelector::draw()
{
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

    if(lastHue!=hue || lastSat!=sat || lastVal!=val) {
        lastHue = hue;
        lastSat = sat;
        lastVal = val;

        //Update crosshair texture:
        //Horizontal + vertical bars built from pixels whose color is inverted from background
        texCrosshair->clear();
        for(int i = 0; i<11; i++) {
            Color pxCol;
            pxCol.setFromHSV(360-hue, 100-sat, 100-val);

            texCrosshair->pixel(i, 5, pxCol.getRGBA());
            texCrosshair->pixel(5, i, pxCol.getRGBA());
        }
    }
}

void ColorSelector::updateColorSelectorUI(Texture* windowTex)
{
    int wtw = 0; int wth = 0;
    windowTex->queryTexInfo(wtw, wth);
    int uiX = wtw/4-50; int uiW = 100;
    int uiY = wth/2-50; int uiH = 100;

    //Color gradient
    for(int ix = 0; ix<uiW; ix++) {
        for(int iy = 0; iy<uiH; iy++) {
            Color c; c.setFromHSV(hue, (double)ix, (double)iy);
            windowTex->pixel(uiX+ix, uiY+iy, c.getRGBA());
        }
    }
}

void ColorSelector::drawCrosshair(int colAreaX, int colAreaY)
{
    texCrosshair->setDrawPos(colAreaX+sat*2-12, colAreaY+val*2-12);
    texCrosshair->draw();
}

void ColorSelector::tick()
{
    Button::tick();

    drawState.first = hovering;
    drawState.second = selected;
}

void ColorSelector::trackMouse(int colAreaX, int colAreaY)
{
    int mX = controls->getMouseX();
    int mY = controls->getMouseY();
    if( mX>=colAreaX && mX<=colAreaX+200 ) {
        if( mY>=colAreaY && mY<=colAreaY+200 ) {
            if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
                setSVFromXY(mX-colAreaX, mY-colAreaY);
            }
        }
    }
}

double ColorSelector::getSat() { return sat; }
double ColorSelector::getVal() { return val; }
bool ColorSelector::justClicked() { return clicked; }

void ColorSelector::unclick() { clicked = false; }
void ColorSelector::setSVFromXY(int selX, int selY)
{
    sat = selX/2;
    val = selY/2;
    color.setFromHSV(hue, sat, val);
}