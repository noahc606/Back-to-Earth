#include "LevelSave.h"
#include <iostream>
#include <set>
#include <sstream>
#include "TileMap.h"
#include "Log.h"

const uint16_t LevelSave::ampSBits[] = {
        0b1000000000000000,
        0b0100000000000000,
        0b0010000000000000,
        0b0001000000000000,
        0b0000100000000000,
        0b0000010000000000,
        0b0000001000000000,
        0b0000000100000000,
        0b0000000010000000,
        0b0000000001000000,
        0b0000000000100000,
        0b0000000000010000,
        0b0000000000001000,
        0b0000000000000100,
        0b0000000000000010,
        0b0000000000000001 };

const uint8_t LevelSave::pows2[] = { 32, 16, 8, 4, 2, 1 };

LevelSave::LevelSave(){}

void LevelSave::init(FileHandler* parentFH)
{
    fileHandler = parentFH;
}

/**
    Return size bucket value [-1-10]. Used to determine where a TilePalette will be saved.
    0->-1, 1->0, 2->1, [3,4]->2, [5,8]->3, [9,16]->4, ..., [513,1024]->10.
*/
int LevelSave::getPaletteSizeBucket(int size)
{
    int res = -1;
    while(size!=0) {
        size /= 2;
        res++;
    }
    return res;
}

void LevelSave::trSave(FilePath path, long rX, long rY, long rZ, t_palette* palette, int16_t tiles[32][32][32])
{
    // Get save region location (coords in 3D grid where each cubic "cell" is 1024x1024x1024 tiles)
    TileMap::t_ll srX = rX*32;
    TileMap::t_ll srY = rY*32;
    TileMap::t_ll srZ = rZ*32;

    //Build save area map. A save area is a 16x16x4 area of tiles
    //std::map<std::tuple<int,int,int>, t_saveArea> saveAreaMap;

    //Part 1: Save palette data to file
    int fileState = fileHandler->cEditFile(path.get());
    for(int i = 0; i<palette->size(); i++) {
        TileType tt = palette->at(i);
        tt.getVal();
    }



    //Part 2: Save tile data to file
    fileState = fileHandler->cEditFile(path.get());
    switch(fileState) {
        case FileHandler::FAILED_ACCESS: {
            std::stringstream ss;
            ss << "Failed to save level at SRXYZ = (" << srX << ", " << srY << ", " << srZ << ")";
            Log::error(__PRETTY_FUNCTION__, ss.str());
        } break;
        case FileHandler::NEW: {

        } break;
        case FileHandler::EXISTING: {

        } break;
    }


    return;



    //fileHandler->pEditMappedFile(path);
    //fileHandler->pSeek()





    //fileHandler->pEditMappedFile(path);
    //fileHandler->pBuildLineMap();


    //Allocate a large string called 'line1' with a capacity of 100000
    std::string line1;
    const std::string::size_type new_capacity{ 100000u };
    line1.reserve( new_capacity );

    //Write to line1 in this format: "Region(<rX>,<rY>,<rZ>)
    line1 += "Region(";
    line1 += std::to_string(rX); line1 += ",";
    line1 += std::to_string(rY); line1 += ",";
    line1 += std::to_string(rZ);
    //Write to line1 in this format: ".palette={<TileType>,<TileType>,<...>}
    line1 += ").palette={";
    for(unsigned int i = 0; i<palette->size()-1; i++) {
        TileType tt = (*palette)[i];
        line1 += tt.toString();
        line1 += ",";
    }
    line1 += (*palette)[palette->size()-1].toString();
    line1 += "}, ";

    //8 characters = 3 tiles
    //1 tile = 16 bits, each character = 6bits
    int x = 0;
    int y = 0;
    int z = 0;
    uint8_t counter = 0;
    uint8_t bits = 0;
    char base64Char = 64;

    line1 += "Region(";
    line1 += rX; line1 += ",";
    line1 += rY; line1 += ",";
    line1 += rZ; line1+= ").data={";

    while(true) {
        if( (tiles[x][y][z]&ampSBits[bits])>0 ) {
            base64Char += pows2[counter];
        }

        counter++;
        if(counter==6) {
            line1 += base64Char;
            base64Char = 64;
            counter = 0;
        }

        bits++;
        if(bits==16) {
            bits = 0;
            x++;
            if(x>31) {
                x = 0;
                y++;
                if( y>31 ) {
                    line1 += base64Char;
                    base64Char = 64;
                    counter = 0;
                    y = 0;
                    z++;
                    if( z>31 ) {
                        z = 0; break;
                    } else {
                        line1 += ";";
                    }
                }
            }
        }
    }
    line1 += "}\n";

    int strSize = 100000;//line1.length();

    std::stringstream prefix;
    std::stringstream numSS; numSS << strSize; std::string num = numSS.str();
    //num must be less than 1000000!
    if(strSize>999999) num = "ERROR!";
    while( num.length()<6 ) {
        num = "0"+num;
    }
    prefix << "[" << num << "]! "; //Length of prefix will always be 10 (4 characters + 6 numerical chracters)

    fileHandler->write(prefix.str());
    fileHandler->write(line1);
    fileHandler->saveCloseFile();
}
