#include "Fuupch.h"
#include "FuuEvent.h"

namespace FuuGB
{
    SDL_Event FUUGB_EVENT;
    bool FUUGB_RUNNING;
    
    int FuuEvent::poll_Event()
    {
        return SDL_PollEvent(&FUUGB_EVENT);
    }
}
