#ifndef Utilities_h
#define Utilities_h
#define ID_LOADROM 1
#define ID_ABOUT 2
#define ID_EXIT 3
#define ID_CONTROLS 4
#define ID_EXT_DISPLAY 5

#include "Fuupch.h"

namespace FuuGB {

    //Namespace variables/Defines
    static HMENU hHelp;
    static HMENU hEdit;
    static HMENU hOptions;
    static HMENU hFile;
    static HMENU hMenuBar;

    HWND getSDLWinHandle(SDL_Window* win);
    HINSTANCE getHinst(SDL_Window* win);
    void ActivateMenu(HWND windowRef);

}

#endif /* Utilities_h */