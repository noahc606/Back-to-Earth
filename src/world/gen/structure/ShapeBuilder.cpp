#include "ShapeBuilder.h"
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include "TileIterator.h"
#include "Noise.h"

using namespace nch;

ShapeBuilder::ShapeBuilder(TileMap::t_regionMap* trm) { ShapeBuilder::trm = trm; }
ShapeBuilder::ShapeBuilder(TileMap* tm): ShapeBuilder(tm->getRegionMap()) {}
ShapeBuilder::~ShapeBuilder(){}

void ShapeBuilder::seedRandFromPos(Vec3<int64_t> worldPos)
{
    //Seed RNG by hashing xyz
    Noise n(0);
    std::srand(n.hash3ToUint32(worldPos.x, worldPos.y, worldPos.z));
}

void ShapeBuilder::fillEllipsoid(Vec3<int64_t> o, int64_t sax, int64_t say, int64_t saz, Tile t)
{
    //Helper values
    int64_t sax2 = sax*sax, say2 = say*say, saz2 = saz*saz;

    //Create TileIterator with rectangular-prism bounds containing the entire ellipsoid
    TileIterator ti(trm);
    ti.setBounds(o.x-sax, o.y-say, o.z-saz, o.x+sax, o.y+say, o.z+saz);

    //Build shape
	for(TileRegion* tr = ti.peekRegion(); !ti.atEnd(); tr = ti.nextRegion()) {
        if(tr==nullptr) continue;

        int palIdx = tr->addToPalette(t, useNaturalTiles);
		int64_t iRX = ti.gir(0), iRY = ti.gir(1), iRZ = ti.gir(2);
        for(int64_t isx = ti.gbs(0); isx<=ti.ges(0); isx++)
        for(int64_t isy = ti.gbs(1); isy<=ti.ges(1); isy++)
        for(int64_t isz = ti.gbs(2); isz<=ti.ges(2); isz++) {
            int64_t ix = iRX*32+isx, iy = iRY*32+isy, iz = iRZ*32+isz;
            //Ellipsoid: (ix-x)^2/sax+(iy-y)^2/say+(iz-z)^2/saz <= 1
            if( (ix-o.x)*(ix-o.x)*say2*saz2 + (iy-o.y)*(iy-o.y)*sax2*saz2 + (iz-o.z)*(iz-o.z)*sax2*say2 <= sax2*say2*saz2 ) {
                tr->setTile(isx, isy, isz, palIdx);
            }
        }
    }
}
void ShapeBuilder::fillEllipsoid(Vec3<int64_t> o, Vec3<int64_t> axes, Tile t) { fillEllipsoid(o, axes.x, axes.y, axes.z, t); }
void ShapeBuilder::fillSphere(Vec3<int64_t> o, int64_t rad, Tile t) { fillEllipsoid(o, rad, rad, rad, t); }

void ShapeBuilder::fillBlob(Vec3<int64_t> o, int64_t aSize, int cSize, std::vector<Tile>& tileset)
{
    if(aSize<cSize) {
        nch::Log::warn(__PRETTY_FUNCTION__, "aSize must be >= cSize");
        return;
    }
    
    /* Build ellipsoid origins and ellipsoid sizes which will make up the structre. */
    //Create lists
    std::vector<Vec3<int64_t>> eOrigins;
    std::vector<Vec3<int64_t>> eSizes;
    for(int i = 0; i<8; i++) {
        Vec3<int64_t> offset(r(aSize-cSize+1), r(aSize-cSize+1), r(aSize-cSize+1));
        eOrigins.push_back(o+offset);
        eSizes.push_back(Vec3<int64_t>(r(cSize), r(cSize), r(cSize)));
    }

    //Halfway-in = quality 1
    //3/4ths-in >= quality 2
    //Everything >= quality 3
    for(int i = 0; i<eOrigins.size(); i++) {
        Vec3<int64_t> eo = eOrigins[i];
        Vec3<int64_t> es = eSizes[i];

        fillEllipsoid(eo, es, tileset[2]);
        fillEllipsoid(eo, es*(.75), tileset[1]);
        fillEllipsoid(eo, es*(.5), tileset[0]);
    }
}

void ShapeBuilder::fillTraces(Vec3<int64_t> o, int64_t aSize, int num, std::vector<Tile>& tileset)
{
    TileIterator ti(trm);
    ti.setBounds(o.x-aSize, o.y-aSize, o.z-aSize, o.x+aSize, o.y+aSize, o.z+aSize);

	for(TileRegion* tr = ti.peekRegion(); !ti.atEnd(); tr = ti.nextRegion()) {
        if(tr==nullptr) continue;
        int16_t pi0 = tr->addToPalette(tileset[0], true);
        int16_t pi1 = tr->addToPalette(tileset[1], true);
        int16_t pi2 = tr->addToPalette(tileset[2], true);
		int64_t iRX = ti.gir(0), iRY = ti.gir(1), iRZ = ti.gir(2);
        for(int64_t isx = ti.gbs(0); isx<=ti.ges(0); isx++)
        for(int64_t isy = ti.gbs(1); isy<=ti.ges(1); isy++)
        for(int64_t isz = ti.gbs(2); isz<=ti.ges(2); isz++) {
            int64_t ix = iRX*32+isx, iy = iRY*32+isy, iz = iRZ*32+isz;

            switch(r(9)) {
                case 0: tr->setTile(isx, isy, isz, pi0); break;
                case 1:
                case 2: tr->setTile(isx, isy, isz, pi1); break;
                case 3:
                case 4:
                case 5: tr->setTile(isx, isy, isz, pi2); break;
            }
        }
    }
}

