//
//  PPU.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef PPU_h
#define PPU_h

#include "Core.h"
#include "Logger.h"
#include "Memory.h"

#define NATIVE_SIZE_X   256
#define NATIVE_SIZE_Y   256
#define PPU_CLOCK_PERIOD_NS 239
#define TILE_BYTE_LENGTH 16

namespace FuuGB
{
    class FUUGB_API PPU
    {
    public:
        PPU(SDL_Window* windowPtr, Memory* mem);
        virtual ~PPU();
        void stop();
    
    private:
        SDL_Rect pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
		SDL_Rect display[160][144];
        SDL_Renderer*               renderer;
        std::thread*                _ppuTHR;
        std::condition_variable     ppuCond;
        std::mutex                  key;
        bool                        _ppuRunning;
		Memory*						MEM;
		uWORD						BGW_Pointer;
		uWORD						OAM_Pointer;
		uWORD						BG_Map_Pointer;
    
        void clock();
        void renderScreen();
    };
}
#endif /* PPU_h */
