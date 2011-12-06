#include "common.hpp"

typedef unsigned int uint32;
int testBuffer();

class audioBuffer{
public:
	audioBuffer(int bufferSize, int tempBufferSize);
	int spaceFree();
	int spaceOccupied();
	int addToBuffer(const void * source, int numBytes);
	~audioBuffer();
	void * readFromBuffer(int numInts);
	int advance(int numInts);
	int writePos;
	int readPos;
	bool live;
	int getTempBufSize();
	void emptyBuffer();
private:
	uint32 * buf;
	uint32 * tempBuf;
	boost::try_mutex bufferChange;
	int bufferSize;
	int occupiedSpace;
	int tempBufSize;
};
