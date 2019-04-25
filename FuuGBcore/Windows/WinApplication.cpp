//
//  WinApplication.cpp
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#include "Fuupch.h"
#include "WinApplication.h"
#include "System.h"
#include "Gameboy.h"
#include "FuuEvent.h"

namespace FuuGB
{
	void WinApplication::run()
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

		FUUGB_WINDOW_CONFIG(_SDLwindow);

		while (FUUGB_RUNNING)
		{
			FUUGB_POLL_EVENT();

			switch (FUUGB_EVENT.type)
			{
			case SDL_QUIT:
				FUUGB_RUNNING = false;
				break;
			case SDL_SYSWMEVENT:
				if (FUUGB_EVENT.syswm.msg->msg.win.msg == WM_COMMAND)
				{
					switch (FUUGB_WIN_EVENT)
					{
					case ID_LOADROM:
						FILE* romFile;
						char filepath[255];
						sprintf(filepath, "%ws", open_file(_SDLwindow));
						romFile = fopen(filepath, "r");
						if (gameBoy != nullptr) { delete gameBoy; }
						gameBoy = new Gameboy(_SDLwindow, new Cartridge(romFile));
						fclose(romFile);
						break;
					case ID_EXIT:
						FUUGB_RUNNING = false;
						break;
					}
				}
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
	wchar_t* WinApplication::open_file(SDL_Window* win)
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
		FUUGB_SUBSYSTEM_LOG("Test");

		return ofn.lpstrFile;
	}
}