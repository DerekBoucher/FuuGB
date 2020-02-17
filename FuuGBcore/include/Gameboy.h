#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Core.h"
#include "CPU.h"
#include "PPU.h"

namespace FuuGB
{
    class FUUGB_API Gameboy
    {
    public:
        Gameboy(SDL_Window*, Cartridge*);
        virtual ~Gameboy();

        void Pause();
        void Resume();
        static void* Run(void* arg);
    
    private:
        CPU*        cpu;
        Memory*     memory;
        PPU*        ppu;
        pthread_t   _gameboyTHR;
        bool        globalPause;
        bool        running;
    };
}

#endif /* GAMEBOY_H_ */
