#ifdef FUUGB_SYSTEM_MACOS

#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include "Application.h"
#include "System.h"
#include "Gameboy.h"
#include "CocoaWindow.h"
#include "FuuEvent.h"

#define WINW 160 * SCALE_FACTOR
#define WINH 144 * SCALE_FACTOR

namespace FuuGB
{
    class MacApplication : public Application
    {
    public:
        MacApplication();
        virtual ~MacApplication();
        void Run(int, char **);

    private:
    };
} // namespace FuuGB

#endif
#endif