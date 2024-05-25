#include "Window.h"
#include "ColorSelector.h"
#include "Log.h"
#include "TextureBuilder.h"
/**/
int Window::bkgdScroll = 0;

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
            //Build a 'background' scrolling window
            windowTex.init(sdlHandler);
            windowTex.setTexDimensions(1024, 1024);
            windowTex.lock(0, 0, 1024, 1024);

            int space = TextureLoader::Textures::WORLD_background_space_interstellar;
            windowTex.blit(space);
            windowTex.setDrawScale(2);
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
        int wtx = 0; int wty = 0; double wts = 0;
        windowTex.queryDrawInfo(wtx, wty, wts);
        int wtw = 0; int wth = 0;
        windowTex.queryTexInfo(wtw, wth);

        windowTex.setDrawPos(wtx-wtw*wts, wty);
        windowTex.draw();
        windowTex.setDrawPos(wtx, wty);

        windowTex.draw();
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
        bkgdScroll+=1;

        int wtw = 0; int wth = 0;
        windowTex.queryTexInfo(wtw, wth);

        windowTex.setDrawPos(bkgdScroll*windowTex.getDrawScale(), 0);
        if( bkgdScroll>wtw ) {
            windowTex.setDrawPos(0, 0);
            bkgdScroll = 0;
        }
    } break;

    case WindowData::COLOR_SELECTOR: {
        int wtw = 0; int wth = 0;
        windowTex.queryTexInfo(wtw, wth);

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

        windowTex.setDrawPos(bkgdScroll*windowTex.getDrawScale(), 0);
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

    int wtw = 0; int wth = 0;
    windowTex.queryTexInfo(wtw, wth);
    int uiX = wtw/4-50; int uiW = 128;
    int uiY = wth/2-50; int uiH = 128;

    //Color gradient
    for(int ix = 0; ix<uiW; ix++) {
        for(int iy = 0; iy<uiH; iy++) {
            Color c; c.setFromHSV(hue, ((double)ix)*100.0/128.0, ((double)iy)*100.0/128.0);
            windowTex.pixel(uiX+ix, uiY+iy, c.getRGBA());
        }
    }
}