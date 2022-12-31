#include "Canvas.h"
#include <math.h>
#include "Log.h"
#include "MainLoop.h"
#include "TileMap.h"

void Canvas::init(SDLHandler* sh, Controls* ctrls, Camera* cam)
{
    BTEObject::init(sh, nullptr, ctrls);
    camera = cam;

    setMaximumFPS(200);
    setCroppingRendering(false);

    realloc(0, 0);
}

void Canvas::destroy()
{
    t_texMap::iterator itrTM = texes.begin();
    while( itrTM!=texes.end() ) {
        delete itrTM->second;
        itrTM = texes.erase(itrTM);
    }
}

void Canvas::tick()
{
    //Loads textures (which will initially be empty) one at a time per tick.
    //Unloads all textures that are too far away.
    long crx = TileMap::getRegRXYZ( camera->getX() );
    long cry = TileMap::getRegRXYZ( camera->getY() );

    //if( crx!=camRX || cry!=camRY ) {
        //std::cout << "realloc\n";
        realloc( TileMap::getRegRXYZ (camera->getX() ), TileMap::getRegRXYZ(camera->getY() ) );
        camRX = crx;
        camRY = cry;
    //}


    if(camera==nullptr) {
        Log::warn(__PRETTY_FUNCTION__, "Camera is nullptr.");
    }

    //If zoom!=camera zoom, update zoom & LOD (level of detail) settings if applicable
    if( zoom!=camera->getZoom() ) {
        zoom = camera->getZoom();

        //If dynamic Level of Detailing is on
        if( texUsingDynamicLOD ) {
            //Update texLOD
            setTexLODBasedOnZoom(zoom);

            //Resize all texes
            for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {
                Texture* tex = itrTM->second;

                if( tex!=nullptr ) {
                    tex->setTexDimensions( defaultTexSize*texLOD, defaultTexSize*texLOD, true );
                }

                currentTexSize = defaultTexSize*texLOD;
            }
        }
    }

    /** Iterate through all texes */
    for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {
        int tX = itrTM->first.first;
        int tY = itrTM->first.second;
        Texture* tex = itrTM->second;

        /** Texture dilation */
        //Tile scale = the size of an onscreen tile in pixels
        double tileScale = zoom*32.0;
        //Set draw scale of tex to be 1 pixel larger than it "should" be:
        //  otherwise there might be tiny visible lines in between regions, breaking continuity.
        //  cf = correctionFactor. Always == a tiny more than 1.0.
        double cf = (zoom*((double)currentTexSize))/(zoom*((double)currentTexSize)-1);
        /* For fun: cf = (zoom*((double)currentTexSize))/(zoom*((double)currentTexSize)-1)*1.4; */

        tex->setDrawScale( zoom );
        tex->dilate(cf);
        if( texUsingDynamicLOD ) {
            tex->dilate( 1.0/texLOD );
        }

        /** Texture translations */
        //Reset location of tex
        tex->setDrawPos(0, 0);
        //Translate tex according to the world region's physical location
        tex->translate( tX*((double)defaultTexSize)*zoom, tY*((double)defaultTexSize)*zoom );
        //Translate tex according to location of the camera
        tex->translate( -camera->getX()*tileScale, -camera->getY()*tileScale );
        //Translate tex so that the center of the screen corresponds to (0, 0) if camera(x, y) = (0, 0).
        tex->translate( sdlHandler->getWidth()/2, sdlHandler->getHeight()/2 );

        /** Other info */
        //Find tile scale
        double mapZoom = camera->getZoom();
        double ts = 32*mapZoom;

        //Find where the mouse is pointing to in the world
        mouseX = ( (controls->getMouseX()-sdlHandler->getWidth()/2 )/(double)ts )/cf +camera->getX();
        mouseY = ( (controls->getMouseY()-sdlHandler->getHeight()/2 )/(double)ts )/cf +camera->getY();
    }

}

void Canvas::draw()
{
    //This block runs every msPerFrame milliseconds.
    if( (SDL_GetTicks()%1000)>(msPerFrame*framesThisSecond) ) {
        framesThisSecond++;
        frameFinished = false;
    } else {
        frameFinished = true;
    }

    //This block runs roughly every second.
    if( SDL_GetTicks()>=nextSecond ) {
        nextSecond += 1000;
        framesThisSecond = 0;
    }

    //This block runs at the current FPS and draws all textures in 'texes'.
    for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {
        Texture* tempTex = itrTM->second;
        tempTex->draw();
    }
}

void Canvas::clearCanvas()
{
    if( !forcingBlits && frameFinished )
        return;

    for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {
        Texture* tempTex = itrTM->second;
        tempTex->clear();
    }
}

