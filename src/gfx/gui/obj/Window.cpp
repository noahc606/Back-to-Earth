#include "Window.h"
#include "ColorSelector.h"
#include "Log.h"
#include "TextureBuilder.h"
/**/
double Window::bkgdScrollA = 0;
double Window::bkgdScrollB = 0;

Window::Window(int x, int y, WindowData* winData, int id)
{
    setTypeAndID(BTEObject::GUI_window, id);
    Window::winData = winData;

    //Set alignments
    if( x<-1000 ) { horAlignment = x; }
    if( y<-1000 ) { verAlignment = y; }

    //Set window pos and dims
    sX = (x/2*2);
    sY = (y/2*2);

    Window::winData = winData;

    if( winData!=nullptr ) {
        Window::width = (winData->getWidth()*64);
        Window::height = (winData->getHeight()*64);
    }


    /**
        111222333333
        111xxx333333
        111xxx333333
        111xxx333333
        111xxx333333
        111222333333
        111222111111
        111222111111
    */

    onWindowUpdate(false);
}

Window::Window(int x, int y, int width, int height, std::string upperPanel, std::string lowerPanel, int id):
Window(x, y, new WindowData(width/64, height/64, upperPanel, lowerPanel), id){}

Window::Window(int id): Window(0, 0, 0, 0, "", "", id)
{
    winData->setSpecialType( WindowData::BACKGROUND );
}

Window::~Window()
{
    if(winData!=nullptr) {
        delete winData;
    }
}

void Window::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    TextureBuilder tb(sdlHandler);
    int texW = width/2;
    int texH = height/2;

    if( winData!=nullptr ) {
        switch( winData->getSpecialType() )
        {
        case WindowData::BACKGROUND: {
            //Build the two background scrolling windows
            windowTexBkgdA.init(sdlHandler);
            windowTexBkgdA.setTexDimensions(1024, 1024);
            windowTexBkgdA.lock();
            windowTexBkgdA.blit(TextureLoader::WORLD_background_space_interstellar);
            windowTexBkgdA.setDrawScale(2);

            windowTexBkgdB.init(sdlHandler);
            windowTexBkgdB.setTexDimensions(1024, 1024);
            windowTexBkgdB.lock();
            windowTexBkgdB.blit(TextureLoader::WORLD_background_space_intergalactic);
            windowTexBkgdB.setDrawScale(2);
        } break;

        case WindowData::COLOR_SELECTOR: {
            tb.buildWindow(windowTex, winData, texW, texH);
            windowTex.setDrawScale(2);

            ColorSelector* cs = (ColorSelector*)winData->getRelatedUI();
            if(cs!=nullptr) {
                updateColorSelectorUI(cs->getHue());
            }
        } break;
        
        default: {
            //Build application sub-window
            tb.buildWindow(windowTex, winData, texW, texH);
            windowTex.setDrawScale(2);
        } break;
        }

        winData->buildTex(&windowTex);
    }

    /* Upper and lower panels */
    //Upper panel
    if( winData!=nullptr ) {
        upperPanelText.init(sdlHandler);
        upperPanelText.setString(winData->getUpperPanel());

        lowerPanelText.init(sdlHandler);
        lowerPanelText.setString(winData->getLowerPanel());
    }

    onWindowUpdate(true);
}

void Window::destroy()
{
    BTEObject::destroy();

    if(winData->getSpecialType()==WindowData::BACKGROUND) {
        windowTex.destroy();
    }

    if(winData->getSpecialType()==WindowData::COLOR_SELECTOR) {
        ColorSelector* cs = (ColorSelector*)winData->getRelatedUI();
        cs->setSelectorWindow(nullptr);
    }
}

/**/

