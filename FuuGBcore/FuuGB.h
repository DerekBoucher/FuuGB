//
//  FuuGB.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

/*
 *  This header file is only referenced by client applications.
 *  Core library never includes this.
 */

#ifndef FuuGB_h
#define FuuGB_h

#ifdef FUUGB_SYSTEM_MACOS
    #include "MacApplication.mm"
    #define FUUGB_APP FuuGB::MacApplication
#else
    #include "headers/Application.h"
#endif

#endif /* FuuGB_h */
