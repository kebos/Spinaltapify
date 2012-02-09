//Spinal tapify library file


#ifndef __SPTY_MAIN_HEADER__
#define __SPTY_MAIN_HEADER__
#if defined(_WIN32)
#include "win32/libspotify/include/libspotify/api.h"
#else
#include <libspotify/api.h>
#endif
#include <boost/thread/mutex.hpp>
//#include "compat.h"

struct _spty_track_info {
	sp_link * link;
	sp_track * track;
	char strLink[40];
};

typedef struct _spty_track_info spty_track;

  extern sp_session * sp; // session object valid after calling initialize
  //extern boost::mutex spMutexLock;

  int spty_initializeSpinalTapify(int, char *); // [0] ignored [1] username [2] password and set argc to 3

  void spty_deInitSpinalTapify();

// Media player related functions

  void spty_prevTrack(unsigned int );

  void spty_chooseTrack(unsigned int );

  void spty_skipTrack();

  void spty_stop();

  void spty_play();

  void spty_pause();

  void spty_unPause();

  void spty_resume();

  unsigned int spty_position();

  void spty_seek(unsigned int );

  unsigned int spty_currentTrack();

// Playlist related functions (spinal tapify maintains one long fifo buffer which wraps around, create your own playlists if you want a playlist longer than 300)

  unsigned int spty_addTrack(sp_track * );

  bool spty_removeTrack(unsigned int );

  bool spty_removeTrack(sp_track * );

  void spty_clearTracks();

  unsigned int spty_playListSize();

  sp_track ** spty_getPlayList();

  bool spty_setPlayList(unsigned int , sp_track * );
#endif
