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

#define NATIVE_SIZE_X   160
#define NATIVE_SIZE_Y   144
#define PPU_CLOCK_PERIOD_NS 239

namespace FuuGB
{
    class FUUGB_API PPU
    {
    public:
        PPU(SDL_Window* windowPtr);
        virtual ~PPU();
        void stop();
    
    private:
        SDL_Rect pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
        SDL_Renderer*               renderer;
        std::thread*                _ppuTHR;
        std::condition_variable     ppuCond;
        std::mutex                  key;
        bool                        _ppuRunning;
    
        void clock();
        void renderScreen();
    };
}
#endif /* PPU_h */
