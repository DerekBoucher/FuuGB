//
//  Fuupch.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

/*
 *  Header Fuupch
 *
 *  Synopsis:   Header used to define custom precompiled Header file for this library.
 *
 */
#ifndef Fuupch_h
#define Fuupch_h

#ifndef FUUGB_SYSTEM_MACOS
    #include <iostream>
    #include <string>
    #include <vector>
    #include <memory>
    #include <cstring>
    #include <mutex>
    #include <chrono>
    #include <cassert>
#endif

#ifdef FUUGB_SYSTEM_MACOS
    #include <Cocoa/Cocoa.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_syswm.h>
#endif

#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <bitset>
#include <limits.h>

#ifdef FUUGB_SYSTEM_LINUX
    #include <condition_variable>
    #include <SDL2/SDL.h>
	#include <SDL2/SDL_syswm.h>
#endif


#ifdef FUUGB_SYSTEM_WINDOWS
    #include <Windows.h>
	#include <SDL.h>
	#include <SDL_syswm.h>
#endif

#endif /* Fuupch_h */
