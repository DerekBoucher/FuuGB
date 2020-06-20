#ifdef FUUGB_SYSTEM_LINUX
#include "Application.h"
#include "Cartridge.h"
#include "System.h"
#include "Gameboy.h"
#include <SDL.h>
#include <SDL_syswm.h>

#define WINW 160 * SCALE_FACTOR
#define WINH 144 * SCALE_FACTOR

namespace FuuGB 
{
    class LinuxApplication : public Application
    {
    public:
        void run();
    private:
        Cartridge* ROM;
    };
}
#endif