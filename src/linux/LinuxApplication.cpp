#ifdef FUUGB_SYSTEM_LINUX

#include "LinuxApplication.h"

namespace FuuGB
{
    LinuxApplication::LinuxApplication() {}

    LinuxApplication::~LinuxApplication() {}

    void LinuxApplication::Run(int argc, char **argv)
    {
        // Initialize SDL
        FUUGB_INIT();

        // Instantiate Application Window
        SDL_SysWMinfo *NativeWindowInfo = new SDL_SysWMinfo;
        SDL_Window *_SDLwindow = SDL_CreateWindow("FuuGBemu",
                                                  200,
                                                  SDL_WINDOWPOS_CENTERED,
                                                  WINW,
                                                  WINH + 20,
                                                  0);

        // Configure Application Window
        SDL_GetWindowWMInfo(_SDLwindow, NativeWindowInfo);
        SDL_CreateRenderer(_SDLwindow, -1, SDL_RENDERER_SOFTWARE);

        // Declare a Gameboy Pointer
        Gameboy *gameBoy = nullptr;
        Cartridge *rom = NULL;

        // Open a rom on startup if command line argument was sent
        if (argc > 1)
        {
            rom = GetRom(argv[1]);
            gameBoy = new Gameboy(_SDLwindow, rom);
        }

        // Main application loop
        while (FUUGB_RUNNING)
        {
            while (FUUGB_POLL_EVENT())
            {
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
            SDL_Delay(1);
        }

        /*
        *  Shutdown System
        */
        delete gameBoy;
        SDL_DestroyWindow(_SDLwindow);
        FUUGB_QUIT();
    }

    Cartridge *LinuxApplication::GetRom(char *path)
    {
        printf("Loading ROM: %s\n", path);
        FILE *romFile;
        if (path[0] == '\0')
            return NULL;
        romFile = fopen(path, "r");
        Cartridge *cart = new Cartridge(romFile);
        fclose(romFile);
        return cart;
    }
} // namespace FuuGB

#endif