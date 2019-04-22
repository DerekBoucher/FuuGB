//
//  Application.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

/*
 *  Class Application
 *
 *  Synopsis:   This class defines the base of an inheritance hierarchy
 *              for any superclass that wishes to be part of an application context.
 *              This class allows client applications to subclass it and use the provided
 *              run() method to effectively have the application context run within the dynamic
 *              library.
 */
#ifndef Application_h
#define Application_h
#include "Logger.h"
#include "FuuEvent.h"

namespace FuuGB
{
    class FUUGB_API Application
    {
    public:
        virtual void run() = 0;
    };
}

#endif /* Application_h */
