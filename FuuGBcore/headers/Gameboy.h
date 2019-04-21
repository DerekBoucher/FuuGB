/*
 * Gameboy.h
 *
 *  Created on: Dec 29, 2018
 *      Author: Derek Boucher
 *		ID:40015459
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_
#include "Core.h"
#include "Cartridge.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"

namespace FuuGB
{
    class Gameboy
    {
    public:
        Gameboy(SDL_Window*, Cartridge*);
        virtual ~Gameboy();
    
    private:
        CPU*        cpu;
        Memory*     memory;
        PPU*        ppu;
    };
}

#endif /* GAMEBOY_H_ */
