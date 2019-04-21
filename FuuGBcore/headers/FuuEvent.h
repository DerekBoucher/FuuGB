//
//  FuuEvent.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#ifndef FuuEvent_h
#define FuuEvent_h
#include "Fuupch.h"
#include "Core.h"
#include "Logger.h"

namespace FuuGB
{
    class FUUGB_API FuuEvent
    {
    public:
        static int init();
        
    protected:
        static SDL_Event event;
        
    private:
        static void poll_Event();
        static std::unique_ptr<std::thread> _eventListener;
    };
}

#define FUU_INIT_EVENT_SYSTEM(...) FuuGB::FuuEvent::init()

#endif /* FuuEvent_h */
