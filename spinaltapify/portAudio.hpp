#include "common.hpp"
#if defined(_WIN32)
#include "win32/portaudio/include/portaudio.h"
#else
#include <portaudio.h>
#endif

extern PaStream *stream;
int initPortAudio();
int closePortAudio();
