//
//  Utilities.h
//  FuuGBemu
//
//  Created by Derek Boucher on 2019-02-04.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h
#include "Fuupch.h"

//Various application utilities
namespace FuuGB
{
    //Namespace variables/Defines
#define ID_LOADROM 1
#define ID_ABOUT 2
#define ID_EXIT 3
#define ID_CONTROLS 4
#define ID_EXT_DISPLAY 5
    static HMENU hHelp;
    static HMENU hEdit;
	static HMENU hOptions;
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
		hOptions = CreateMenu();
        hHelp = CreateMenu();

        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFile, L"File");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hEdit, L"Edit");
		AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hOptions, L"Options");
        AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hHelp, L"Help");
        
        AppendMenu(hFile, MF_STRING, ID_LOADROM, L"Load ROM");
        AppendMenu(hFile, MF_STRING, ID_EXIT, L"Exit");
        
        AppendMenu(hEdit, MF_STRING, ID_CONTROLS, L"Configure Controls");

		AppendMenu(hOptions, MF_STRING, ID_EXT_DISPLAY, L"Extend Display");
        
        AppendMenu(hHelp, MF_STRING, ID_ABOUT, L"About");
        
        SetMenu(windowRef, hMenuBar);
    }
};
#endif /* Utilities_h */
