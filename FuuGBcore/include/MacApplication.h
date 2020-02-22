#ifdef FUUGB_SYSTEM_MACOS

#ifndef MacApplication_h
#define MacApplication_h

#include "Application.h"
#include "System.h"
#include "Gameboy.h"
#include "FuuEvent.h"

namespace FuuGB 
{
    class FUUGB_API MacApplication : public Application
    {
    public:
        MacApplication();
        virtual ~MacApplication();
        void run();
    };
}

#endif

#endif