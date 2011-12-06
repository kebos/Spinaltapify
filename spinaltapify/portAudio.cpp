#include "common.hpp"
#include "portAudio.hpp"
#include "audioBuffer.hpp"

PaStream * stream;
bool bPortAudioActive = false;
boost::mutex mSetup;
static bool bFirstRun = true;
extern audioBuffer abuf;
unsigned int samplesPlayed = 0;
extern audioBuffer abuf2;
int PaCallBack( const void *input,
                                      void *output,
                                      unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo* timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData ) {
	static audioBuffer * pPrevBuffer = &abuf;
	audioBuffer * currBuffer = &abuf;
	if (abuf.live){
		if (pPrevBuffer != currBuffer){
			// Do some stuff to flush abuf2
			DebugP("Switched buffers\n");
			abuf2.emptyBuffer();
			pPrevBuffer = currBuffer;
		}
	}else if (abuf2.live){
		currBuffer = &abuf2;
		if (pPrevBuffer != currBuffer){
			DebugP("Switched buffers\n");
			// Do some stuff to flush abuf
			abuf.emptyBuffer();
			pPrevBuffer = currBuffer;
		}
	}else{
		//return 0;
	}
	//printf("from buffer %d \n", abuf.live ? 1 : 2);
	int buffer_space_occ = currBuffer->spaceOccupied();
	int pa_read_capacity = frameCount;
	int copyDataSize = 0;
	if (buffer_space_occ == 0){
		//return 0;
	}
	if (pa_read_capacity > buffer_space_occ){
		//printf ("Num read %d\n", buffer_space_occ);
		int tempBufSize = currBuffer->getTempBufSize();
		if (buffer_space_occ > tempBufSize){
			copyDataSize = tempBufSize;
		}else{
			copyDataSize = buffer_space_occ;
		}
		//return num_frames;
	}else if (pa_read_capacity > 0){
		int tempBufSize = currBuffer->getTempBufSize();
		//printf ("Num read %d\n", pa_read_capacity);
		if (pa_read_capacity > tempBufSize){
			copyDataSize = tempBufSize;
		}else{
			copyDataSize = pa_read_capacity;
		}
	}else{
		copyDataSize = 0;
	}
	// copy data here
	void * source = currBuffer->readFromBuffer(copyDataSize);
	memcpy(output, source, copyDataSize * sizeof(int));
	samplesPlayed += copyDataSize;
	//std::cout << samplesPlayed/44100 << " played so far\n";
	if (copyDataSize < pa_read_capacity){
		//std::cout << "copy some extra\n";
		memset((void *)((int *)output+copyDataSize), 0, sizeof(int) * (frameCount - copyDataSize));
	}
	currBuffer->advance(copyDataSize);
	//std::cout << "Call back" << frameCount << "" << copyDataSize << "\n";
	return 0;
}


int initPortAudio(){
	PaStreamParameters outputParameters;
    	PaError err;
	boost::lock_guard<boost::mutex> l(mSetup);
	
	if (bFirstRun) {
		bFirstRun = false;
	}



	abuf.emptyBuffer();
	abuf2.emptyBuffer();

	err = Pa_Initialize();
	if( err != paNoError ){
		DebugP("Got a problem\n");
	}


	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      DebugP("No device\n");
	  exit(0);
    }
    outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paInt16; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
	#define SAMPLE_RATE         (44100)
	#define FRAMES_PER_BUFFER   (2048)

	err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              0,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              PaCallBack, /* no callback, use blocking API */
              NULL ); /* no callback, so no callback userData */
	Pa_StartStream(stream);
	bPortAudioActive = true;
	return 0;
}



int closePortAudio(){
	boost::lock_guard<boost::mutex> l(mSetup);
	DebugP("Clear buffers\n");
	bPortAudioActive = false;
	Pa_CloseStream(stream);
	Pa_Terminate();
	abuf.emptyBuffer();
	abuf2.emptyBuffer();
	return 0;
}
