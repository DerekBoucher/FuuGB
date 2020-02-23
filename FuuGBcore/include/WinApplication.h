#ifdef FUUGB_SYSTEM_WINDOWS

#ifndef WinApplication_h
#define WinApplication_h

#include "Application.h"
#include "Utilities.h"
#include "System.h"
#include "Gameboy.h"

namespace FuuGB
{
class FUUGB_API WinApplication : public Application
{
public:
	void run();

private:
	char *open_file(SDL_Window *);
	Cartridge *getRom(char *, SDL_Window *);
};
} // namespace FuuGB

#define FUUGB_WINDOW_CONFIG(...) FuuGB::ActivateMenu(FuuGB::getSDLWinHandle(__VA_ARGS__))
#define FUUGB_WIN_EVENT FUUGB_EVENT.syswm.msg->msg.win.wParam
#define FUUGB_WIN_HANDLE(...) FuuGB::getSDLWinHandle(__VA_ARGS__)
#define FUUGB_LOAD_ROM() getRom(FILE_PATH, _SDLwindow)
#define FILE_PATH open_file(_SDLwindow)

#endif /* WinApplication_h */

#endif