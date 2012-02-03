// TODO

/*
1. Mutex lock around spotify API calls.....
2. Make next track cause a stop!

*/

#include <iostream>
#include "common.hpp"
#include <portaudio.h>
#include "include/appkey.c"

#include <vector>
#include <string>

#include "portAudio.hpp"
#include "audioBuffer.hpp"


void inputThread();
sp_track *track;
sp_track *currentPlayingtrack;
audioBuffer abuf(44100, 44100);
audioBuffer abuf2(44100, 44100);
bool bAbuf = true;
boost::mutex spMutexLock;
extern PaStream *stream;
extern unsigned int samplesPlayed;
#define BUFFER_DEBUG_LOG
#define LOGIN_DEBUG


__declspec T_LIB_DIR sp_session * sp; // session object



boost::mutex callingAPI; // Need to start using this mutex as spotify api is not thread safe!!
boost::mutex pauseTheMainLoop; // We lock on this for the 'process events loop', unlocked when api asks us to call process events
boost::mutex logIn; // We lock on this till the login call back unlocks it


// Two mutexes to allow smooth addition of new tracks/transition to new tracks
extern boost::mutex mWaitForTrack; // This is called by getnexttrack, causes next track to sleep until a new track appears on the playlist
boost::mutex loadNextTrack; // Unlock this to cause the next track to be loaded



sp_track * getNextTrack();
void initPlayList();


static volatile bool serviceLoop = false;
static volatile bool loggedin = false;
static volatile bool loggingOut = false;
static volatile bool serviceLoopActive = true;

static sp_session_config spconfig;



static sp_session_callbacks spCallbacks;

static void __stdcall end_of_track(sp_session *session);

static int __stdcall audio_data(sp_session *session, const sp_audioformat *format, const void *frames, int num_spotify_frames);

static void __stdcall logged_in(sp_session *session, sp_error error){
	if (error == SP_ERROR_OK){
		loggedin = true;
		DebugPC("Log in successful\n");
	}else{
		DebugPC("Log in failed with %u\n", error);
		loggedin = false;
	}
	logIn.unlock();
}

static void __stdcall logged_out(sp_session *session){
	DebugPC("Logged out was called\n");
	loggedin = false;
	serviceLoopActive = false;
	pauseTheMainLoop.try_lock();
	pauseTheMainLoop.unlock();
}


static void __stdcall main_sp_thread(sp_session *session){
	serviceLoop = true;
	pauseTheMainLoop.unlock();
}



static int initCallbacks(){
	memset(&spCallbacks, 0, sizeof(spCallbacks));
	spCallbacks.logged_in = &logged_in;
	spCallbacks.logged_out = &logged_out;
	spCallbacks.notify_main_thread = &main_sp_thread;
	spCallbacks.music_delivery = &audio_data;
	spCallbacks.end_of_track = &end_of_track;
	return 1;
}


static void mainServiceThread(){
	static int nextTimeOut = 0;
	pauseTheMainLoop.lock();
	while (serviceLoopActive) {
		sp_session_process_events(sp , &nextTimeOut);
		DebugPC("Processed process events %d\n", nextTimeOut);
		pauseTheMainLoop.lock();
		// Should recheck before timeout! Possibly sort this out tonight...
	}
	pauseTheMainLoop.unlock();
}


//UNUSED
static void __stdcall get_audio_buffer_stats(sp_session *session, sp_audio_buffer_stats *stats){
	stats->samples = 0;
	stats->stutter = 0;
	DebugPC("Got audio buffer stats\n");
}

//UNUSED
static void __stdcall start_playback(sp_session *session){
	DebugPC("Start playing\n");
	return;
}


static int __stdcall audio_data(sp_session *session, const sp_audioformat *format, const void *frames, int num_spotify_frames){
	audioBuffer  * currBuffer = &abuf;
	if (!bAbuf) {
		//printf("Add to abuf 2\n");
		currBuffer = &abuf2;
		currBuffer->live = true;
		abuf.live = false;
	}else{
		//printf("Add to abuf 1\n");
		currBuffer->live = true;
		abuf2.live = false;
	}

	int buffer_space_free = currBuffer->spaceFree();
	int bytesRead = 0;
	if (buffer_space_free > num_spotify_frames){
		bytesRead = num_spotify_frames;
		currBuffer->addToBuffer(frames, bytesRead);
	}else if (buffer_space_free > 0){
		bytesRead = buffer_space_free;
		currBuffer->addToBuffer(frames, bytesRead);
		// copy it
	}else{
		bytesRead = 0;
	}
	return bytesRead;
}

void nextTrackUnlock(){
	if (loadNextTrack.try_lock()){
		loadNextTrack.unlock();
	}else{
		loadNextTrack.unlock();
	}
}


static void __stdcall end_of_track(sp_session *session){
	//Load the next track
	DebugPC("Track finished!\n");
	nextTrackUnlock();
}






static bool bSkipTrack = false; // Use this to decide whether to cut between tracks or to gapless into the same buffer

void spty_skipTrack(){
	bSkipTrack= true;
	nextTrackUnlock();
}

