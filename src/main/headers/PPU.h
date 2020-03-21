#ifndef PPU_h
#define PPU_h

#include "Core.h"
#include "Memory.h"

#define NATIVE_SIZE_X   160
#define NATIVE_SIZE_Y   144
#define EXT_SIZE_X        256
#define EXT_SIZE_Y        256
#define PPU_CLOCK_PERIOD_NS 239
#define TILE_BYTE_LENGTH 16

namespace FuuGB
{
    class PPU
    {
    public:
        PPU(SDL_Window* windowPtr, Memory* mem, bool extended);
        virtual ~PPU();
        void DrawScanLine();
        SDL_Renderer*           renderer;
        void renderscreen();
        void updateGraphics(int);

    private:
        SDL_Rect                pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
        SDL_Rect                ext_Pixels[EXT_SIZE_X][EXT_SIZE_Y];
        Memory*                 MEM;
        uWORD                   BG_Map_Pointer;
        int                     currentScanLine;
        int                     scanline_counter;
        void RenderTiles();
        void RenderSprites();
        void setLCDStatus();
        bool ext;
    };
}
#endif /* PPU_h */
