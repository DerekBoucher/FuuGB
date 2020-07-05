#ifdef FUUGB_SYSTEM_WINDOWS
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
        200,
        SDL_WINDOWPOS_CENTERED,
        WINW,
        WINH+20,
        0);

        // Configure Application Window
        SDL_GetWindowWMInfo(_SDLwindow, NativeWindowInfo);
        SDL_CreateRenderer(_SDLwindow, -1, SDL_RENDERER_SOFTWARE);
        FUUGB_WINDOW_CONFIG(_SDLwindow);

        // Declare a Gameboy Pointer
        Gameboy* gameBoy = nullptr;

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
            SDL_Delay(1);
        }

        /*
        *  Shutdown System
        */
        delete gameBoy;
        SDL_DestroyWindow(_SDLwindow);
        FUUGB_QUIT();
    }
}

#endif