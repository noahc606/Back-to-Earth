#pragma once
#include <map>
#include "Camera.h"
#include "Texture.h"
#include "BTEObject.h"
#include "Loggable.h"

class Canvas : public BTEObject, public Loggable
{
public:
    typedef std::map<std::pair<long, long>, Texture*> t_texMap;
    /**/
    void init(SDLHandler* sh, Controls* ctrls, Camera* cam);
    void destroy();
    /**/
    void tick();
    void draw();
    /**/
    //Load / unload textures from map given the location of a 1024x1024 region.
    int loadTex(int64_t csRX, int64_t csRY);
    void realloc(int64_t csRX, int64_t csRY, int maxRegions);
    void realloc(int64_t csRX, int64_t csRY);
    void reallocSingle(int64_t csRX, int64_t csRY);
    /**/
    //Get a texture from the map given the location of a 1024x1024 region.
    Texture* getTex(long rX, long rY);
    std::tuple<double, double> getMouseXY();
    bool isFrameFinished();

    //Get info about the Canvas.
    void putInfo(std::stringstream& ss, int& tabs);
    Camera* getCamera();
    //Get sub position, or position of a region given t_ll (long long) coordinates.
    void getSubPos(int64_t& x, int64_t& y, int64_t& z);
    int64_t getSubPos(int64_t c);
    void getRXYZ(int64_t& x, int64_t& y, int64_t& z);
    int64_t getRXYZ(int64_t c);
    static float getTexLODBasedOnZoom(double zoom);

    /**/

    //Canvas texture details
    void resetTexes();
    void setTexLODBasedOnZoom(double zoom);
    //Canvas blitting (src)
    void setSourceTex(Texture* src, int srcX, int srcY);
    void setSourceTex(int srcID, int srcX, int srcY);
    void setSourceTex(Texture* src);
    //Canvas blitting (dst)
    void clearCanvas();
    void rcopyNI(int64_t dx, int64_t dy, int64_t dw, int64_t dh);
    void rcopy(int64_t dx, int64_t dy, int64_t dw, int64_t dh);

    //Optimization options
    void setMaximumFPS(int fps);
    void setCroppingRendering(bool cr);
    void setTexUsingDynamicLOD(bool dLod);
    void setTexAllocRadiusX(int arx);
    void setTexAllocRadiusY(int ary);
    void setTexAllocCount(int ac);
    void setMoveWithCamera(bool mwc);
protected:

private:
    /**/
    bool shouldRendRect(int64_t dx, int64_t dy, int64_t dw, int64_t dh);
    /**/

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

    //Map of texes and how many to draw.
    t_texMap texes;
    int defaultTexSize = 1024; int currentTexSize = 1024;
    float texLOD = 1.0;
    bool texUsingDynamicLOD = false;
    int texAllocRadiusX = 3;
    int texAllocRadiusY = 3;
    int texAllocCount = 1;

    //Extra features
    bool moveWithCamera = true; // Move with camera is disabled for certain Canvases (Sky)
    
    //Camera object
    Camera* camera = nullptr; double zoom = 0;
    long camRX = 0; long camRY = 0;

    //Mouse info
    double mouseX = 0; double mouseY = 0;
};
