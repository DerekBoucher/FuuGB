#ifndef PPU_H
#define PPU_H

#include "Defines.h"
#include "Memory.h"

#include <SDL.h>
#include <wx/wx.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

class PPU {

public:
    PPU(wxWindow*, Memory*);
    PPU(PPU&) = delete;
    ~PPU();
    void RenderScreen();
    void UpdateGraphics(int);

private:
    struct sprite {
        uBYTE yPos;
        uBYTE xPos;
        uBYTE patternNumber;
        uBYTE attributes;
    };

    wxFrame* parent;
    SDL_Window* sdlWindow;
    SDL_Renderer* renderer;
    SDL_Rect pixels[NATIVE_SIZE_X][NATIVE_SIZE_Y];
    uBYTE pixelData[NATIVE_SIZE_X][NATIVE_SIZE_Y];
    uBYTE LCDC;
    uBYTE STAT;
    Memory* memoryRef;
    int currentScanline;
    int scanlineCounter;

    void DrawScanline();
    void RenderTiles();
    void RenderWindow();
    void RenderSprites();
    void SetLCDStatus();
    sprite* ProcessSprites();
    uBYTE GetStat();
    uBYTE GetLCDC();
};

#endif