#include "Fuupch.h"
#include "FuuEvent.h"

SDL_Event FuuGB::FuuEvent::event;

namespace FuuGB
{
bool FUUGB_RUNNING;

int FuuEvent::poll_Event()
{
    return SDL_PollEvent(&FUUGB_EVENT);
}
} // namespace FuuGB
