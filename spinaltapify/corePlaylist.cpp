#include "common.hpp"


sp_track * gsPLTracks[300];
unsigned int uiQTracks = 0;
unsigned int currentTrack = 0;
boost::mutex mWaitForTrack;


void clearTracks()
{
	currentTrack = 0;
	uiQTracks = 0;
	memset(gsPLTracks, 0, sizeof(gsPLTracks));
	
}

unsigned int spty_addTrack(sp_track * track){
	sp_track_add_ref(track);
	DebugPC("Added a track %d tracks\n", uiQTracks+1);
	gsPLTracks[uiQTracks] = track;
	uiQTracks++;
	if (mWaitForTrack.try_lock()){
		mWaitForTrack.unlock();
	}else{
		mWaitForTrack.unlock();
	}
	return uiQTracks-1;
}

bool backTrack(unsigned int num){
	unsigned int newPos = currentTrack-num;
	DebugPC("New pos %d %d of %d\n",newPos,  currentTrack, uiQTracks);
	if (num <= currentTrack){
		currentTrack = newPos;
		DebugPC("New pos %d of %d\n", currentTrack, uiQTracks);	
		
	}else if (num == 2 && currentTrack == 1) {
		currentTrack = 0;
	}else{
		return false;
	}


	if (mWaitForTrack.try_lock()){
		mWaitForTrack.unlock();
	}else{
		mWaitForTrack.unlock();
	}
	return true;
}



sp_track * getNextTrack(){
	DebugPC("Waiting for next track!\n");
	while (uiQTracks == currentTrack){
		DebugPC("Wait here!\n");
		mWaitForTrack.lock();
		DebugPC("Unlocked %d of %d\n", currentTrack, uiQTracks);
	}
	DebugPC("Playing track from playlist position %d\n", currentTrack);
	return gsPLTracks[currentTrack++];
}

bool spty_removeTrack(unsigned int position){
	return true;
}

bool spty_removeTrack(sp_track * track){
	return true;
}

void initPlayList(){
	mWaitForTrack.lock();
}


