#ifdef FUUGB_SYSTEM_WINDOWS

#include "Utilities.h"

//Various application utilities
namespace FuuGB
{

//Function which retrieves the address/Handle of an SDL window
//Also retrieves the specific subsystem used by SDL to create that window which is platform specific (Windows, MAC OS x, IOS, etc...)
HWND getSDLWinHandle(SDL_Window *win)
{
    SDL_SysWMinfo infoWindow;
    SDL_VERSION(&infoWindow.version);
    if (!SDL_GetWindowWMInfo(win, &infoWindow))
    {
        return NULL;
    }
    return (infoWindow.info.win.window);
}

HINSTANCE getHinst(SDL_Window *win)
{
    SDL_SysWMinfo infoWindow;
    SDL_VERSION(&infoWindow.version);
    if (!SDL_GetWindowWMInfo(win, &infoWindow))
    {
        return NULL;
    }
    return (infoWindow.info.win.hinstance);
}

//Initializes the native windows drop down menu elements of the window
void ActivateMenu(HWND windowRef)
{
    HMENU hMenuBar = CreateMenu();
    HMENU hFile = CreateMenu();
    HMENU hEdit = CreateMenu();
    HMENU hOptions = CreateMenu();
    HMENU hHelp = CreateMenu();

    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, "File");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEdit, "Edit");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hOptions, "Options");
    AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelp, "Help");

    AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
    AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");

    AppendMenu(hEdit, MF_STRING, ID_CONTROLS, "Configure Controls");

    AppendMenu(hOptions, MF_STRING, ID_EXT_DISPLAY, "Extend Display");

    AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");

    SetMenu(windowRef, hMenuBar);
}
}; // namespace FuuGB

#endif
