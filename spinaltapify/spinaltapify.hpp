//Spinal tapify library file
#include <libspotify/api.h>
#if defined(_WIN32_)
#include <boost/thread/detail/move.hpp>
#endif
#include <boost/thread/mutex.hpp>
#include "compat.h"

__declspec T_LIB_DIR extern sp_session * sp; // session object valid after calling initialize
__declspec T_LIB_DIR extern boost::mutex spMutexLock;

__declspec T_LIB_DIR int spty_initializeSpinalTapify(int argc, char * argv[]); // [0] ignored [1] username [2] password and set argc to 3

__declspec T_LIB_DIR void spty_deInitSpinalTapify();

// Media player related functions

__declspec T_LIB_DIR void spty_prevTrack(unsigned int num);

__declspec T_LIB_DIR void spty_chooseTrack(unsigned int num);

__declspec T_LIB_DIR void spty_skipTrack();

__declspec T_LIB_DIR void spty_stop();

__declspec T_LIB_DIR void spty_play();

__declspec T_LIB_DIR void spty_pause();

__declspec T_LIB_DIR void spty_unPause();

__declspec T_LIB_DIR void spty_resume();

__declspec T_LIB_DIR unsigned int spty_position();

__declspec T_LIB_DIR void spty_seek(unsigned int seek);

__declspec T_LIB_DIR unsigned int spty_currentTrack();

// Playlist related functions (spinal tapify maintains one long fifo buffer which wraps around, create your own playlists if you want a playlist longer than 300)

__declspec T_LIB_DIR bool spty_addTrack(sp_track * track);

__declspec T_LIB_DIR bool spty_addTrack(sp_track * track, unsigned int position);

__declspec T_LIB_DIR bool spty_removeTrack(unsigned int position);

__declspec T_LIB_DIR bool spty_removeTrack(sp_track * track);

__declspec T_LIB_DIR void spty_clearTracks();

__declspec T_LIB_DIR unsigned int spty_playListSize();

__declspec T_LIB_DIR sp_track ** spty_getPlayList();

__declspec T_LIB_DIR bool spty_setPlayList(unsigned int size, sp_track * tracks);
