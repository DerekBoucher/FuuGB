#ifdef FUUGB_SYSTEM_LINUX

#include "Fuupch.h"
#include "LinuxApplication.h"

namespace FuuGB
{
    void LinuxApplication::run()
    {
        FUUGB_INIT();
		SDL_Window* _SDLwindow = SDL_CreateWindow("FuuGBemu",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			NATIVE_SIZE_X*SCALE_FACTOR,
			NATIVE_SIZE_Y*SCALE_FACTOR,
			0);
		SDL_SysWMinfo* NativeWindowInfo = new SDL_SysWMinfo;
		SDL_GetWindowWMInfo(_SDLwindow, NativeWindowInfo);

		Gameboy* gameBoy = nullptr;

		//Configure Windows window TO-DO

		while (FUUGB_RUNNING)
		{
			FUUGB_POLL_EVENT();

			switch (FUUGB_EVENT.type)
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
}

#endif