void Canvas::setMaximumFPS(int fps) { msPerFrame = 1000.0 / (double)fps; }
void Canvas::setCroppingRendering(bool cr) { croppingRendering = cr; }
/**
    Dynamic LOD is when the lod changes itself depending on
    the camera's zoom.
*/
void Canvas::setTexUsingDynamicLOD(bool dLod) { texUsingDynamicLOD = dLod; }
void Canvas::setTexAllocRadiusX(int arx) { texAllocRadiusX = arx; }
void Canvas::setTexAllocRadiusY(int ary) { texAllocRadiusY = ary; }
void Canvas::setTexAllocCount(int ac) { texAllocCount = ac; }

void Canvas::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    int mapSize = 0;
    ss << "Map elements: { ";
    for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {

        ss << "(" << itrTM->first.first << ", " << itrTM->first.second << ") ";
        mapSize++;
    }
    ss << "}";
    DebugScreen::newLine(ss);

    DebugScreen::indentLine(ss, tabs);
    ss << "# of elements in map: " << mapSize << ";";
    DebugScreen::newLine(ss);

    DebugScreen::indentLine(ss, tabs);
    double onscreenSize = floor(((double)currentTexSize)*camera->getZoom());
    ss << "Texture onscreen size (px)=" << onscreenSize << ";";
    DebugScreen::newLine(ss);
}


void Canvas::getSubPos(t_ll& x, t_ll& y, t_ll& z) { x = getSubPos(x); y = getSubPos(y); z = getSubPos(z); }
Canvas::t_ll Canvas::getSubPos(t_ll c) { c %= defaultTexSize; if( c<0 ) c+=defaultTexSize; return c; }
void Canvas::getRXYZ(t_ll& x, t_ll& y, t_ll& z) { x = getRXYZ(x); y = getRXYZ(y); z = getRXYZ(z); }
Canvas::t_ll Canvas::getRXYZ(t_ll c) { c = floor(c/((double)defaultTexSize)); return c; }
float Canvas::getTexLODBasedOnZoom(double p_zoom)
{
    if( p_zoom<0.03125 ) {
        return 0.03125;
    } else
    if( p_zoom<0.0625 ) {
        return 0.0625;
    } else
    if( p_zoom<0.125 ) {
        return 0.125;
    } else
    if( p_zoom<0.25 ) {
        return 0.25;
    } else
    if( p_zoom<0.5 ) {
        return 0.5;
    } else {
        return 1.0;
    }
}

Texture* Canvas::getTex(long rX, long rY)
{
    t_texMap::iterator itr = texes.find( std::make_pair(rX, rY) );
    if( itr!=texes.end() ) {
        return itr->second;
    }
    return nullptr;
}

std::tuple<double, double> Canvas::getMouseXY()
{
    return std::make_tuple ( mouseX, mouseY );
}

void Canvas::setSourceTex(Texture* src, int srcX, int srcY)
{
    sourceTex = src->getSDLTexture();
    sourceX = srcX;
    sourceY = srcY;
    sourceW = -1;
    sourceH = -1;
}

void Canvas::setSourceTex(int srcID, int srcX, int srcY)
{
    sourceTex = sdlHandler->getTextureLoader()->getTexture(srcID);
    sourceX = srcX;
    sourceY = srcY;
    sourceW = -1;
    sourceH = -1;
}

void Canvas::setSourceTex(Texture* src)
{
    setSourceTex(src, 0, 0);
}

/**
    Reset all textures belonging to the canvas
    (reset lock settings)
*/
void Canvas::resetTexes()
{
    for( t_texMap::iterator itrTM = texes.begin(); itrTM!=texes.end(); itrTM++ ) {
        (itrTM->second)->unlock();
    }
}

/**
    Set the textures' LOD (Level of Detail). Default is 1.0.
    WARNING:
        For a texture (especially a texture full of tile images), lod should be = 1/(2^n) (1, 0.5, 0.25, 0.125, etc) for it to look good from far away.
        Thus you should always use this method to set texLOD instead of setting texLOD directly.

    Blitting becomes easier for example when map is zoomed
    out so far that a 32x32 tile becomes smaller than 32x32
    onscreen.
*/
void Canvas::setTexLODBasedOnZoom(double p_zoom)
{
    texLOD = getTexLODBasedOnZoom(p_zoom);
}

/**
    rcopyNI = RenderCopy Non-Intersecting
    If a drawn object is known not to cross any canvas
    texture borders (for example, tiles),
    use this function which performs a single blit.
*/
void Canvas::rcopyNI(t_ll dx, t_ll dy, t_ll dw, t_ll dh)
{
    if(!shouldRendRect(dx, dy, dw, dh)) return;

    Texture* tex = getTex(getRXYZ(dx), getRXYZ(dy));

    if( tex!=nullptr ) {
        tex->lock(getSubPos(dx), getSubPos(dy), dw, dh);
        tex->blit(sourceTex, sourceX, sourceY);
    }
}

