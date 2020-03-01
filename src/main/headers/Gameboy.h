#ifndef GAMEBOY_H_
#define GAMEBOY_H_
#include "Core.h"
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
        CPU*        cpu;
        Memory*     memory;
        PPU*        ppu;
		std::thread* _gameboyTHR;
		bool		globalPause;
		bool		running;
    };
}

#endif /* GAMEBOY_H_ */
