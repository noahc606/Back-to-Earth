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




/*
void renderSkydomeFisheye(const Vec3F& sunDir, Texture& tex) 
{ 
    Atmosphere atmosphere(sunDir); 
 
    const unsigned width = tex.getTexWidth(), height = tex.getTexHeight();


    Vec3F* image = new Vec3F[width * heigh;t];
    Vec3F* p = image; 
    memset(image, 0x0, sizeof(Vec3F) * width * height); 
    for (unsigned j = 0; j < height; ++j) { 
        float y = 2.f * (j + 0.5f) / float(height - 1) - 1.f; 
        for (unsigned i = 0; i < width; ++i, ++p) {
            float x = 2.f * (i + 0.5f) / float(width - 1) - 1.f; 
            float z2 = x*x + y*y; 
            if (z2 <= 1) { 
                float phi = std::atan2(y, x); 
                float theta = std::acos(1 - z2); 
                Vec3F dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)); 
                // 1 meter above sea level
                *p = atmosphere.computeIncidentLight(Vec3F(0, atmosphere.earthRadius+1, 0), dir, 0, kInfinity); 
            } 
        }
        fprintf(stderr, "\b\b\b\b\%3d%c", (int)(100 * j / (width - 1)), '%'); 
    }

    p = image;

    float powval = 1.0f / 2.2f;
    float tone = 0.38317f;

    for (unsigned j = 0; j < height; ++j) { 
        for (unsigned i = 0; i < width; ++i, ++p) { 
            // Apply tone mapping function
            p->x = p->x<1.413f ? pow(p->x*tone, powval) : 1.0f-exp(-p->x); //Red [0-1]
            p->y = p->y<1.413f ? pow(p->y*tone, powval) : 1.0f-exp(-p->y); //Green [0-1]
            p->z = p->z<1.413f ? pow(p->z*tone, powval) : 1.0f-exp(-p->z); //Blue [0-1]


            uint8_t r = (unsigned char)(std::min(1.f, p->x)*255);
            uint8_t g = (unsigned char)(std::min(1.f, p->y)*255);
            uint8_t b = (unsigned char)(std::min(1.f, p->z)*255);
            tex.pixel(i, j, r, g, b);
        } 
    } 
    
    delete[] image; 
}*/

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t SuperFastHash (const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;

    assetList(fileHandler);
    al = sdlHandler->getAudioLoader();

    int64_t seed = 0xFFFFFFFFFFFFFFFF;

    char* data = (char*)malloc(sizeof(char)*16 + sizeof(int64_t));
    for(int i = 0; i<16; i++) {
        data[i] = 0b00000000;
    }

    uint64_t kings = 0xffffffffffffffffULL; 
    kings = kings << 4;
    //std::cout << kings << "\n";

    printf("%" PRIx64 "\n", kings);
    
    uint32_t x = SuperFastHash(data, 24);
    delete[] data;

    printf("hashed: %" PRIu32 "\n", x);
}

Tests::~Tests(){}

/**/

void drawlevel()
{
    /*
    counter++;
	
	SDL_Rect dst;
	dst.w = 32*6;
	dst.h = dst.w*4;
	dst.x = sdlHandler->getWidth()/2-dst.w/2;
	dst.y = sdlHandler->getHeight()/2-dst.h/2;
	
	tex.draw(&lvlImgSrc, &dst);
	
	int mw = controls->getMouseWheel();
	if( mw!=0 ) {
		
		if(mw<0) {
			lvlImgSrc.y += 32;
		}
		if (mw>0) {
			lvlImgSrc.y -= 32;
		}
		
		if(lvlImgSrc.y<0) {
			lvlImgSrc.y = 0;
		}
		
		controls->resetWheel(__PRETTY_FUNCTION__);
	}

    //sdlHandler->renderCopy(3, &dst, &dst);
    */
	

    for(int i = 0; i<10; i++) {
        //stex.lock(rx*32, ry*32, 32, 32);
        //stex.
    }

    //test123.draw();
    //stex.draw();
    //ttex.draw();

    /*

    test123.lock(200, 200, 40, 200);
    test123.blit(TextureLoader::GUI_FONT_robot, 0, 0);
    test123.draw();*/
}

void Tests::draw()
{
    std::stringstream ss;
    ss << "To get back to the main screen:\n";
    ss << "1) [RSHIFT] + [F12] to open console.\n";
    ss << "2) Type in \"gs 0\"\n";
    ss << "3) Press [ENTER].";
    TextOld::draw(sdlHandler, ss.str(), sdlHandler->getWidth()/2-180, sdlHandler->getHeight()/2-36, 2);

    //tex.draw();
    SDL_RenderCopy(sdlHandler->getRenderer(), tex.getSDLTexture(), NULL, NULL);
    
    //tex.draw();

    double scale = 2;
    
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
