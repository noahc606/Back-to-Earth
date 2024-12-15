#pragma once
#include "TileMap.h"
#include "Tile.h"
#include <nch/math-utils/vec3.h>

class ShapeBuilder {
public:
    ShapeBuilder(TileMap::t_regionMap* trm);
    ShapeBuilder(TileMap* tm);
    ~ShapeBuilder();

    void seedRandFromPos(nch::Vec3<int64_t> worldPos);

    void fillEllipsoid(nch::Vec3<int64_t> o, int64_t sax, int64_t say, int64_t saz, Tile t);
    void fillEllipsoid(nch::Vec3<int64_t> o, nch::Vec3<int64_t> axes, Tile t);
    void fillSphere(nch::Vec3<int64_t> o, int64_t r, Tile t);
    void fillBlob(nch::Vec3<int64_t> o, int64_t aSize, int cSize, std::vector<Tile>& tileset);
    void fillTraces(nch::Vec3<int64_t> o, int64_t aSize, int num, std::vector<Tile>& tileset);
    nch::Vec3<int64_t> carveOrganicTunnel(nch::Vec3<int64_t> o, int64_t aSize, int64_t cSize, Tile t);
private:
    int64_t r();
    int64_t r(int64_t mod);

    TileMap::t_regionMap* trm = nullptr;
    bool useNaturalTiles = true;
};