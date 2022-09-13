#include "WindowPanelData.h"
#include <sstream>

WindowPanelData::WindowPanelData(int w, int h)
{
    width = w;
    height = h;

    std::string line = "";
    for( int x = 0; x<w; x++ ) line += "x";
    for( int y = 0; y<h; y++ ) {
        setPanelData(y, line);
    }
}

WindowPanelData::WindowPanelData(int w, int h, std::string upperPanel, std::string lowerPanel):
WindowPanelData::WindowPanelData(w, h)
{
    WindowPanelData::lowerPanel = lowerPanel;
    WindowPanelData::upperPanel = upperPanel;

    for( int j = 0; j<height; j++ ) {
        std::string thisLine = "";
        char c = '1';
        if( j==0 || j==height-1 ) {
            c = '0';
        }
        for( int i = 0; i<width; i++ ) {
            thisLine += c;
        }
        setPanelData(j, thisLine);
    }
}

WindowPanelData::~WindowPanelData()
{
}

std::string WindowPanelData::getLowerPanel() { return lowerPanel; }
std::string WindowPanelData::getUpperPanel() { return upperPanel; }
int WindowPanelData::getWidth() { return width; }
int WindowPanelData::getHeight() { return height; }
char WindowPanelData::getPanelData(int x, int y)
{
    auto obj = panelData.find(y);
    if( obj!=panelData.end() ) {
        std::string s = obj->second;
        if( x>=0 && x<=(int)s.size()-1 ) {
            return s[x];
        }
    }
    return '\0';
}


void WindowPanelData::setPanelData(int line, std::string data)
{
    auto obj = panelData.find(line);
    if( obj!=panelData.end() ) {
        panelData.erase( obj );
    }

    panelData.insert(std::make_pair(line, data));
}

void WindowPanelData::setPanelColor(char ch, Color& col)
{
    auto obj = colors.find(ch);
    if( obj!=colors.end() ) {
        colors.erase( obj );
    }

    colors.insert(std::make_pair(ch, col));
}

void WindowPanelData::buildTex(Texture* windowTex)
{
    int img = TextureLoader::GUI_window;

    int dx0[] = { 00, -1, 01, 00 };
    int dy0[] = { -1, 00, 00, 01 };

    int dx1[] = { 0, 1, 1, 0 };
    int dy1[] = { 0, 0, 1, 1 };


    int s0 = sizeof(dx0)/sizeof(dx0[0]);

    for( int x = 0; x<width; x++ ) {
        for( int y = 0; y<height; y++ ) {
            char curr = getPanelData(x, y);

            //Build sides (simple lines)
            for( int i = 0; i<s0; i++ ) {
                char next = getPanelData(x+dx0[i], y+dy0[i]);

                if( next!='\0' && curr!=next ) {
                    //Change in lock x, lock y
                    int dlx = 0; int dly = 0;
                    //blitsrc x, blitsrc y
                    int by = 0; int bx = 0;
                    //Lock w, lock h
                    int lw = 0; int lh = 0;

                    //Set change in lock x and lock y
                    switch(i) {
                        case 0: { dlx = 02; dly = 00; } break;
                        case 1: { dlx = 00; dly = 02; } break;
                        case 2: { dlx = 30; dly = 02; } break;
                        case 3: { dlx = 02; dly = 30; } break;
                    }

                    //Set blitsrc x and blitsrc y
                    switch(i) {
                        case 0: { bx = 05; by = 02; } break;
                        case 1: { bx = 02; by = 05; } break;
                        case 2: { bx = 38; by = 05; } break;
                        case 3: { bx = 05; by = 38; } break;
                        default:{ bx = 43; by = 00; } break;
                    }

                    //Set lock w and lock h
                    switch(i) {
                        case 0: case 3: { lw = 28; lh = 02; } break;
                        case 1: case 2: { lw = 02; lh = 28; } break;
                    }

                    windowTex->lock(4+32*x+dlx, 4+32*y+dly, lw, lh);
                    windowTex->blit(img, bx, by);
                }
            }

            //Build corners
            /*
                nw = 0, ne = 1, se = 2, sw = 3
            */
            for( int i = 0; i<4; i++ ) {
                /* Lock depending on if i is nw, ne, sw, or se */
                //Change in lock x, lock y
                int dlx = 0; int dly = 0;
                //Change in blit x, blit y
                int dbx = 0; int dby = 0;

                //Set lock x and lock y
                switch(i) {
                    case 0: { dlx = 00; dly = 00; } break;
                    case 1: { dlx = 30; dly = 00; } break;
                    case 2: { dlx = 30; dly = 30; } break;
                    case 3: { dlx = 00; dly = 30; } break;
                }
                //Lock 2x2 area at (lock x, lock y)
                windowTex->lock(4+32*x+dlx, 4+32*y+dly, 2, 2);

                /* Blit depending on what is at nw, ne, sw, and se */
                //Get panel char at nw, ne, sw, and se of this corner's location.

                char pd[8];
                char nw = getPanelData(x-1+dx1[i], y-1+dy1[i]);
                char ne = getPanelData(x+0+dx1[i], y-1+dy1[i]);
                char se = getPanelData(x+0+dx1[i], y+0+dy1[i]);
                char sw = getPanelData(x-1+dx1[i], y+0+dy1[i]);
                for( int j = 0; j<5; j+=4 ) {
                    pd[0+j] = nw;
                    pd[1+j] = ne;
                    pd[2+j] = se;
                    pd[3+j] = sw;
                }

                if( (pd[3+i]&&!pd[0+i]&&!pd[1+i])&&pd[3+i]!=pd[2+i] ) {
                    windowTex->blitEx(img, 45, 2, (i%4)*90);
                }
                if( (pd[1+i]&&!pd[0+i]&&!pd[3+i])&&pd[1+i]!=pd[2+i] ) {
                    windowTex->blitEx(img, 43, 0, (i%4)*90);
                }

                //Do nothing
                if( pd[0+i] && pd[1+i] ) {
                    //2 dark points
                    if( pd[3+i]==pd[2+i] && pd[1+i]!=pd[2+i] ) { windowTex->blitEx(img, 43, 0, (i%4)*90); }
                    if( pd[1+i]==pd[2+i] && pd[3+i]!=pd[2+i] ) { windowTex->blitEx(img, 45, 2, (i%4)*90); }
                    //3 dark points
                    if( pd[1+i]==pd[2+i] && pd[3+i]==pd[2+i] && pd[0+i]!=pd[3+i] ) { windowTex->blitEx(img, 48, 0, (i%4)*90); }
                    //1 dark point
                    if( pd[1+i]!=pd[2+i] && pd[3+i]!=pd[2+i] ) { windowTex->blitEx(img, 53, 0, (i%4)*90); }
                }
            }
        }
    }
}

std::string WindowPanelData::toString()
{
    std::stringstream ss;
    for(auto pdItr = panelData.begin(); pdItr!=panelData.end(); pdItr++ ) {
        ss << pdItr->second << "\n";
    }

    return ss.str();
}
