//
//  FuuEvent.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#include "FuuEvent.h"

namespace FuuGB
{
    std::unique_ptr<std::thread> FuuEvent::_eventListener;
    SDL_Event FuuEvent::event;
    
    int FuuEvent::init()
    {
        int returnVal = SDL_InitSubSystem(SDL_INIT_EVENTS);
        _eventListener = std::make_unique<std::thread>(&FuuEvent::poll_Event);
        return returnVal;
    }
    
    void FuuEvent::poll_Event()
    {
        while(true)
        {
            SDL_PollEvent(&event);
            
            switch(event.type)
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
}
