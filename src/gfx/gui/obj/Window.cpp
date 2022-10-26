#include "Window.h"
#include "TextureBuilder.h"
#include "Log.h"
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
    bkgd = true;
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
    int space = TextureLoader::Textures::WORLD_background_space_interstellar;

    if( winData!=nullptr ) {

        //Build application sub-window
        if(!bkgd) {
            tb.buildWindow(windowTex, winData, texW, texH);
            windowTex.setDrawScale(2);
        //Build a 'background' scrolling window
        } else {
            windowTex.init(sdlHandler);
            windowTex.setTexDimensions(1024, 1024);
            windowTex.lock(0, 0, 1024, 1024);
            windowTex.blit(space);
            windowTex.setDrawScale(2);
        }

        winData->buildTex(&windowTex);
    }

    /* Upper and lower panels */
    //Upper panel
    if( winData!=nullptr ) {
        /*
        if( winData->getUpperPanel()!="" ) {
            for(int i = 0; i<texW; i+=32) {
                windowTex.lock( 4+i, 5, 32, 30 );
                windowTex.blit(img, 48,  0);
            }
        }
        */
        upperPanelText.init(sdlHandler);
        upperPanelText.setString(winData->getUpperPanel());

        //Lower panel
        /*
        if( winData->getLowerPanel()!="" ) {
            for(int i = 0; i<texW; i+=32) {
                windowTex.lock( 4+i,texH-26, 32, 30 );
                windowTex.blit(img, 48, 31);
            }
        }
        */
        lowerPanelText.init(sdlHandler);
        lowerPanelText.setString(winData->getLowerPanel());
    }

    onWindowUpdate(true);
}

void Window::destroy()
{
    BTEObject::destroy();
    windowTex.destroy();
}

/**/

void Window::draw()
{
    if( bkgd ) {
        int wtx = 0; int wty = 0; double wts = 0;
        windowTex.queryDrawInfo(wtx, wty, wts);
        int wtw = 0; int wth = 0;
        windowTex.queryTexInfo(wtw, wth);

        windowTex.setDrawPos(wtx-wtw*wts, wty);
        windowTex.draw();
        windowTex.setDrawPos(wtx, wty);

        windowTex.draw();
    } else {
        windowTex.draw();
        upperPanelText.draw();
        lowerPanelText.draw();
    }


}

void Window::tick()
{
    if( bkgd ) {
        bkgdScroll+=1;

        int wtw = 0; int wth = 0;
        windowTex.queryTexInfo(wtw, wth);

        windowTex.setDrawPos(bkgdScroll*windowTex.getDrawScale(), 0);
        if( bkgdScroll>wtw ) {
            windowTex.setDrawPos(0, 0);
            bkgdScroll = 0;
        }
    }
}

void Window::onWindowUpdate(bool preventInvalidTPos)
{
    windowTex.setDrawPos(sX, sY);

    upperPanelText.setPos( sX+width/2-upperPanelText.getWidth()/4, sY+32 );
    lowerPanelText.setPos( sX+width/2-upperPanelText.getWidth()/2, sY+height-64 );


    if(bkgd) {
        width = sdlHandler->getWidth();
        height = sdlHandler->getHeight();

        windowTex.setDrawPos(bkgdScroll*windowTex.getDrawScale(), 0);
    }
}

WindowData* Window::getWindowData()
{
    return winData;
}

/**/
