#ifndef CORE_H
#define CORE_H

    #ifdef FUUGB_SYSTEM_WINDOWS
        #include <Windows.h>
        #include <condition_variable>
    #endif

    #ifdef FUUGB_SYSTEM_MACOS
        #include <Cocoa/Cocoa.h>
    #endif

    #ifdef FUUGB_SYSTEM_LINUX
        #include <condition_variable>
    #endif

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

    #include <SDL_syswm.h>
    #include <SDL.h>
    #include <thread>
    #include <stdlib.h>
    #include <stdio.h>
    #include <time.h>
    #include <limits.h>

    typedef unsigned char uBYTE;
    typedef unsigned short uWORD;
    typedef char sBYTE;
    typedef short sWORD;

    namespace FuuGB
    {
        struct Shared
        {
            static const int ScaleFactor = 4;
            static bool RUNNING;
            static std::condition_variable cv_GB;
            static std::mutex mu_GB;
        };
    }

    #define FUUGB_RUNNING Shared::RUNNING
    #define SCALE_FACTOR FuuGB::Shared::ScaleFactor

#endif
