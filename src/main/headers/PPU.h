#ifndef PPU_h
#define PPU_h

#include "Core.h"
#include "Memory.h"

#define NATIVE_SIZE_X       160
#define NATIVE_SIZE_Y       144
#define EXT_SIZE_X          256
#define EXT_SIZE_Y          256
#define PPU_CLOCK_PERIOD_NS 239
#define TILE_BYTE_LENGTH    16
#define LCDC_ADR                0xFF40
#define STAT_ADR                0xFF41

namespace FuuGB
{
    class PPU
    {
    public:
        PPU(SDL_Window* windowPtr, Memory* mem);
        virtual ~PPU();
        void DrawScanLine();
        void RenderScreen();
        void UpdateGraphics(int);
        std::bitset<8> GetSTAT();
        std::bitset<8> GetLCDC();

    private:
        SDL_Renderer*   renderer;
        SDL_Rect        pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
        Memory*         memoryRef;
        int             currentScanline;
        int             scanlineCounter;
        std::bitset<8>  LCDC;
        std::bitset<8>  STAT;

        void drawScanline();
        void renderTiles();
        void renderSprites();
        void setLCDStatus();
    };
}
#endif
