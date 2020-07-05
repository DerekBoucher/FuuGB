#ifdef FUUGB_SYSTEM_WINDOWS
#include "Utilities.h"

namespace FuuGB
{
    HMENU handles::hHelp;
    HMENU handles::hEdit;
    HMENU handles::hOptions;
    HMENU handles::hFile;
    HMENU handles::hMenuBar;

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
        handles::hMenuBar = CreateMenu();
        handles::hFile = CreateMenu();
        handles::hEdit = CreateMenu();
        handles::hOptions = CreateMenu();
        handles::hHelp = CreateMenu();

        AppendMenu(handles::hMenuBar, MF_POPUP, (UINT_PTR)handles::hFile, L"File");
        AppendMenu(handles::hMenuBar, MF_POPUP, (UINT_PTR)handles::hEdit, L"Edit");
        AppendMenu(handles::hMenuBar, MF_POPUP, (UINT_PTR)handles::hOptions, L"Options");
        AppendMenu(handles::hMenuBar, MF_POPUP, (UINT_PTR)handles::hHelp, L"Help");
        
        AppendMenu(handles::hFile, MF_STRING, ID_LOADROM, L"Load ROM");
        AppendMenu(handles::hFile, MF_STRING, ID_EXIT, L"Exit");
        
        AppendMenu(handles::hEdit, MF_STRING, ID_CONTROLS, L"Configure Controls");

        AppendMenu(handles::hOptions, MF_STRING, ID_EXT_DISPLAY, L"Extend Display");
        
        AppendMenu(handles::hHelp, MF_STRING, ID_ABOUT, L"About");
        
        SetMenu(windowRef, handles::hMenuBar);
    }

    char* OpenFile(SDL_Window* win)
    {
        OPENFILENAME ofn;

        wchar_t file_path[255];
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = getSDLWinHandle(win);
        ofn.lpstrFile = file_path;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = 255;
        ofn.lpstrFilter = L"Gameboy Roms (*.gb)\0*.gb\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_NOCHANGEDIR;

        GetOpenFileName(&ofn);

        char* path = new char[255];
        sprintf(path, "%ls", ofn.lpstrFile);
        printf("Loading ROM: %s\n", path);
        SDL_SetWindowTitle(win, path);
        return path;
    }

    Cartridge* GetRom(char* path)
    {
        FILE* romFile;
        if (path[0] == '\0')
            return NULL;
        romFile = fopen(path, "r");
        Cartridge* cart = new Cartridge(romFile);
        fclose(romFile);
        delete path;
        return cart;
    }
};
#endif