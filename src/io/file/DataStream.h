#pragma once
#include <cstdint>
#include <queue>
#include <sstream>
#include "FileHandler.h"

class DataStream
{
public:

    /**/
    DataStream();
    virtual ~DataStream();
    /**/
    void info(std::stringstream& ss, int& tabs);
    void logInfo();
    uint64_t getSeekBytePos();
    uint64_t getSeekBitPos();
    uint8_t getSeekBitOffset();
    uint8_t peekByteCell();
    uint8_t peekHexDigitCell();
    uint8_t peekByte();
    uint64_t peekXBits(uint8_t numBits);
    bool peekBit();


    /**/
    void seekByte(uint64_t byteIndex);
    void seekByteDelta(uint64_t bytesDelta);
    void seekHexDigit(uint64_t hexDigIndex);
    void seekHexDigitDelta(uint64_t hexDigsDelta);
    void seekBit(uint64_t bitIndex);
    void seekBitDelta(uint64_t bitsDelta);
    void putXBits(uint64_t data, uint8_t numBits);
    void putXBits(uint64_t data);
    void putBit(bool bit);
    void put5Bits(uint64_t data);
    void putByte(uint64_t byte);
    void put64Bits(uint64_t byte);
    void dumpBytestream(FileHandler* fh);
    void close();
    /**/

protected:

private:

    void popWriteBytes();

    std::queue<bool> bitcache;          //Hold bits that we will add to collection later (we only hold a collection of whole bytes at any given time)
    std::vector<uint8_t> bytestream;    //Hold collection of bytes (uint8_t's)

    uint8_t seekBitOffset = 0;
    uint64_t seekBytePos = 0;
};
