//
//  Utilities.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h
#include <Windows.h>

//Various application utilities
namespace Utilities
{
    //Namespace variables/Defines
#define ID_LOADROM 1
#define ID_ABOUT 2
#define ID_EXIT 3
#define ID_CONTROLS 4
    static HMENU hHelp;
    static HMENU hEdit;
    static HMENU hFile;
    static HMENU hMenuBar;
    
    
    //Function which retrieves the address/Handle of an SDL window
    //Also retrieves the specific subsystem used by SDL to create that window which is platform specific (Windows, MAC OS x, IOS, etc...)
    HWND getSDLWinHandle(SDL_Window* win)
    {
        SDL_SysWMinfo infoWindow;
        SDL_VERSION(&infoWindow.version);
        if (!SDL_GetWindowWMInfo(win, &infoWindow))
        {
            return NULL;
        }
        return (infoWindow.info.win.window);
    }
    
    HINSTANCE getHinst(SDL_Window* win)
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
        hMenuBar = CreateMenu();
        hFile = CreateMenu();
        hEdit = CreateMenu();
        hHelp = CreateMenu();
        
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, "File");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEdit, "Edit");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelp, "Help");
        
        AppendMenu(hFile, MF_STRING, ID_LOADROM, "Load ROM");
        AppendMenu(hFile, MF_STRING, ID_EXIT, "Exit");
        
        AppendMenu(hEdit, MF_STRING, ID_CONTROLS, "Configure Controls");
        
        AppendMenu(hHelp, MF_STRING, ID_ABOUT, "About");
        
        SetMenu(windowRef, hMenuBar);
    }
};
#endif /* Utilities_h */
