//
//  Core.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

/*
 *  Header Core
 *
 *  Synposis:   Header containing various system core defines and macros.
 *              Referenced by almost all components of this library.
 */
#ifndef Core_h
#define Core_h

#include "Fuupch.h"

#ifdef FUUGB_SYSTEM_WINDOWS
    #ifdef FUUGB_BUILD_DLL
        #define FUUGB_API __declspec(dllexport)
    #else
        #define FUUGB_API __declspec(dllimport)
    #endif
#endif

#ifdef FUUGB_SYSTEM_MACOS
    #define FUUGB_API
#endif

#ifdef FUUGB_SYSTEM_LINUX
    #define FUUGB_API
#endif

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
#endif /* Core_h */
