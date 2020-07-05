#ifdef FUUGB_SYSTEM_WINDOWS

#ifndef WinApplication_h
#define WinApplication_h

#include "Application.h"
#include "Utilities.h"
#include "System.h"
#include "Gameboy.h"
#include "FuuEvent.h"

#define FUUGB_WINDOW_CONFIG(...) FuuGB::ActivateMenu(FuuGB::getSDLWinHandle(__VA_ARGS__))
#define FUUGB_WIN_EVENT FUUGB_EVENT.syswm.msg->msg.win.wParam
#define FUUGB_WIN_HANDLE(...) FuuGB::getSDLWinHandle(__VA_ARGS__)
#define FUUGB_LOAD_ROM() FuuGB::GetRom(FILE_PATH, _SDLwindow)
#define FILE_PATH FuuGB::OpenFile(_SDLwindow)
#define WINW 160 * SCALE_FACTOR
#define WINH 144 * SCALE_FACTOR

namespace FuuGB
{
    class WinApplication : public Application
    {
    public:
        void run();
    private:
        Cartridge* ROM;
    };
}

#endif

#endif