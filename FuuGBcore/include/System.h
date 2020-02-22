#ifndef System_h
#define System_h

#include "Core.h"
#include "FuuEvent.h"
#include "PPU.h"

namespace FuuGB
{
class FUUGB_API System
{
public:
    static inline void initSystems()
    {
        FUUGB_RUNNING = true;
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
        Shared::mu_GB = PTHREAD_MUTEX_INITIALIZER;
        Shared::cv_GB = PTHREAD_COND_INITIALIZER;
    }

    static inline void closeSystems()
    {
        SDL_Quit();
    }
};
} // namespace FuuGB

#define FUUGB_INIT() FuuGB::System::initSystems()
#define FUUGB_QUIT() FuuGB::System::closeSystems()

#endif /* System_h */
