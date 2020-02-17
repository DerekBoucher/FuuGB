#include "Fuupch.h"
#include "FuuEvent.h"

namespace FuuGB
{
    SDL_Event event;
    bool FUUGB_RUNNING;
    
    void FuuEvent::poll_Event()
    {
        SDL_PollEvent(&FUUGB_EVENT);
    }
}
