#ifndef GAMEBOY_H_
#define GAMEBOY_H_
#include "CPU.h"
#include "PPU.h"

namespace FuuGB
{
    class Gameboy
    {
    public:
        Gameboy(SDL_Window*, Cartridge*);
        virtual ~Gameboy();

        void Pause();
        void Resume();
        void Run();

    private:
        CPU*            CpuUnit;
        Memory*         MemoryUnit;
        PPU*            PpuUnit;
        std::thread*    GameboyThread;
        bool            globalPause;
        bool            running;
    };
}

#endif /* GAMEBOY_H_ */