void Window::draw()
{
    switch( winData->getSpecialType() )
    {
    case WindowData::BACKGROUND: {
        //Draw windowTexBkgdB twice
        int wtx = 0; int wty = 0; double wts = 0;           wtx = windowTexBkgdB.getDrawX(); wty = windowTexBkgdB.getDrawY(); wts = windowTexBkgdB.getDrawScale();
        int wtw = 0; int wth = 0;                           wtw = windowTexBkgdB.getTexWidth(); wth = windowTexBkgdB.getTexHeight();
        windowTexBkgdB.setDrawPos(wtx-wtw*wts, wty);        windowTexBkgdB.draw();
        windowTexBkgdB.setDrawPos(wtx, wty);                windowTexBkgdB.draw();

        //Draw windowTexBkgdA twice
        wtx = windowTexBkgdA.getDrawX(); wty = windowTexBkgdA.getDrawY(); wts = windowTexBkgdA.getDrawScale();
        wtw = windowTexBkgdA.getTexWidth(); wth = windowTexBkgdA.getTexHeight();
        windowTexBkgdA.setDrawPos(wtx-wtw*wts, wty);        windowTexBkgdA.draw();
        windowTexBkgdA.setDrawPos(wtx, wty);                windowTexBkgdA.draw();
    } break;

    case WindowData::COLOR_SELECTOR: {
        windowTex.draw();
        upperPanelText.draw();
        lowerPanelText.draw();

        if( winData->getRelatedUI()!=nullptr && winData->getRelatedUI()->getType()==BTEObject::GUI_colorselect ) {
            ColorSelector* cs = (ColorSelector*)winData->getRelatedUI();
            cs->drawExtras();
        }
    } break;

    default: {
        windowTex.draw();
        upperPanelText.draw();
        lowerPanelText.draw();
    } break;
    }
}

void Window::tick()
{
    switch( winData->getSpecialType() )
    {
    case WindowData::BACKGROUND: {
        //Increment scroll of both textures
        bkgdScrollA+=0.7;
        bkgdScrollB+=0.2;

        //Get information of both textures
        int wtwA = windowTexBkgdA.getTexWidth();
        int wthA = windowTexBkgdA.getTexHeight();
        int wtwB = windowTexBkgdB.getTexWidth();
        int wthB = windowTexBkgdB.getTexHeight();

        //Set draw position of both textures
        windowTexBkgdA.setDrawPos(((int)bkgdScrollA)*windowTexBkgdA.getDrawScale(), 0);
        if( bkgdScrollA>wtwA ) {
            windowTexBkgdA.setDrawPos(0, 0);
            bkgdScrollA = 0;
        }
        windowTexBkgdB.setDrawPos(((int)bkgdScrollB)*windowTexBkgdB.getDrawScale(), 0);
        if( bkgdScrollB>wtwB ) {
            windowTexBkgdB.setDrawPos(0, 0);
            bkgdScrollB = 0;
        }

    } break;

    case WindowData::COLOR_SELECTOR: {
        int wtw = windowTex.getTexWidth();
        int wth = windowTex.getTexHeight();

        ColorSelector* cs = (ColorSelector*)winData->getRelatedUI();
        cs->updateColAreaXY(sX+wtw/2-100, sY+wth-100);
        cs->setSelectorWindow(this);
    } break;

    }
}

void Window::onWindowUpdate(bool preventInvalidTPos)
{
    windowTex.setDrawPos(sX, sY);

    upperPanelText.setPos( sX+width/2-upperPanelText.getWidth()/4+12, sY+32 );
    lowerPanelText.setPos( sX+width/2-upperPanelText.getWidth()/2, sY+height-64 );


    if( winData->getSpecialType()==WindowData::BACKGROUND ) {
        width = sdlHandler->getWidth();
        height = sdlHandler->getHeight();

        windowTexBkgdA.setDrawPos(bkgdScrollA*windowTexBkgdA.getDrawScale(), 0);
    }
}

WindowData* Window::getWindowData() { return winData; }
bool Window::isActive() { return active; }

void Window::setActive(bool isActive) { Window::active = isActive; }

void Window::updateColorSelectorUI(double hue)
{
    if(winData->getSpecialType()!=WindowData::COLOR_SELECTOR) {
        return;
    }

    int wtw = windowTex.getTexWidth();
    int wth = windowTex.getTexHeight();
    int uiX = wtw/4-50; int uiW = 128;
    int uiY = wth/2-50; int uiH = 128;

    //Color gradient
    for(int ix = 0; ix<uiW; ix++) {
        for(int iy = 0; iy<uiH; iy++) {
            nch::Color c; c.setFromHSV(hue, ((double)ix)*100.0/128.0, ((double)iy)*100.0/128.0);
            windowTex.pixel(uiX+ix, uiY+iy, c.getRGBA());
        }
    }
}