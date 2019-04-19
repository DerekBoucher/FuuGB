//
//  Core.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

#ifndef Core_h
#define Core_h

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

namespace FuuGB {
}

#endif /* Core_h */
