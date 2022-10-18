#include "Window.h"
#include "TextureBuilder.h"
#include "Log.h"
/**/
int Window::bkgdScroll = 0;

Window::Window(int x, int y, WindowPanelData* panelData, int id)
{
    setTypeAndID(BTEObject::GUI_window, id);
    Window::panelData = panelData;

    //Set alignments
    if( x<-1000 ) { horAlignment = x; }
    if( y<-1000 ) { verAlignment = y; }

    //Set window pos and dims
    sX = (x/2*2);
    sY = (y/2*2);

    Window::panelData = panelData;

    if( panelData!=nullptr ) {
        Window::width = (panelData->getWidth()*64);
        Window::height = (panelData->getHeight()*64);
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
Window(x, y, new WindowPanelData(width/64, height/64, upperPanel, lowerPanel), id){}

Window::Window(int id): Window(0, 0, 0, 0, "", "", id)
{
    bkgd = true;
}

Window::~Window()
{
    if(panelData!=nullptr) {
        delete panelData;
    }
}

void Window::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    TextureBuilder tb(sdlHandler);
    int texW = width/2;
    int texH = height/2;
    int img = TextureLoader::Textures::GUI_window;
    int img2 = TextureLoader::Textures::WORLD_background_space_interstellar;

    //Build application sub-window
    if(!bkgd) {
        tb.init(tb.WINDOW, windowTex, texW, texH);
        windowTex.setDrawScale(2);
    //Build a 'background' scrolling window
    } else {
        windowTex.init(sdlHandler);
        windowTex.setTexDimensions(1024, 1024);
        windowTex.lock(0, 0, 1024, 1024);
        windowTex.blit(img2);
        windowTex.setDrawScale(2);
    }

    if( panelData!=nullptr ) {
        panelData->buildTex(&windowTex);
    }

    /* Upper and lower panels */
    //Upper panel
    if( panelData!=nullptr ) {
        /*
        if( panelData->getUpperPanel()!="" ) {
            for(int i = 0; i<texW; i+=32) {
                windowTex.lock( 4+i, 5, 32, 30 );
                windowTex.blit(img, 48,  0);
            }
        }
        */
        upperPanelText.init(sdlHandler);
        upperPanelText.setString(panelData->getUpperPanel());

        //Lower panel
        /*
        if( panelData->getLowerPanel()!="" ) {
            for(int i = 0; i<texW; i+=32) {
                windowTex.lock( 4+i,texH-26, 32, 30 );
                windowTex.blit(img, 48, 31);
            }
        }
        */
        lowerPanelText.init(sdlHandler);
        lowerPanelText.setString(panelData->getLowerPanel());
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

WindowPanelData* Window::getWindowPanelData()
{
    return panelData;
}

/**/
