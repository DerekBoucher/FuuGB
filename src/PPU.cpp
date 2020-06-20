#include "Core.h"
#include "PPU.h"

namespace FuuGB
{
    PPU::PPU(SDL_Window* windowRef, Memory* mem)
    {
        memoryRef   = mem;
        renderer    = SDL_GetRenderer(windowRef);

        if (renderer == NULL)
        {
            renderer = SDL_CreateRenderer(windowRef, -1, SDL_RENDERER_SOFTWARE);
        }

        for (int i = 0; i < NATIVE_SIZE_X; ++i)
        {
            for (int j = 0; j < NATIVE_SIZE_Y; ++j)
            {
                pixels[i][j].h = SCALE_FACTOR;
                pixels[i][j].w = SCALE_FACTOR;
                pixels[i][j].x = i * SCALE_FACTOR;
                pixels[i][j].y = j * SCALE_FACTOR;
            }
        }

        LCDC = getLCDC();
        STAT = getStat();

        currentScanline = 1;
        scanlineCounter = 456;
    }

    PPU::~PPU()
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_DestroyRenderer(renderer);
    }

    void PPU::RenderScreen()
    {
        LCDC = getLCDC();

        if(!LCDC.test(7))
            return;
        
        SDL_RenderPresent(renderer);
    }


    void PPU::UpdateGraphics(int cycles)
    {
        LCDC = getLCDC();

        setLCDStatus();
        
        if(LCDC.test(7))
        {
            scanlineCounter -= cycles;
        }
        else
        {
            return;
        }
        
        if(scanlineCounter <= 0) // Time to render new frame
        {
            currentScanline = memoryRef->DmaRead(0xFF44);
            
            scanlineCounter = 456;
            
            if(currentScanline < 144) 
            {
                drawScanline();
            }
            else if(currentScanline > 144 && currentScanline < 153) 
            {
                memoryRef->RequestInterupt(0);
            }
            else if (currentScanline == 153)
            {
                memoryRef->DmaWrite(0xFF44, 0x00);
                return;
            }
            
            memoryRef->DmaWrite(0xFF44, memoryRef->DmaRead(0xFF44) + 1);
        }
    }
    
    void PPU::drawScanline()
    {
        LCDC = getLCDC();

        if(LCDC.test(0))
        {
            renderTiles();
        }
        if(LCDC.test(1))
        {
            renderSprites();
        }
    }
    
    void PPU::renderTiles()
    {
        LCDC = getLCDC();

        uWORD tileDataPtr   = 0x0;
        uWORD tileMapPtr    = 0x0;
        uWORD winMapPtr     = 0x0;
        
        bool winEnabled = false;
        bool unsignedID = false;
        
        // Determine The offsets to use when retrieving the Tile Identifiers from
        // Tile Map address space.
        uBYTE scrollX   = memoryRef->DmaRead(0xFF43);
        uBYTE scrollY   = memoryRef->DmaRead(0xFF42);
        uBYTE winX      = memoryRef->DmaRead(0xFF4B) - 7;
        uBYTE winY      = memoryRef->DmaRead(0xFF4A);
        
        
        // Determine base address of tile data for BG & window
        if(LCDC.test(4))
        {
            tileDataPtr = 0x8000;
            unsignedID  = true;
        }
        else
        {
            tileDataPtr = 0x8800;
            unsignedID = false;
        }
        
        // Determine the base address tile Mappings for BG & window
        if(LCDC.test(3))
        {
            tileMapPtr = 0x9C00;
        }
        else
        {
            tileMapPtr = 0x9800;
        }
        
        // Determine if the window is to be rendered
        // The window is rendered above the background
        if(LCDC.test(5))
        {
            winEnabled = true;
        }
        else
        {
            winEnabled = false;
        }
        
        // If Window is to be rendered, determine its Maping ptr in memoryUnit
        if(winEnabled)
        {
            if(LCDC.test(6))
            {
                winMapPtr = 0x9C00;
            }
            else
            {
                winMapPtr = 0x9800;
            }
        }
        
        // Determine the current scanline we are on
        currentScanline = memoryRef->DmaRead(0xFF44);

        uWORD yPos;

        // If window is not enabled, then add scrollY to current scanline,
        // else 
        if (!winEnabled)
        {
            yPos = scrollY + currentScanline;
        }
        else
        {
            yPos = currentScanline - winY;
        }

        uWORD tileRow = (yPos / 8) * 32;
        
        // Start Rendering the scanline
        for(int pixel = 0; pixel < 160; pixel++) 
        {
            if(currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159)
            {
                continue;
            }

            uWORD xPos = pixel + scrollX;

            if(winEnabled)
            {
                if (pixel >= winX)
                {
                    xPos = pixel - winX;
                }
            }

            uWORD tileColumn = xPos / 8;

            // Determine the address for the tile ID
            uWORD currentTileMapAdr = tileMapPtr + tileColumn + tileRow;
            
            // Fetch the tile ID
            uBYTE tileID = memoryRef->DmaRead(currentTileMapAdr);
            
            // Determine the current pixel data from the tile data
            uBYTE tileLineOffset = (yPos % 8) * 2; //Each line is 2 bytes
            uWORD tileDataAdr;
            uBYTE data1, data2;

            if(unsignedID)
            {
                tileDataAdr = tileDataPtr + (tileID * 16);
                data1 = memoryRef->DmaRead(tileDataAdr + tileLineOffset);
                data2 = memoryRef->DmaRead(tileDataAdr + tileLineOffset + 1);
            }
            else
            {
                if (tileID & 0x80)
                {
                    tileID = ~tileID;
                    tileID += 0x01;
                    tileDataAdr = tileDataPtr - (tileID * 16);
                }
                else
                {
                    tileDataAdr = tileDataPtr + (tileID * 16);
                }
                
                data1 = memoryRef->DmaRead(tileDataAdr + tileLineOffset);
                data2 = memoryRef->DmaRead(tileDataAdr + tileLineOffset + 1);
            }
            
            int currentBitPosition = (((pixel % 8) - 7)* -1);
            
            std::bitset<8> d1(data1);
            std::bitset<8> d2(data2);
            std::bitset<2> ColorCode;

            if(d2.test(currentBitPosition))
            {
                ColorCode.set(1);
            }
            else
            {
                ColorCode.reset(1);
            }
            
            if(d1.test(currentBitPosition))
            {
                ColorCode.set(0);
            }
            else
            {
                ColorCode.reset(0);
            }
            
            int R = 0x0;
            int G = 0x0;
            int B = 0x0;

            std::bitset<2> Color_00(memoryRef->DmaRead(0xFF47) & 0x03);
            std::bitset<2> Color_01((memoryRef->DmaRead(0xFF47)>>2) & 0x03);
            std::bitset<2> Color_10((memoryRef->DmaRead(0xFF47)>>4) & 0x03);
            std::bitset<2> Color_11((memoryRef->DmaRead(0xFF47)>>6) & 0x03);
            
            //Determine actual color for pixel via Color Pallete reg
            switch(ColorCode.to_ulong())
            {
                case 0x00:
                    if(Color_00.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_00.to_ulong() == 0x1) { R = 211; G = 211; B = 211; }
                    else if(Color_00.to_ulong() == 0x2) { R = 169; G = 169; B = 169; }
                    else if(Color_00.to_ulong() == 0x3) { R = 0; G = 0; B = 0; }
                    break;
                case 0x01:
                    if(Color_01.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_01.to_ulong() == 0x1) { R = 211; G = 211; B = 211; }
                    else if(Color_01.to_ulong() == 0x2) { R = 169; G = 169; B = 169; }
                    else if(Color_01.to_ulong() == 0x3) { R = 0; G = 0; B = 0; }
                    break;
                case 0x02:
                    if(Color_10.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_10.to_ulong() == 0x1) { R = 211; G = 211; B = 211; }
                    else if(Color_10.to_ulong() == 0x2) { R = 169; G = 169; B = 169; }
                    else if(Color_10.to_ulong() == 0x3) { R = 0; G = 0; B = 0; }
                    break;
                case 0x03:
                    if(Color_11.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_11.to_ulong() == 0x1) { R = 211; G = 211; B = 211; }
                    else if(Color_11.to_ulong() == 0x2) { R = 169; G = 169; B = 169; }
                    else if(Color_11.to_ulong() == 0x3) { R = 0; G = 0; B = 0; }
                    break;
                default:
                    break;
            }
            
            if(currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159)
            {
                continue;
            }
            
            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);
        }
    }
    
    void PPU::renderSprites()
    {
        LCDC = getLCDC();
        
        bool u_8x16 = false;
        
        if(LCDC.test(2))
        {
            u_8x16 = true;
        }
        
        for (int sprite = 0; sprite < 40; sprite++)
        {
            uBYTE index         = sprite * 4;
            uBYTE yPos          = memoryRef->DmaRead(0xFE00 + index);
            uBYTE xPos          = memoryRef->DmaRead(0xFE00 + index + 1);
            uBYTE tileLocation  = memoryRef->DmaRead(0xFFE0 + index + 2);
            uBYTE attributes    = memoryRef->DmaRead(0xFFE0 + index + 3);
            
            std::bitset<8> attr(attributes);
            
            bool yFlip = attr.test(6);
            bool xFlip = attr.test(5);
            
            currentScanline = memoryRef->DmaRead(0xFF44);
            
            int ysize = 8;

            if(u_8x16)
            {
                ysize = 16;
            }
            
            if((currentScanline >= yPos) && (currentScanline < (yPos + ysize)))
            {
                int line = currentScanline - yPos;
                
                if(yFlip)
                {
                    line -= ysize;
                    line *= -1;
                }
                
                line *= 2;
                
                uWORD dataaddr  = (0x8000 + (tileLocation * 16)) + line;
                uBYTE data1     = memoryRef->DmaRead(dataaddr);
                uBYTE data2     = memoryRef->DmaRead(dataaddr + 1);
                
                for(int tilepixel =7 ; tilepixel >= 0; tilepixel--)
                {
                    int colorbit = tilepixel;
                    
                    if(xFlip)
                    {
                        colorbit -= 7;
                        colorbit *= -1;
                    }
                    
                    std::bitset<8> d2(data2);
                    std::bitset<8> d1(data1);
                    int ColorEncoding = d2.test(colorbit);
                    
                    ColorEncoding <<= 1;
                    
                    ColorEncoding |= d1.test(colorbit);
                    
                    uWORD coloradr;
                    if(attr.test(4))
                        coloradr = 0xFF49;
                    else
                        coloradr = 0xFF48;
                    
                    int R = 0x0;
                    int G = 0x0;
                    int B = 0x0;
                    
                    std::bitset<2> Color_00(memoryRef->DmaRead(coloradr) & 0x03);
                    std::bitset<2> Color_01((memoryRef->DmaRead(coloradr) >> 2) & 0x03);
                    std::bitset<2> Color_10((memoryRef->DmaRead(coloradr) >> 4) & 0x03);
                    std::bitset<2> Color_11((memoryRef->DmaRead(coloradr) >> 6) & 0x03);
                    
                    //Determine actual color for pixel via Color Pallete reg
                    switch(ColorEncoding)
                    {
                        case 0x00:
                            if(Color_00.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_00.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_00.to_ulong() == 0x02) { R = 169; G = 169; B = 169; }
                            else if(Color_00.to_ulong() == 0x03) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x01:
                            if(Color_01.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_01.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_01.to_ulong() == 0x02) { R = 169; G = 169; B = 169; }
                            else if(Color_01.to_ulong() == 0x03) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x02:
                            if(Color_10.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_10.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_10.to_ulong() == 0x02) { R = 169; G = 169; B = 169; }
                            else if(Color_10.to_ulong() == 0x03) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x03:
                            if(Color_11.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_11.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_11.to_ulong() == 0x02) { R = 169; G = 169; B = 169; }
                            else if(Color_11.to_ulong() == 0x03) { R = 0; G = 0; B = 0; }
                            break;
                    }
                    
                    if(R == 255 && G == 255 && B == 255)
                    {
                        continue;
                    }
                    
                    int xPix = 0 - tilepixel;
                    
                    xPix += 7;
                    
                    int pixel = xPos + xPix;
                    
                    SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
                    SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
                    SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);
                }
            }
        }
    }
    
    void PPU::setLCDStatus()
    {
        LCDC = getLCDC();
        STAT = getStat();
        
        if(!LCDC.test(7))
        {
            scanlineCounter = 456;
            memoryRef->DmaWrite(0xFF44, 0x00);
            STAT.reset(0);
            STAT.reset(0);
            memoryRef->DmaWrite(STAT_ADR, STAT.to_ulong());
            return;
        }
        
        currentScanline = memoryRef->DmaRead(0xFF44);
        uBYTE currentMode = STAT.to_ulong() & 0x03;
        
        uBYTE mode = 0;
        bool reqInt = false;
        
        // Mode 1
        if(currentScanline >= 144)
        {
            mode = 0x01;
            STAT.reset(1);
            STAT.set(0);
            reqInt = STAT.test(5);
        }
        else
        {
            int mode2BOUND = 456 - 80;
            int mode3BOUND = mode2BOUND - 172;

            // Mode 2
            if (scanlineCounter >= mode2BOUND)
            {
                mode = 0x02;
                STAT.set(1);
                STAT.reset(0);
                reqInt = STAT.test(5);
            }
            // Mode 3
            else if (scanlineCounter >= mode3BOUND)
            {
                mode = 0x03;
                STAT.set(1);
                STAT.set(0);
            }
            // Mode 0
            else
            {
                mode = 0x00;
                STAT.reset(1);
                STAT.reset(0);
                reqInt = STAT.test(3);
            }
        }
            
        if(reqInt && (mode != currentMode))
        {
            memoryRef->RequestInterupt(1);
        }
            
        if(memoryRef->DmaRead(0xFF44) == memoryRef->DmaRead(0xFF45))
        {
            STAT.set(2);
            if(STAT.test(6)) {
                memoryRef->RequestInterupt(1);
            }
        }
        else
        {
            STAT.reset(2);
        }

        memoryRef->DmaWrite(STAT_ADR, STAT.to_ulong());
    }

    std::bitset<8> PPU::getLCDC() {
        return std::bitset<8>(memoryRef->DmaRead(LCDC_ADR));
    }

    std::bitset<8> PPU::getStat() {
        return std::bitset<8>(memoryRef->DmaRead(STAT_ADR));
    }
}
