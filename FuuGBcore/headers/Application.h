//
//  Application.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-19.
//

#ifndef Application_h
#define Application_h
#include "Logger.h"

namespace FuuGB
{
    class FUUGB_API Application
    {
    public:
        Application();
        virtual ~Application();
        
        void run();
    };
}

#endif /* Application_h */
