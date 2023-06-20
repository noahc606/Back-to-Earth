#include "RegTexBuilder.h"
#include "DebugScreen.h"
#include <math.h>

RegTexBuilder::RegTexBuilder(Texture* tex, TileIterator& ti, int dstX, int dstY, int blitScalePx, TileType ttfc, int dZ)
{
    drawTypeA( tex, dstX, dstY, blitScalePx, ttfc, dZ );
    if(dZ==0) {
        detailDepth0Tiles(tex, ti, dstX, dstY, blitScalePx);
    }

    if( dZ>0 ) {
        detailDepthPTiles(tex, ti, dstX, dstY, blitScalePx, dZ);
    }

}

RegTexBuilder::~RegTexBuilder(){}

void RegTexBuilder::info(std::stringstream& ss, int& tabs, TileIterator& ti, int dZ)
{
    bool bl[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // "Block-light" tile locations
    infoBl(ti, bl, dZ);

    bool co[4] = { 0, 0, 0, 0 }; //Corners
    bool si[4] = { 0, 0, 0, 0 }; //Sides
    bool sh[4] = { 1, 1, 1, 1 };  //Shadows
    infoCoSiSh(bl, co, si, sh);


    DebugScreen::indentLine(ss, tabs);
    ss << "bl={" << bl[0]<<bl[1]<<bl[2]<<bl[3]<<bl[4]<<bl[5]<<bl[6]<<bl[7] << "}, ";
    ss << "co={" << co[0]<<co[1]<<co[2]<<co[3] << "}, ";
    ss << "si={" << si[0]<<si[1]<<si[2]<<si[3] << ", ";
    ss << "sh={" << sh[0]<<sh[1]<<sh[2]<<sh[3] << "}; ";
    DebugScreen::newLine(ss);


}

void RegTexBuilder::infoBl(TileIterator& ti, bool bl[8], int dZ)
{
    //'bl' = blocking
    //012
    //3 4
    //567
    bl[0] = ti.peekTrackedTile(-1, -1, dZ).isVisionBlocking();
    bl[1] = ti.peekTrackedTile(00, -1, dZ).isVisionBlocking();
    bl[2] = ti.peekTrackedTile(01, -1, dZ).isVisionBlocking();
    bl[3] = ti.peekTrackedTile(-1, 00, dZ).isVisionBlocking();
    bl[4] = ti.peekTrackedTile(01, 00, dZ).isVisionBlocking();
    bl[5] = ti.peekTrackedTile(-1, 01, dZ).isVisionBlocking();
    bl[6] = ti.peekTrackedTile(00, 01, dZ).isVisionBlocking();
    bl[7] = ti.peekTrackedTile(01, 01, dZ).isVisionBlocking();
}
void RegTexBuilder::infoBl(TileIterator& ti, bool bl[8]) { infoBl(ti, bl, 0); }

void RegTexBuilder::infoCoSi(bool bl[8], bool co[4], bool si[4])
{
    //Set corners
    if( !bl[0]&&bl[1]&&bl[3] ) co[0] = 1;
    if( !bl[2]&&bl[1]&&bl[4] ) co[1] = 1;
    if( !bl[5]&&bl[3]&&bl[6] ) co[2] = 1;
    if( !bl[7]&&bl[4]&&bl[6] ) co[3] = 1;

    //Set sides
    if( !bl[1] ) si[0] = 1;
    if( !bl[3] ) si[1] = 1;
    if( !bl[4] ) si[2] = 1;
    if( !bl[6] ) si[3] = 1;

    //Anywhere a side intersects the corner of the square, that corner = 1.
    if( si[0] ) { co[0] = 1; co[1] = 1; }
    if( si[1] ) { co[0] = 1; co[2] = 1; }
    if( si[2] ) { co[1] = 1; co[3] = 1; }
    if( si[3] ) { co[2] = 1; co[3] = 1; }
}

void RegTexBuilder::infoSh(bool co[4], bool si[4], bool sh[4])
{
    //Set shadows
    if( si[0] ) { sh[0] = 0; sh[1] = 0; }
    if( si[1] ) { sh[0] = 0; sh[2] = 0; }
    if( si[2] ) { sh[1] = 0; sh[3] = 0; }
    if( si[3] ) { sh[2] = 0; sh[3] = 0; }
    if( co[0] ) { sh[0] = 0; }
    if( co[1] ) { sh[1] = 0; }
    if( co[2] ) { sh[2] = 0; }
    if( co[3] ) { sh[3] = 0; }
}

void RegTexBuilder::infoCoSiSh(bool bl[8], bool co[4], bool si[4], bool sh[4])
{
    infoCoSi(bl, co, si);
    infoSh(co, si, sh);
}

void RegTexBuilder::detailDepth0Tiles(Texture* tex, TileIterator& ti, int dstX, int dstY, int blitScalePx)
{
    bool bl[8] = { false };
    infoBl(ti, bl);

    bool co[4] = { 0, 0, 0, 0 }; //Corners
    bool si[4] = { 0, 0, 0, 0 }; //Sides
    bool sh[4] = { 1, 1, 1, 1 };  //Shadows
    infoCoSiSh(bl, co, si, sh);

    //Get total # of shadows
    int shadows = sh[0]+sh[1]+sh[2]+sh[3];
    bool special2Shadow = false;

    if( shadows==2 ) {
        if( (sh[0]&&sh[1]) || (sh[1]&&sh[3]) || (sh[2]&&sh[3]) || (sh[0]&&sh[2]) ) {
            special2Shadow = true;
        }
    }

    int osx = 0; //Overlay source X
    int osy = 0; //Overlay source Y
    if( sh[0] ) osy+=2;
    if( sh[1] ) osy+=1;
    if( sh[2] ) osx+=2;
    if( sh[3] ) osx+=1;

    drawOverlay( tex, dstX, dstY, blitScalePx, osx, osy );

    //If shadows <= 1 || special2Shadow
    //Decides locations of the corner and side texture
    if( shadows==0 || shadows==1 || special2Shadow ) {

        /* Draw corner(s) */
        osx = 0;
        osy = 4;
        if( co[0] ) osy += 2;
        if( co[1] ) osy += 1;
        if( co[2] ) osx += 2;
        if( co[3] ) osx += 1;

        drawOverlay( tex, dstX, dstY, blitScalePx, osx, osy );

        /* Draw side(s) */
        osx = 4;
        osy = 4;
        if( si[0] ) osy += 2;
        if( si[1] ) osy += 1;
        if( si[2] ) osx += 2;
        if( si[3] ) osx += 1;

        drawOverlay( tex, dstX, dstY, blitScalePx, osx, osy );
    }

}

void RegTexBuilder::detailDepthPTiles(Texture* tex, TileIterator& ti, int dstX, int dstY, int blitScalePx, int dZ)
{
    bool bl[8] = { false };
    infoBl(ti, bl, dZ);

    bool co[4] = { 0, 0, 0, 0 }; //Corners
    bool si[4] = { 0, 0, 0, 0 }; //Sides
    infoCoSi(bl, co, si);

    int osx = 0; //Overlay source X
    int osy = 0; //Overlay source Y

    /* Draw corner(s) */
    osx = 0;
    osy = 8;
    if( co[0] ) osy += 2;
    if( co[1] ) osy += 1;
    if( co[2] ) osx += 2;
    if( co[3] ) osx += 1;

    drawOverlay( tex, dstX, dstY, blitScalePx, osx, osy );

    /* Draw side(s) */
    osx = 4;
    osy = 8;
    if( si[0] ) osy += 2;
    if( si[1] ) osy += 1;
    if( si[2] ) osx += 2;
    if( si[3] ) osx += 1;

    drawOverlay( tex, dstX, dstY, blitScalePx, osx, osy );
}

/**
    Draw a tile image with a given color and depending on depth, shade it.
*/
void RegTexBuilder::drawTypeA(Texture* tex, int dstX, int dstY, int blitScalePx, int srcX, int srcY, Color c, int dZ)
{
    tex->lock( dstX, dstY, blitScalePx, blitScalePx);
    tex->setColorMod(c.r, c.g, c.b);
    tex->blit(TextureLoader::WORLD_TILE_type_a, srcX, srcY, 32, 32 );

    int depth = dZ;
    if( depth!=0 ) {
        if(depth>6) {
            depth = 6;
        }

        //int sky[] = { 135, 206, 235 }; //Sky blue
        //int sky[] = { 135, 206, 250 }; //Light sky blue
        int sky[] = {  25,  25, 112 }; //Midnight blue
        //int sky[] = {   0,   0, 128 }; //Navy blue

        tex->rect( dstX, dstY, blitScalePx, blitScalePx, sky[0]/6, sky[1]/6, sky[2]/6, 40*(depth-1), SDL_BLENDMODE_BLEND);
    }
}
void RegTexBuilder::drawTypeA(Texture* tex, int dstX, int dstY, int blitScalePx, TileType tt, int depth)
{
    int srcX = 32*std::get<0>(tt.getTextureXY());
    int srcY = 32*std::get<1>(tt.getTextureXY());
    int r = std::get<0>(tt.getRGB());
    int g = std::get<1>(tt.getRGB());
    int b = std::get<2>(tt.getRGB());

    drawTypeA(tex, dstX, dstY, blitScalePx, srcX, srcY, Color(r, g, b), depth );
}
void RegTexBuilder::drawTypeA(Texture* tex, int dstX, int dstY, int blitScalePx, int srcX, int srcY)
{
    drawTypeA( tex, dstX, dstY, blitScalePx, srcX, srcY, Color(255, 255, 255), 0 );
}

void RegTexBuilder::drawOverlay(Texture* tex, int dstX, int dstY, int blitScalePx, int srcX, int srcY)
{
    int sx32 = (srcX*32);
    int sy32 = (srcY*32);

    tex->lock( dstX, dstY, blitScalePx, blitScalePx );
    tex->blit( TextureLoader::WORLD_TILE_OVERLAY_wall, sx32, sy32, 32, 32 );
}
