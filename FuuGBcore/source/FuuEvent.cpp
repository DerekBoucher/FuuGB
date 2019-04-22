//
//  FuuEvent.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#include "FuuEvent.h"

namespace FuuGB
{
    SDL_Event FUUGB_EVENT;
    bool FUUGB_RUNNING;
    
    void FuuEvent::poll_Event()
    {
        SDL_PollEvent(&FUUGB_EVENT);
        
        switch(FUUGB_EVENT.type)
        {
            case SDL_QUIT:
                FUUGB_SUBSYSTEM_LOG("User has clicked Exit!");
                break;
            case SDL_MOUSEMOTION:
                FUUGB_SUBSYSTEM_LOG("User Has Moved Mouse!");
                break;
            case SDL_KEYDOWN:
                FUUGB_SUBSYSTEM_LOG("User has pressed a key!");
                break;
            default:
                break;
        }
    }
}