static void nextTrackFunc(){
	
	while(serviceLoopActive){
		currentPlayingtrack = track;
		track = NULL;
	
		while(track == NULL)
		{
			loadNextTrack.lock();
			track = getNextTrack();
		}
		if (bSkipTrack){
			if (bPortAudioActive) closePortAudio();
		}

		sp_session_player_unload(sp);
		samplesPlayed = 0;
		if (!bPortAudioActive) initPortAudio();
		sp_error err = sp_session_player_load(sp, track);

		int retryCount = 2;
		while (SP_ERROR_OK != err && (--retryCount)){
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
			DebugP("Problem: " << sp_error_message(err) << "\n");
			err = sp_session_player_load(sp, track);
		}
		if (SP_ERROR_OK != err) spty_skipTrack(); // Still hasn't loaded

		if (bSkipTrack){
			bSkipTrack = false;
			bAbuf ? bAbuf = false : bAbuf = true;
		}
		sp_session_player_play(sp, true);
	}
}





bool backTrack(unsigned int num);

void spty_prevTrack(unsigned int num){
	if (backTrack(num))spty_skipTrack();
}


void spty_chooseTrack(unsigned int num){
	extern unsigned int currentTrack;	
	currentTrack = num;
	// Last track issue
	spty_skipTrack();
}


void spty_stop(){
	if (bPortAudioActive) closePortAudio();
	sp_session_player_unload(sp);
	samplesPlayed = 0;
}

void spty_play(){
	backTrack(1);
	spty_skipTrack();

	//nextTrackUnlock();
	//if (!bPortAudioActive) initPortAudio();
}

void spty_pause(){
	if (bPortAudioActive) closePortAudio();
}

void spty_unPause(){
	if (!bPortAudioActive) initPortAudio();
}

void spty_seek(unsigned int seek){
	bAbuf ? bAbuf = false : bAbuf = true;
	sp_session_player_seek(sp, seek);
	samplesPlayed = (seek/1000)*44100;
}

unsigned int spty_position(){
	return samplesPlayed/44100;
}

boost::thread gtMainThreadProcess;
boost::thread gtQuitThread;
static int initSpinalTapify(char * user, char * pass){
	static bool oneTimeInit = true;
	spconfig.api_version = SPOTIFY_API_VERSION;
	spconfig.cache_location = "tmp";
	spconfig.settings_location = "tmp";
	spconfig.application_key = g_appkey;
	spconfig.application_key_size = g_appkey_size;
	spconfig.user_agent = "spinal-tapify";
	spconfig.callbacks = NULL;

	int nextTimeOut = 0;
	sp = NULL;
	sp_error err;

	if (oneTimeInit){
	oneTimeInit = false;
	}

	//initPlayList();
	initCallbacks();
	pauseTheMainLoop.lock();
	

	spconfig.callbacks = &spCallbacks;
	err = sp_session_create(&spconfig, &sp);
	DebugP("sp is " << sp << std::endl);
	if (SP_ERROR_OK != err) {
		DebugPC(" Bad sesssion !!!!\n");
		//std::cout << "Unable to create session: " << sp_error_message(err) << "\n";
		return 1;
	}

	static char * username = user;
	static char * password = pass;
	printf("Calling with %s %s\n", user, pass);

	// Make login check for success

	sp_session_login(sp, username, password, false);
	
	serviceLoopActive = true;
	gtMainThreadProcess = boost::thread(mainServiceThread);
	printf("Waiting on login to complete\n");
	logIn.lock();
	printf("Hit second mutex!\n");
	logIn.lock(); 
	if (!loggedin){
		DebugPC( "Login mutex was unlocked but we are not actually logged in \n");
	//	return 1;
	}
	if (!loggedin){
		DebugPC("try to close gracefully\n");
		//sp_session_logout(sp);
		serviceLoopActive = false;
		
		gtMainThreadProcess.join();
		pauseTheMainLoop.try_lock();
		pauseTheMainLoop.unlock();
		sp_session_release(sp);
		DebugPC("Closed gracefully\n");
		return 1;
	}
	
	boost::thread nextTrackThread(nextTrackFunc);

	return 0;
}

void spty_deInitSpinalTapify(){
	//cleanup:
	if (bPortAudioActive) closePortAudio();
	sp_session_logout(sp);
	DebugPC("Wait for logout");
	gtMainThreadProcess.join();
	if (bPortAudioActive) closePortAudio();
	sp_session_logout(sp);
	DebugPC("Wait for logout");
}
extern unsigned int currentTrack;
unsigned int spty_currentTrack(){
	return currentTrack;
}

	void clearTracks();

void spty_clearTracks(){
	clearTracks();
	spty_skipTrack();
	spty_stop();
}



void startConsoleUI(){
	gtQuitThread = boost::thread(inputThread); 
	gtQuitThread.join();
}

 
int spty_initializeSpinalTapify(int argc, char * argv[]){

	if (!initSpinalTapify(argv[1], argv[2])){
		DebugPC("Logged in\n");
		if (initPortAudio()){
			return 1;
		}
	return 0;
	}else{
		DebugPC("Failed to login\n");
		return 1;
	}
}

extern int stopMongoose(void);
extern int startMongoose(void);

int main(int argc, char * argv[])
{
	attemp2:
	if (!spty_initializeSpinalTapify(argc, argv)){
	startMongoose();
	startConsoleUI();
	spty_deInitSpinalTapify();
	}else{
		DebugPC("Failed to login\n");
		PAUSE();
		goto attemp2;
	}
	stopMongoose();
	DebugPC("spinaltapify finished\n");
	PAUSE();
	return 0;
}
