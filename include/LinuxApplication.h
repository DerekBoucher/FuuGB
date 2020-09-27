#ifdef FUUGB_SYSTEM_LINUX

#ifndef LINUXAPPLICATION_h
#define LINUXAPPLICATION_h

#include "Application.h"
#include "System.h"
#include "Gameboy.h"
#include "Core.h"

#define WINW 160 * SCALE_FACTOR
#define WINH 144 * SCALE_FACTOR

namespace FuuGB
{
    class LinuxApplication : public Application
    {
    public:
        LinuxApplication();
        ~LinuxApplication();
        void Run(int, char **) override;

    private:
        Cartridge *GetRom(char *);
    };
} // namespace FuuGB

#endif
#endif