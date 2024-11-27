#pragma once
#include "TileMap.h"
#include "Tile.h"
#include "Vec3X.h"

class ShapeBuilder {
public:
    ShapeBuilder(TileMap::t_regionMap* trm);
    ShapeBuilder(TileMap* tm);
    ~ShapeBuilder();

    void seedRandFromPos(Vec3X<int64_t> worldPos);

    void fillEllipsoid(Vec3X<int64_t> o, int64_t sax, int64_t say, int64_t saz, Tile t);
    void fillSphere(Vec3X<int64_t> o, int64_t r, Tile t);
    void carveOrganicTunnel(Vec3X<int64_t> o, int64_t aSize, int64_t cSize, Tile t);
private:
    int64_t r();

    TileMap::t_regionMap* trm = nullptr;

};