#include "DataStream.h"
#include <iomanip>
#include <nch/cpp-utils/log.h>
#include "DebugScreen.h"

DataStream::DataStream()
{
    seekBit(0);
}

DataStream::~DataStream(){}

void DataStream::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::newGroup(ss, tabs, "DataStream");
		DebugScreen::indentLine(ss, tabs);
		ss << "seek(byte, bit)=(" << getSeekBytePos() << ", " << (int)getSeekBitOffset() << "); ";
		ss << "bytecell = " << std::setfill('0') << std::setw(2) << std::hex << (int)peekByteCell() << "; ";
		DebugScreen::newLine(ss);
		DebugScreen::indentLine(ss, tabs);
		std::queue<bool> bitcacheCopy = bitcache;
		ss << "bitcache = { ";
		while( !bitcacheCopy.empty() ) {
			ss << bitcacheCopy.front() << " ";
			bitcacheCopy.pop();
		}
		ss << "};";
		DebugScreen::newLine(ss);
		DebugScreen::indentLine(ss, tabs);
		
		ss << "bytestream={";
		for(int i = 0; i<bytestream.size(); i++) {
			ss << std::setfill('0') << std::setw(2) << std::hex << (int)peekByteCell();
			seekByteDelta(1);
		}
		ss << "};";
		
		seekByte(0);
		
		DebugScreen::newLine(ss);
	DebugScreen::endGroup(tabs);
}

uint64_t DataStream::getSeekBytePos() { return seekBytePos; }
uint64_t DataStream::getSeekBitPos() { return getSeekBytePos()*8+seekBitOffset; }
uint8_t DataStream::getSeekBitOffset() { return seekBitOffset; }

uint8_t DataStream::peekByteCell()
{
	if( bytestream.size()!=0 ) {
		return bytestream.at(seekBytePos);
	} else {
		nch::Log::warn(__PRETTY_FUNCTION__, "Bytestream is empty", "returning 0");
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
	
	//Return to 'oldBitSeekPos' position in data stream
	seekBit(oldBitSeekPos);

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
	uint64_t one = 0b1;
    for( uint8_t i = numBits; i>0; i-- ) {
        bitcache.push( data&(one<<(i-1)) );
	}

    popWriteBytes();
}
/**
 * 	This function will automatically put X bits toward the DataStream.
 * 	Example: data=0b110101011 -> call putXBits(0b11010101, 9)
 * 	WARNING: For values with leading zeros (ex: 0b0010101) the function treats those zeros as insignificant! In these cases, use putXBits(data, numBits) to specify exactly how many bits you want. 
 */
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
void DataStream::put64Bits(uint64_t data) { putXBits(data, 64); }

void DataStream::dumpBytestream(FileHandler* fh, uint64_t bytePos)
{
	if( bytePos>fh->getFileLength() ) {
		std::stringstream ss;
		ss << "Byte dump position (" << bytePos << ") cannot be larger than the file length (" << fh->getFileLength() << ")";
		nch::Log::warn(__PRETTY_FUNCTION__, ss.str());
		return;
	}
	
	fh->seekTo(bytePos);
	for( int i = 0; i<bytestream.size(); i++ ) {
		fh->writeByte( bytestream.at(i) );
	}
	
	clear();
}

void DataStream::dumpBytestream(FileHandler* fh)
{
	fh->seekToEnd();
	uint64_t bytePos = fh->tellPos();
	dumpBytestream(fh, bytePos);
}

void DataStream::clear()
{
	bool redundant = (bytestream.size()==0 && bitcache.size()==0);
	bytestream.clear();
	bool ignoredReturnValueLol = bitcache.empty();
	if(redundant) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Tried to clear DataStream which was already empty");
	}
	
	seekBitOffset = 0;
	seekBytePos = 0;
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
