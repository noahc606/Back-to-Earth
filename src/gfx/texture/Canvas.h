#pragma once
#include <map>
#include "Camera.h"
#include "Texture.h"
#include "BTEObject.h"

class Canvas : public BTEObject
{
public:
    typedef long long t_ll;
    typedef std::map<std::pair<long, long>, Texture*> t_texMap;
    /**/
    void init(SDLHandler* sh, Controls* ctrls, Camera* cam);
    void destroy();
    /**/
    void tick();
    void draw();
    /**/
    //Load / unload textures from map given the location of a 1024x1024 region.
    int loadTex(long rX, long rY);

    void realloc(long x, long y, int maxRegions);
    void realloc(long x, long y);
    /**/
    //Get a texture from the map given the location of a 1024x1024 region.
    Texture* getTex(long rX, long rY);
    std::tuple<double, double> getMouseXY();

    //Get info about the Canvas.
    void info(std::stringstream& ss, int& tabs);
    //Get sub position, or position of a region given t_ll (long long) coordinates.
    void getSubPos(t_ll& x, t_ll& y, t_ll& z);
    t_ll getSubPos(t_ll c);
    void getRXYZ(t_ll& x, t_ll& y, t_ll& z);
    t_ll getRXYZ(t_ll c);
    /**/

    //Canvas manipulation
    void setSrc(Texture* src, int srcX, int srcY);
    void setSrc(int srcID, int srcX, int srcY);
    void setSrc(Texture* src);
    void unlock();
    void clearCanvas();
    void rcopyLazy(t_ll dx, t_ll dy, t_ll dw, t_ll dh);
    void rcopy(t_ll dx, t_ll dy, t_ll dw, t_ll dh);
    //Set optimization options for Canvas
    void setFPS(int fps);
    void cropRendering(bool cr);

    /**/

    /**/

protected:

private:
    /**/
    bool shouldRendRect(t_ll dx, t_ll dy, t_ll dw, t_ll dh);
    /**/
    void updateDrawSettings();

    //Extra information for optimizing render functions.
    bool croppingRendering = false;
    bool forcingBlits = false;

    //Handle when to draw if limiting FPS
    uint64_t nextSecond = 0;
    bool frameFinished = false;
    int framesThisSecond = 0;
    double msPerFrame = 0;

    //Source texture and (x, y, w, h) info used in rcopy().
    SDL_Texture* source;
    int sourceX = 32; int sourceY = 32; int sourceW = -1; int sourceH = -1;

    //Map of texes
    t_texMap texes;
    int texSize = 1024;

    //Camera object
    Camera* camera; double zoom = 0;

    //Mouse info
    double mouseX = 0; double mouseY = 0;

};
