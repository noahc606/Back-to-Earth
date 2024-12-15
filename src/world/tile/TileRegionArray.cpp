#include "TileRegionArray.h"
#include <nch/cpp-utils/log.h>

TileRegionArray::TileRegionArray(size_t bitsize)
{
    TileRegionArray::bitsize = bitsize;

    /* Allocate array, set function pointers */
    switch(bitsize) {
        case 1:  { t1 = new Tiles1Bit();    atp = &TileRegionArray::at1; setp = &TileRegionArray::set1; } break;
        case 2:  { t2 = new Tiles2Bit();    atp = &TileRegionArray::at2; setp = &TileRegionArray::set2; } break;
        case 4:  { t4 = new Tiles4Bit();    atp = &TileRegionArray::at4; setp = &TileRegionArray::set4; } break;
        case 8:  { t8 = new Tiles8Bit();    atp = &TileRegionArray::at8; setp = &TileRegionArray::set8; } break;
        case 16: { tX = new Tiles16Bit();   atp = &TileRegionArray::atX; setp = &TileRegionArray::setX; } break;
        default: {
            nch::Log::errorv(__PRETTY_FUNCTION__, "Invalid bitsize (must be 1, 2, 4, 8, or 16)", "Failed to create a %d-bit TileRegionArray", bitsize);
            bitsize = 1;

            t1 = new Tiles1Bit();   atp = &TileRegionArray::at1; setp = &TileRegionArray::set1;
        } break;
    }
    
    reset();
}

TileRegionArray::~TileRegionArray()
{
    if(t1!=nullptr) delete t1;
    if(t2!=nullptr) delete t2;
    if(t4!=nullptr) delete t4;
    if(t8!=nullptr) delete t8;
    if(tX!=nullptr) delete tX;
}

void TileRegionArray::printInfoIndices()
{
	std::stringstream ss;
	for( int z = 0; z<32; z++ ) {
		ss << "Z Layer " << z << ":\n";
		for( int y = 0; y<32; y++ ) {
			ss << "y=" << y << ": { ";
			for( int x = 0; x<32; x++ ) {
				ss << at(x, y, z) << " ";
			}
			ss << "} ";
		}
		ss << "\n\n";
	}
	
	nch::Log::log(ss.str());
}



int16_t TileRegionArray::at1(int x, int y, int z) { return (t1->arr[x][y][z>>3]&(   0b1<<((z&0b111)   )))>>((z&0b111)   ); }
int16_t TileRegionArray::at2(int x, int y, int z) { return (t2->arr[x][y][z>>2]&(  0b11<<((z& 0b11)<<1)))>>((z& 0b11)<<1); }
int16_t TileRegionArray::at4(int x, int y, int z) { return (t4->arr[x][y][z>>1]&(0b1111<<((z&  0b1)<<2)))>>((z&  0b1)<<2); }
int16_t TileRegionArray::at8(int x, int y, int z) { return t8->arr[x][y][z]; }
int16_t TileRegionArray::atX(int x, int y, int z) { return tX->arr[x][y][z]; }

void TileRegionArray::set1(int x, int y, int z, int16_t val) { (t1->arr[x][y][z>>3] &= (0b11111111-(   0b1<<((z&0b111)   )))) += ((val&0b1   )<<((z&0b111)   )); }
void TileRegionArray::set2(int x, int y, int z, int16_t val) { (t2->arr[x][y][z>>2] &= (0b11111111-(  0b11<<((z& 0b11)<<1)))) += ((val&0b11  )<<((z& 0b11)<<1)); }
void TileRegionArray::set4(int x, int y, int z, int16_t val) { (t4->arr[x][y][z>>1] &= (0b11111111-(0b1111<<((z&  0b1)<<2)))) += ((val&0b1111)<<((z&  0b1)<<2)); }
void TileRegionArray::set8(int x, int y, int z, int16_t val) { t8->arr[x][y][z] = val; }
void TileRegionArray::setX(int x, int y, int z, int16_t val) { tX->arr[x][y][z] = val; }

int16_t TileRegionArray::at(int x, int y, int z) { return ((*this).*atp)(x, y, z); }
void TileRegionArray::set(int x, int y, int z, int16_t val) { ((*this).*setp)(x, y, z, val); }

void TileRegionArray::reset()
{
    for(int ix = 0; ix<32; ix++)
    for(int iy = 0; iy<32; iy++)
    for(int iz = 0; iz<32; iz++) {
        set(ix, iy, iz, 0);
    }
}

