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

#define NATIVE_SIZE_X   160
#define NATIVE_SIZE_Y   144
#define PPU_CLOCK_PERIOD_NS 239
#define TILE_BYTE_LENGTH 16

namespace FuuGB
{
    class FUUGB_API PPU
    {
    public:
        PPU(SDL_Window* windowPtr, Memory* mem);
        virtual ~PPU();
        void DrawScanLine();
		SDL_Renderer*               renderer;
        void renderscreen();
        void updateGraphics(int);
    
    private:
        SDL_Rect pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
		Memory*						MEM;
		uWORD						BG_Map_Pointer;
        int                         currentScanLine;
        int                         scanline_counter;
        void RenderTiles();
        void RenderSprites();
        void setLCDStatus();
    };
}
#endif /* PPU_h */
