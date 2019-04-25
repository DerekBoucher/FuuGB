//
//  FuuEvent.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#include "Fuupch.h"
#include "FuuEvent.h"

namespace FuuGB
{
    SDL_Event FUUGB_EVENT;
    bool FUUGB_RUNNING;
    
    void FuuEvent::poll_Event()
    {
        SDL_PollEvent(&FUUGB_EVENT);
    }
}
