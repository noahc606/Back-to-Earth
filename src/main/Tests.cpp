#include "Tests.h"
#include "CampaignCreation.h"

Tests::Tests(){}

void assetList(FileHandler* fileHandler)
{
    std::vector<std::string> contents = fileHandler->listDirContents("resources", false, true);
    for(std::string s : contents) {
        if(!fileHandler->dirExists(s)) {
            printf("backtoearth/%s\n", s.c_str());
        }
    }
}

void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;

    assetList(fileHandler);
    al = sdlHandler->getAudioLoader();

    //MissionHolder mh;
    //mh.init("missiontests");
    mh.init("missiontests");



}

Tests::~Tests(){}

/**/
void Tests::draw()
{
    std::stringstream ss;
    ss << "To get back to the main screen:\n";
    ss << "1) [RSHIFT] + [F12] to open console.\n";
    ss << "2) Type in \"gs 0\"\n";
    ss << "3) Press [ENTER].";
    TextOld::draw(sdlHandler, ss.str(), sdlHandler->getWidth()/2-180, sdlHandler->getHeight()/2-36, 2);

    double scale = 2;
    
    mh.draw(sdlHandler, 0, 0);

    //tex.setDrawScale(scale);
    //tex.setDrawPos(sdlHandler->getWidth()/2-tex.getTexWidth()*scale/2, sdlHandler->getHeight()/2-tex.getTexHeight()*scale/2);
}

void Tests::tick(){}

/**/

void Tests::putInfo(std::stringstream& ss, int& tabs)
{
	ss << "Test...\n";
}

/*
void test1()
{
    //SpriteSheet ss;
    //ColorPalette cpal; cpal.init(cpal.DEFAULT_PLAYER);

    //SpriteSheetBuilder ssb(sh);

    //ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

    stex.init(sdlHandler);
    stex.setTexDimensions(1024, 1024);

    SpriteSheet ss;
    ss.init(sdlHandler, TextureLoader::WORLD_TILE_type_a);

    stex.lock(0, 0, 512, 512);
    stex.fill(127, 127, 255);
    stex.update();
    Timer tim1("test1", false);
    {
        stex.setColorMod(255, 0, 0);
        stex.setBlendMode(SDL_BLENDMODE_MOD);
        stex.lock(128, 128, 128, 128);
        stex.sblit(&ss, 0, 0, 512, 512);
        stex.update();
    }

    tim1.debugElapsedTimeMS();


    ttex.init(sdlHandler, 1024, 1024);
    ttex.lock(0, 0, 512, 512);
    ttex.setColorMod(127, 127, 255);
    ttex.fill();
    Timer tim2("test2", false);
    for( int i = 0; i<1; i++ ) {
        ttex.setColorMod(255, 0, 0);
        ttex.setBlendMode(SDL_BLENDMODE_MOD);
        ttex.lock(128, 128, 128, 128);
        ttex.blit(TextureLoader::WORLD_TILE_type_a, 0, 0, 512, 512);
    }
    tim2.debugElapsedTimeMS();

    //stex.lock(10, 10, 32, 32);
    //stex.unlock();
    //stex.update();
    //stex.blit(TextureLoader::WORLD_TILE_type_a, 64, 64);
    //stex.unlock();

    std::string trPath = "saved\\games\\tutorial\\tr";

    Timer fw("file writing");

    TileRegion treg;
    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            for( int z = 0; z<32; z++ ) {
                treg.setTile(x, y, z, x*1024+y*32+z);
            }
        }
    }

    treg.save(sdlHandler, fileHandler, false);
}
*/
