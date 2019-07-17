//
//  MacApplication.mm
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-21.
//

#include "Application.h"
#include "System.h"
#include "Gameboy.h"
#include "CocoaWindow.h"
#include "FuuEvent.h"


namespace FuuGB
{
    class FUUGB_API MacApplication : public Application
    {
    public:
        MacApplication()
        {
            
        }
        ~MacApplication()
        {
            
        }
        void run()
        {
            FUUGB_INIT();
            SDL_Window* _SDLwindow = SDL_CreateWindow("FuuGBemu",
                                                      SDL_WINDOWPOS_CENTERED,
                                                      SDL_WINDOWPOS_CENTERED,
                                                      160*SCALE_FACTOR,
                                                      144*SCALE_FACTOR,
                                                      0);
            SDL_SysWMinfo* NativeWindowInfo = new SDL_SysWMinfo;
            SDL_GetWindowWMInfo(_SDLwindow, NativeWindowInfo);
            
            Gameboy* gameBoy = nullptr;
            
            CocoaWindow* MacWindow = [[CocoaWindow alloc] init];
            MacWindow->MacEvent->gb_ref = gameBoy;
            [MacWindow configureWindow:NativeWindowInfo];
            
            while(FUUGB_RUNNING)
            {
                FUUGB_POLL_EVENT();

                if(MacWindow->MacEvent->inputBuffer != NULL)
                {
                    if(gameBoy != nullptr)
                    {
                        delete gameBoy;
                        gameBoy = nullptr;
                    }
                    gameBoy = new Gameboy(_SDLwindow,
                                          new Cartridge(MacWindow->MacEvent->inputBuffer));
                    fclose(MacWindow->MacEvent->inputBuffer);
                    MacWindow->MacEvent->inputBuffer = NULL;
                    MacWindow->MacEvent->gb_ref = gameBoy;
                }
                
                switch(FUUGB_EVENT.type)
                {
                    case SDL_QUIT:
                        FUUGB_RUNNING = false;
                        break;
                    case SDL_SYSWMEVENT:
                        break;
                    default:
                        break;
                }
            }
            
            /*
             *  Shutdown System
             */
            delete gameBoy;
            SDL_DestroyWindow(_SDLwindow);
            FUUGB_QUIT();
        }
    };
}
