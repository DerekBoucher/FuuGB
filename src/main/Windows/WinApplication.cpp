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
        WINH,
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
#ifdef FUUGB_DEBUG
                                    debugger->SetGbRef(gameBoy);
                                    debugger->SetCartridgeName();
#endif
                                    
                                    break;

                                case ID_EXT_DISPLAY: break;

                                case ID_EXIT: FUUGB_RUNNING = false; break;
                            }
                        }
                    break;
#ifdef FUUGB_DEBUG
                    case SDL_WINDOWEVENT:
                        if(FUUGB_EVENT.window.event == SDL_WINDOWEVENT_MOVED) {
                            int x, y;
                            SDL_GetWindowPosition(_SDLwindow, &x, &y);
                            debugger->ResetWindowPosition(x, y);
                        }
                        if(FUUGB_EVENT.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                            debugger->MinimizeWindow();
                        }
                        if(FUUGB_EVENT.window.event == SDL_WINDOWEVENT_RESTORED) {
                            debugger->MaximizeWindow();
                        }
                        if(FUUGB_EVENT.window.event == SDL_WINDOWEVENT_CLOSE) {
                            FUUGB_RUNNING = false;
                        }
                        break;
#endif
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
#ifdef FUUGB_DEBUG
        delete debugger;
#endif
        delete gameBoy;
        SDL_DestroyWindow(_SDLwindow);
        FUUGB_QUIT();
    }
}