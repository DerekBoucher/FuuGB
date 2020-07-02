#ifndef GAMEBOY_H_
#define GAMEBOY_H_
#include "CPU.h"
#include "PPU.h"
#include "Core.h"
#include <chrono>

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
        CPU*            cpuUnit;
        Memory*         memoryUnit;
        PPU*            ppuUnit;
        std::thread*    thread;
        bool            pause;
        bool            running;

        void wait();
    };
}

#endif /* GAMEBOY_H_ */
