#pragma once
#include "BTEObject.h"
#include "Canvas.h"
#include "Player.h"
#include "Texture.h"
#include "TileMap.h"
#include "TileMapScreen.h"

class World : public BTEObject
{
public:
    typedef TileMap::t_ll t_ll;
    /**/
    void init(SDLHandler* sh, Controls* ctrls);
    virtual ~World();
    /**/
    void draw();
    void tick();
    /**/
    void info(std::stringstream& ss, int& tabs);
protected:

private:
    Texture defaultTile;


    int counter = 0;

    double prevZoom = -1;
    Player player;
    double mouseX = 0; double mouseY = 0;
    t_ll mouseXLL = 0; t_ll mouseYLL = 0; t_ll mouseZLL = 0;

    bool paused = false;

    TileMap tileMap;
    TileMapScreen tileMapScreen;

    Canvas csTileMap;
    Canvas csATileMap;
    Canvas csInteractions;
    Canvas csEntities;

    void updateMouseAndCamInfo();
    void entityInteractions();
};
