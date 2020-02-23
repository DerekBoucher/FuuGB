#ifdef FUUGB_SYSTEM_WINDOWS

#ifndef Utilities_h
#define Utilities_h

#include "Fuupch.h"
#include "Core.h"

#define ID_LOADROM 1
#define ID_ABOUT 2
#define ID_EXIT 3
#define ID_CONTROLS 4
#define ID_EXT_DISPLAY 5

namespace FuuGB
{
HWND FUUGB_API getSDLWinHandle(SDL_Window *);
HINSTANCE FUUGB_API getHinst(SDL_Window *);
void FUUGB_API ActivateMenu(HWND);

} // namespace FuuGB

#endif /* Utilities_h */

#endif