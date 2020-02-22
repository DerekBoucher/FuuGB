#ifndef FuuEvent_h
#define FuuEvent_h

#include "Core.h"

namespace FuuGB
{
class FUUGB_API FuuEvent
{
public:
    static int poll_Event();
    static SDL_Event event;
};
} // namespace FuuGB

#define FUUGB_EVENT FuuGB::FuuEvent::event
#define FUUGB_POLL_EVENT() FuuGB::FuuEvent::poll_Event()

#endif /* FuuEvent_h */