/**
    General canvas blit function:
    May perform multiple blits if an object crosses
    a texture border.
*/
void Canvas::rcopy(t_ll dx, t_ll dy, t_ll dw, t_ll dh)
{
    if(!shouldRendRect(dx, dy, dw, dh)) return;

    int sx = sourceX;
    int sy = sourceY;
    int sw = sourceW;
    int sh = sourceH;

    if(sw==-1) { sw = dw; }
    if(sh==-1) { sh = dh; }

    if( dw<=0 || dh<=0 || dw>defaultTexSize || dh>defaultTexSize ) {
        Log::warn(__PRETTY_FUNCTION__, "invalid args for Canvas blit");
        return;
    }

    //Get region (defaultTexSize x defaultTexSize) coordinates of dx, dy, dx+dw, and dy+dh
    int rdX0 = getRXYZ(dx);
    int rdY0 = getRXYZ(dy);
    int rdX1 = getRXYZ(dx+dw);
    int rdY1 = getRXYZ(dy+dh);

    //Get 4 texture pointers corresponding to the locations of the 4 points of the rectangle specified by dx, dy, dw, dh.
    //These texture pointers may all point to the same texture, 2 adjacent ones, or 4 adjacent ones in a 2x2 square.
    Texture* tex00 = getTex(rdX0, rdY0);
    Texture* tex01 = getTex(rdX0, rdY1);
    Texture* tex10 = getTex(rdX1, rdY0);
    Texture* tex11 = getTex(rdX1, rdY1);


    //Get sub pos within defaultTexSize x defaultTexSize region. A subpos coordinate thus is any number in [0, defaultTexSize-1].
    int sdx = getSubPos(dx);
    int sdy = getSubPos(dy);
    int sdw = getSubPos(dx+dw);
    int sdh = getSubPos(dy+dh);

    //Lock texes
    //Upper left tex
    if( tex00!=nullptr ) {
        tex00->lock(sdx, sdy, dw, dh);
        tex00->blit(sourceTex, sx, sy);
    }
    //Upper right tex
    if( tex10!=nullptr && sdx+dw>defaultTexSize ) {
        tex10->lock(0, sdy, sdw, dh);
        tex10->blit(sourceTex, sx+defaultTexSize-sdx, sy);
    }
    //Lower left tex
    if( tex01!=nullptr && sdy+dh>defaultTexSize ) {
        tex01->lock(sdx, 0, dw, sdh);
        tex01->blit(sourceTex, sx, sy+defaultTexSize-sdy);
    }
    //Lower right tex
    if( tex11!=nullptr && sdx+dw>defaultTexSize && sdy+dh>defaultTexSize ) {
        tex11->lock(0, 0, sdw, sdh);
        tex11->blit(sourceTex, sx+defaultTexSize-sdx, sy+defaultTexSize-sdy);
    }


}

int Canvas::loadTex(long rX, long rY)
{
    Texture* tex = getTex(rX, rY);
    if(tex==nullptr) {
        Texture* tex = new Texture();
        tex->init(sdlHandler);
        tex->setBlendMode(SDL_BLENDMODE_BLEND);
        tex->setTexDimensions(currentTexSize, currentTexSize);
        texes.insert( std::make_pair(std::make_pair(rX, rY), tex) );
        return 0;
    }
    return -1;
}

void Canvas::realloc(long rX, long rY, int maxRegions)
{
    //Num of regions to be loaded in per function call that must be less than maxRegions.
    int numRegions = 0;

    //Step 1: Unload texes that are far away (not contained within the square)
    t_texMap::iterator itrTM = texes.begin();
    while( itrTM!=texes.end() ) {
        int riX = std::get<0>(itrTM->first);
        int riY = std::get<1>(itrTM->first);

        //Unload texes with extreme X or Y coords.
        if( riX<rX-texAllocRadiusX || riX>rX+texAllocRadiusY ||
            riY<rY-texAllocRadiusX || riY>rY+texAllocRadiusY
           ) {
            itrTM->second->destroy();
            delete itrTM->second;
            itrTM = texes.erase(itrTM);
        } else {
            itrTM++;
        }
    }

    //Step 2: Try to load in texes contained within the square.
    for( long ix = rX-texAllocRadiusX; ix<=rX+texAllocRadiusX; ix++ ) {
        for( long iy = rY-texAllocRadiusY; iy<=rY+texAllocRadiusY; iy++ ) {
            if( numRegions<maxRegions ) {
                if( loadTex(ix, iy)==0 ) numRegions++;
            }
        }
    }
}
void Canvas::realloc(long rX, long rY) { realloc(rX, rY, texAllocCount);}
void Canvas::reallocSingle(long rX, long rY) { realloc(rX, rY, 1); }

bool Canvas::shouldRendRect(t_ll dx, t_ll dy, t_ll dw, t_ll dh)
{
    if( !forcingBlits && frameFinished )
        return false;

    if( croppingRendering ) {
        int cx = camera->getX()*32;
        int cy = camera->getY()*32;
        int sw = sdlHandler->getWidth()/2/zoom;
        int sh = sdlHandler->getHeight()/2/zoom;

        if( dx+zoom*32<cx-sw ||
            dy+zoom*32<cy-sh ||
            dx>cx+sw ||
            dy>cy+sh
        ){
            return false;
        }
    }

    return true;
}
