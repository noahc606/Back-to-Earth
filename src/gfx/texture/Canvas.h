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


    //Canvas texture details
    void resetTexes();
    void setTexLOD(float lod);
    //Canvas blitting (src)
    void setSourceTex(Texture* src, int srcX, int srcY);
    void setSourceTex(int srcID, int srcX, int srcY);
    void setSourceTex(Texture* src);
    //Canvas blitting (dst)
    void clearCanvas();
    void rcopyNI(t_ll dx, t_ll dy, t_ll dw, t_ll dh);
    void rcopy(t_ll dx, t_ll dy, t_ll dw, t_ll dh);

    //Optimization options
    void setMaximumFPS(int fps);
    void setCroppingRendering(bool cr);
    void setUsingDynamicLOD(bool dLod);

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
    SDL_Texture* sourceTex;
    int sourceX = 32; int sourceY = 32; int sourceW = -1; int sourceH = -1;

    //Map of texes
    t_texMap texes;
    int texSize = 1024;
    float texLOD = 1.0;
    bool dynamicLOD = false;

    //Camera object
    Camera* camera; double zoom = 0;

    //Mouse info
    double mouseX = 0; double mouseY = 0;

};
