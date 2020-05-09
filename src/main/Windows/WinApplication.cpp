#include "WinApplication.h"

namespace FuuGB
{
    void WinApplication::run()
    {
        // Initialize SDL
        FUUGB_INIT();

        // Enable Configurations
        bool extDisp = false;

        // Instantiate Application Window
        SDL_SysWMinfo* NativeWindowInfo = new SDL_SysWMinfo;
        SDL_Window* _SDLwindow = SDL_CreateWindow("FuuGBemu",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINX,
        WINY,
        0);

        // Configure Application Window
        SDL_GetWindowWMInfo(_SDLwindow, NativeWindowInfo);
        SDL_CreateRenderer(_SDLwindow, -1, SDL_RENDERER_SOFTWARE);
        FUUGB_WINDOW_CONFIG(_SDLwindow);

        // Declare a Gameboy Pointer
        Gameboy* gameBoy = nullptr;

#ifdef FUUGB_DEBUG
        // Attach Debugger
        Debugger* debugger = new Debugger(_SDLwindow, gameBoy);
#endif

        while (FUUGB_RUNNING) {

            while (FUUGB_POLL_EVENT()) {

                switch (FUUGB_EVENT.type) {

                    case SDL_QUIT: FUUGB_RUNNING = false; break;

                    case SDL_SYSWMEVENT:
                        if (FUUGB_EVENT.syswm.msg->msg.win.msg == WM_COMMAND) {
                        
                        switch (FUUGB_WIN_EVENT) {
                            
                            case ID_LOADROM:

                                if (gameBoy != nullptr) gameBoy->Pause();

                                ROM = FUUGB_LOAD_ROM();

                                if (ROM == NULL) { gameBoy->Resume(); break; }

                                if (gameBoy != nullptr) { gameBoy->Resume(); delete gameBoy; }

                                gameBoy = new Gameboy(_SDLwindow, ROM);

                                break;

                            case ID_EXT_DISPLAY: break;

                            case ID_EXIT: FUUGB_RUNNING = false; break;
                        }
                    }
                    break;

                    default: break;
                }
            }
#ifdef FUUGB_DEBUG
            debugger->ProcessEvents(FUUGB_EVENT);
            debugger->PerformLogic();
            debugger->RenderGui();
#endif
            SDL_Delay(1);
        }

        /*
        *  Shutdown System
        */
        delete gameBoy;
        SDL_DestroyWindow(_SDLwindow);
        FUUGB_QUIT();
    }

    char* WinApplication::open_file(SDL_Window* win)
    {
        OPENFILENAME ofn;

        wchar_t file_path[255];
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = FUUGB_WIN_HANDLE(win);
        ofn.lpstrFile = file_path;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = 255;
        ofn.lpstrFilter = L"Gameboy Roms (*.gb)\0*.gb\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_NOCHANGEDIR;

        GetOpenFileName(&ofn);

        char* path = new char[255];
        sprintf(path, "%ws", ofn.lpstrFile);
        printf("Loading ROM: %s\n", path);
        SDL_SetWindowTitle(win, path);
        return path;
    }

    Cartridge* WinApplication::getRom(char* path, SDL_Window* win)
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
}