/*

    aSize: [A]llowed size. Size of the area that the organic mass should take up. Represents a sidelength==(2*aSize) cube centered at (x, y, z).
    cSize: [C]omponent size. Relative size of each ellipsoid. Determines how large the tunnels are.
*/
Vec3<int64_t> ShapeBuilder::carveOrganicTunnel(Vec3<int64_t> o, int64_t aSize, int64_t cSize, Tile t)
{
    /* Preparations */
    //Maximum allowed bounds of this tunnel
    Box3<int64_t> tunnelBounds;
    tunnelBounds.c1 = {o.x-aSize/2, o.y-aSize/2, o.z-aSize/2};
    tunnelBounds.c2 = {o.x+aSize/2, o.y+aSize/2, o.z+aSize/2};

    /* Build ellipsoid origins and ellipsoid sizes which will make up the tunnel. */
    //Create lists
    std::vector<Vec3<int64_t>> eOrigins;
    std::vector<Vec3<int64_t>> eSizes;
    //Populate lists
    Vec3<int64_t> iPnt = o;
    int64_t minTaxiDist = cSize/3;
    for(int i = 0; i<16; i++) {
        //Stop once we are outside the tunnelBounds
        if(!tunnelBounds.contains(iPnt)) { break; }

        //Add a new ellipsoid origin position
        eOrigins.push_back(iPnt);
        eSizes.push_back({ cSize/2+r()%5, cSize/2+r()%5, cSize/2+r()%5 });

        //Find a new ellipsoid origin position that is sufficiently far away from (xyz0)
        Vec3<int64_t> delta;
        Vec3<int64_t> nextPnt;
        int64_t taxiDist;
        do {
            delta = { (r()%11-5), (r()%11-5), (r()%11-5) };
            nextPnt = { iPnt.x+delta.x, iPnt.y+delta.y, iPnt.z+delta.z };
            taxiDist = std::abs(nextPnt.x-o.x) + std::abs(nextPnt.y-o.y) + std::abs(nextPnt.z-o.z);
        } while(taxiDist<minTaxiDist);

        //New iPnt and new minimum taxi dist
        iPnt = nextPnt;
        minTaxiDist += (cSize/3);
    }
    //Limit size of all ellipsoids so that they don't go outside of the allowedArea
    for(int i = eOrigins.size()-1; i>=0; i--) {
        Vec3<int64_t> eo = eOrigins[i];
        Vec3<int64_t> es = eSizes[i];
        Box3<int64_t> ellipseBounds(eo.x-es.x, eo.y-es.y, eo.x-es.z, eo.x+es.x, eo.y+es.y, eo.x+es.z);
        if(!tunnelBounds.contains(ellipseBounds)) {
            eOrigins.erase(eOrigins.begin()+i);
            eSizes.erase(eSizes.begin()+i);
        }
    }
    
    /* Place ellipsoids created from the two lists */
    //Shrink bounds as much as possible
    Box3<int64_t> minbox = Box3<int64_t>(eOrigins[0]-eSizes[0], eOrigins[0]+eSizes[0]);
    Vec3 c2Min = eOrigins[0];
    for(int i = 0; i<eOrigins.size(); i++) {
        Vec3<int64_t> c1 = eOrigins[i]-eSizes[i];
        Vec3<int64_t> c2 = eOrigins[i]+eOrigins[i];

        if(c1.x<minbox.c1.x) minbox.c1.x = c1.x;
        if(c1.y<minbox.c1.y) minbox.c1.y = c1.y;
        if(c1.z<minbox.c1.z) minbox.c1.z = c1.z;
        if(c2.x>minbox.c2.x) minbox.c2.x = c2.x;
        if(c2.y>minbox.c2.y) minbox.c2.y = c2.y;
        if(c2.z>minbox.c2.z) minbox.c2.z = c2.z;
    }

    //Placement
    Vec3<int64_t> dP0 = minbox.c1;
    for(int i = 0; i<eOrigins.size(); i++) {
        fillEllipsoid(eOrigins[i]-dP0, eSizes[i], t);
    }

    Vec3<int64_t> finalSize(minbox.c2-dP0+Vec3<int64_t>(1, 1, 1));
    return finalSize;
}

int64_t ShapeBuilder::r() { return std::rand(); }
int64_t ShapeBuilder::r(int64_t mod) { return r()%mod; }