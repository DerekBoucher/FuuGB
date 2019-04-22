//
//  FuuEvent.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#ifndef FuuEvent_h
#define FuuEvent_h
#include "Core.h"
#include "Logger.h"

namespace FuuGB
{
    class FUUGB_API FuuEvent
    {
    public:
        static void poll_Event();
        static SDL_Event event;
    };
}

#define FUUGB_EVENT FuuGB::FuuEvent::event
#define FUUGB_POLL_EVENT() FuuGB::FuuEvent::poll_Event()

#endif /* FuuEvent_h */
