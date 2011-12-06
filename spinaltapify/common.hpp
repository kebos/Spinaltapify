#include "spinaltapify.hpp"
#include <libspotify/api.h>
#include <iostream>
#include <memory.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <stdio.h>
#include "compat.h"

extern bool bPortAudioActive;
#define PRINT_LOG
#ifdef PRINT_LOG
#define DebugP(x) std::cout << x
#else
#define DebugP(x) ;
#endif

#ifdef PRINT_LOG
#define DebugPC(...) printf (__VA_ARGS__)
#else
#define DebugPC(...) ;
#endif
