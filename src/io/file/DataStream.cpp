#include "DataStream.h"
#include <iomanip>
#include "DebugScreen.h"
#include "Log.h"

DataStream::DataStream()
{
    seekBit(0);
}

DataStream::~DataStream(){}

void DataStream::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::newGroup(ss, tabs, "DataStream");
        DebugScreen::indentLine(ss, tabs);
        ss << "seek(byte, bit)=(" << std::hex << getSeekBytePos() << ", " << (int)getSeekBitOffset() << "); ";
        ss << "bytecell = " << std::setw(2) << std::hex << (int)peekByteCell() << "; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        std::queue<bool> bitcacheCopy = bitcache;
        ss << "bitcache = { ";
        while( !bitcacheCopy.empty() ) {
            ss << bitcacheCopy.front() << " ";
            bitcacheCopy.pop();
        }
        ss << "}";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);
}

void DataStream::logInfo()
{
    std::stringstream ss; int tabs = 0;
    info(ss, tabs);
    std::cout << ss.str() << "\n";
}


uint64_t DataStream::getSeekBytePos() { return seekBytePos; }
uint64_t DataStream::getSeekBitPos() { return getSeekBytePos()*8+seekBitOffset; }
uint8_t DataStream::getSeekBitOffset() { return seekBitOffset; }

uint8_t DataStream::peekByteCell()
{
	if( bytestream.size()!=0 ) {
		return bytestream.at(seekBytePos);
	} else {
		Log::warn(__PRETTY_FUNCTION__, "Bytestream is empty", "returning 0");
		return 0;
	}
}
uint8_t DataStream::peekHexDigitCell()
{
    // SBO is in [0, 3] (first half of byte) -> return byte shifted by 4 to the right
    if( seekBitOffset<4 ) {
        return peekByteCell()>>4;
    }
    // SBO in second half of byte -> return last 4 bits of byte
    return peekByteCell()&0b1111;
}

bool DataStream::peekBit()
{
    return ( (peekByteCell()>>(7-seekBitOffset) )&0b1 );
}
uint64_t DataStream::peekXBits(uint8_t numBits)
{
    uint64_t oldBitSeekPos = getSeekBitPos();

    //Start 'numBits' bits forward, build 'res' by going backwards 'numBits' amount of times
    uint64_t res = 0;
    for( uint64_t i = oldBitSeekPos; i<oldBitSeekPos+numBits; i++ ) {
        seekBit(i);             //Go to new bit (backwards)
        res = res << 1;               //Leftshift
        res += peekBit();       //Add this bit to the end
    }

    //Return result
    return res;
}


void DataStream::seekByte(uint64_t byteIndex) { seekBytePos = byteIndex; }
void DataStream::seekByteDelta(uint64_t bytesDelta) { seekByte(seekBytePos+bytesDelta); }

void DataStream::seekHexDigit(uint64_t hexDigIndex)
{
    seekByte(hexDigIndex/2);
    seekBitOffset = (hexDigIndex%2)*4;
}
void DataStream::seekHexDigitDelta(uint64_t hexDigsDelta)
{
    seekBitDelta(hexDigsDelta*4);
}
void DataStream::seekBit(uint64_t bitIndex)
{
    seekByte(bitIndex/8);
    seekBitOffset = bitIndex%8;
}
void DataStream::seekBitDelta(uint64_t bitsDelta)
{
    uint64_t newBitSeekPos = getSeekBytePos()*8+seekBitOffset+bitsDelta;
    seekByte(newBitSeekPos/8);
    seekBitOffset = newBitSeekPos%8;
}

void DataStream::putXBits(uint64_t data, uint8_t numBits)
{
    for( uint8_t i = numBits; i>0; i-- ) {
        bitcache.push( data&(0b1<<(i-1)) );
    }

    popWriteBytes();
}
void DataStream::putXBits(uint64_t data)
{
    //Find the number of significant bits in 'data'
    uint64_t leadingOne = 0b1;
	uint8_t sigBits = 0;
    for( uint8_t i = 0; i<64; i++ ) {
		if( (data&(leadingOne<<i))==(leadingOne<<i) ) {
            sigBits = i+1;
        }
    }

    //Write significant bits to file
    putXBits(data, sigBits);
}
void DataStream::putBit(bool bit) { putXBits(bit, 1); }
void DataStream::put5Bits(uint64_t data) { putXBits(data, 5); }
void DataStream::putByte(uint64_t byte) { putXBits(byte, 8); }


void DataStream::dumpBytestream(FileHandler* fh)
{
    for( int i = 0; i<bytestream.size(); i++ ) {
        fh->writeByte( bytestream.at(i) );
    }

    bytestream.clear();
}

/**
    Fill the last byte with '0's
*/
void DataStream::close()
{
    while( bitcache.size()>0 && bitcache.size()<8 ) {
        bitcache.push(0);
    }
    popWriteBytes();
}

void DataStream::popWriteBytes()
{
    while( bitcache.size()>=8 ) {
        uint8_t newByte = 0;
        for( int8_t i = 7; i>=0; i-- ) {
            newByte += (bitcache.front()<<i);
            bitcache.pop();
        }
        bytestream.push_back(newByte);
    }
}
