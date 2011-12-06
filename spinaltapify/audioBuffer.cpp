#include "audioBuffer.hpp"
#include <malloc.h>
#include <memory.h>

// define framesize as 2048
// So that's 2048*1 2048 bytes
extern boost::mutex mSetup;
audioBuffer::audioBuffer(int bufferSize, int tempBufferSize)
{
	this->bufferSize = bufferSize;
	buf = new uint32[bufferSize];
	tempBuf = new uint32[tempBufferSize];
	writePos = 0;
	readPos = 0;
	occupiedSpace = 0;
	tempBufSize = tempBufferSize;
	live = false;
}

audioBuffer::~audioBuffer(){
	delete buf;
	delete tempBuf;
	bufferSize = 0;
	occupiedSpace = 0;
	writePos = 0;
	readPos = 0;
}

int audioBuffer::spaceFree(){
	return bufferSize - occupiedSpace;
}

int audioBuffer::spaceOccupied(){
	return occupiedSpace;
}

int audioBuffer::addToBuffer(const void * source2, int size){
	boost::lock_guard<boost::mutex> lock(mSetup);	
	uint32 * source = (uint32 *) source2;
	if (size > this->spaceFree()){
		DebugP( "Error buffer used incorrectly \n");
		PAUSE();
		return 0;
	}
	if (writePos + size >= bufferSize){
		int copyLoc = 0;
		memcpy ((void *) &buf[writePos], (void *) &source[copyLoc] , sizeof(uint32) * (bufferSize - writePos));
		copyLoc += bufferSize - writePos;
		memcpy ((void *) &buf[0], (void *) &source[copyLoc] , sizeof(uint32) * (size - (bufferSize - writePos)));
	}else{
		memcpy ((void *) &buf[writePos], (void *) source, sizeof(uint32) * size);
	}
	writePos += size;
	writePos = writePos % bufferSize;
	occupiedSpace += size;
	return size;
}

void * audioBuffer::readFromBuffer(int numInts){
	boost::lock_guard<boost::mutex> lock(mSetup);
	void * returnAddr;

	if (numInts > this->spaceOccupied()){
		DebugP("Error buffer used incorrectly \n");
		PAUSE();
		returnAddr = NULL;
		return returnAddr;
	}

	if (readPos + numInts < bufferSize){
		returnAddr = (void *) &buf[readPos];
	}else{
		int copyLoc = 0;
		memcpy ((void *) &tempBuf[copyLoc]  , (void *) &buf[readPos],  sizeof(uint32) * (bufferSize - readPos));
		copyLoc+= bufferSize- readPos;
		memcpy ((void *) &tempBuf[copyLoc], (void *) &buf[0],  sizeof(uint32) * (numInts - ( bufferSize - readPos)));
		returnAddr = (void *) &tempBuf[0];
	}
	return returnAddr;
}

int audioBuffer::getTempBufSize(){
	return tempBufSize;
}

int audioBuffer::advance(int numInts){
	boost::lock_guard<boost::mutex> lock(mSetup);
	if (numInts > this->spaceOccupied()){
		DebugP("Error buffer used incorrectly \n");
		PAUSE();
	}
	occupiedSpace -= numInts;
	readPos += numInts;
	readPos = readPos % bufferSize;
	return numInts;
}
void audioBuffer::emptyBuffer(){
	writePos = 0;
	readPos = 0;
	occupiedSpace = 0;
	live = false;
}



audioBuffer a(100, 10);
void blockTest(){
	for ( int i = 0;i < 10000; i ++){
		a.readFromBuffer(1);
	}
}



int testBuffer(){
	
	uint32 tar[3] = {0 , 1 ,2};
	for (int i =0 ; i < 3000; i++){
	uint32 repR[3] = {i, i+1, i+2};
	a.addToBuffer((uint32 *) &repR, 3);
	
	uint32 * aptr = (uint32 *) a.readFromBuffer(3);
	uint32 rep[3] = {aptr[0], aptr[1], aptr[2]};
	a.advance(3);
	for (int i = 0 ; i < 3; i ++){
		if (rep[i] != repR[i]){
			std::cout << "Problem";
		}
		std::cout << aptr[i] << " ";
	}
	}


	std::cout << "test two\n";
	
	while (a.spaceFree() > 0){
		static int i = 0;

		a.addToBuffer((void *)&i, 1);
		i++;

	}

	while (a.spaceOccupied() > 15){
		void * buf = a.readFromBuffer(5);
		a.advance(5);
		//printf(" %d %d \n", ((int *)buf)[0], ((int *) buf)[4]);
	}

	while (a.spaceFree() > 0){
		static int i = 0;

		a.addToBuffer((void *)&i, 1);
		i++;

	}

	boost::thread thr(blockTest);
	while (a.spaceOccupied() > 15){
		void * buf = a.readFromBuffer(5);
		a.advance(5);
		//printf(" %d %d \n", ((int *)buf)[0], ((int *) buf)[4]);
	}
	thr.join();
	return 0;
}

