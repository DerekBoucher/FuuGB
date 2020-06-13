#include "Core.h"
#include "PPU.h"

namespace FuuGB
{
    PPU::PPU(SDL_Window* windowRef, Memory* mem)
    {
        memoryRef = mem;
        renderer = SDL_GetRenderer(windowRef);
        if (renderer == NULL)
            renderer = SDL_CreateRenderer(windowRef, -1, SDL_RENDERER_SOFTWARE);
        
#ifdef FUUGB_DEBUG
        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = 160 * SCALE_FACTOR;
        viewport.h = 144 * SCALE_FACTOR;
        SDL_RenderSetViewport(renderer, &viewport);
#endif

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
        LCDC = GetLCDC();
        STAT = GetSTAT();

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
        LCDC = GetLCDC();

        if(!LCDC.test(7))
            return;
        
        SDL_RenderPresent(renderer);
    }


    void PPU::UpdateGraphics(int cycles)
    {
        LCDC = GetLCDC();
        setLCDStatus();
        
        if(LCDC.test(7))
        {
            scanlineCounter -= cycles;
        }
        else
            return;
        
        if(scanlineCounter <= 0) //Time to render new frame
        {
            currentScanline = memoryRef->DMA_read(0xFF44);
            
            scanlineCounter = 456;
            
            if(currentScanline < 144) {
                drawScanline();
            }
            else if(currentScanline > 144 && currentScanline < 153) {
                memoryRef->RequestInterupt(0);
            }
            else if (currentScanline == 153)
            {
                memoryRef->DMA_write(0xFF44, 0x00);
                return;
            }
            
            memoryRef->DMA_write(0xFF44, memoryRef->DMA_read(0xFF44)+1);
        }
    }
    
    void PPU::drawScanline()
    {
        LCDC = GetLCDC();
            
        if(LCDC.test(0))
            renderTiles();
        
        if(LCDC.test(1))
            renderSprites();
    }
    
    void PPU::renderTiles()
    {
        LCDC = GetLCDC();
        uWORD Tile_Data_Ptr = 0x0;
        uWORD Tile_Map_Ptr = 0x0;
        uWORD Win_Map_Ptr = 0x0;
        
        bool WinEnabled = false;
        bool unsigned_ID = false;
        
        // Determine The offsets to use when retrieving the Tile Identifiers from
        // Tile Map address space.
        uBYTE ScrollX = memoryRef->DMA_read(0xFF43);
        uBYTE ScrollY = memoryRef->DMA_read(0xFF42);
        uBYTE WinX = memoryRef->DMA_read(0xFF4B) - 7;
        uBYTE WinY = memoryRef->DMA_read(0xFF4A);
        
        
        // Determine Address of Tile Data depending on the LCDC bit 4
        if(LCDC.test(4))
        {
            Tile_Data_Ptr = 0x8000;
            unsigned_ID = true;
        }
        else
        {
            Tile_Data_Ptr = 0x8800;
            unsigned_ID = false;
        }
        
        // Determine the Address of the Tile Mapping For BG & Window
        if(LCDC.test(3))
            Tile_Map_Ptr = 0x9C00;
        else
            Tile_Map_Ptr = 0x9800;
        
        // Determine if the window is to be rendered
        // The window is rendered above the background
        if(LCDC.test(5))
            WinEnabled = true;
        else
            WinEnabled = false;
        
        // If Window is to be rendered, determine its Maping ptr in MemoryUnit
        if(WinEnabled)
        {
            if(LCDC.test(6))
                Win_Map_Ptr = 0x9C00;
            else
                Win_Map_Ptr = 0x9800;
        }
        
        // Determine the current scanline we are on
        currentScanline = memoryRef->DMA_read(0xFF44);
        uWORD yPos;
        if (!WinEnabled)
            yPos = ScrollY + currentScanline;
        else
            yPos = currentScanline - WinY;

        uWORD Tile_Row = (yPos / 8) * 32;
        
        // Start Rendering the scanline
        for(int pixel = 0; pixel < 160; pixel++) {
            if(currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159)
                continue;

            uWORD xPos = pixel + ScrollX;
            if(WinEnabled)
            {
                if (pixel >= WinX)
                {
                    xPos = pixel - WinX;
                }
            }

            uWORD Tile_Col = xPos / 8;

            //Determine the address for the tile identifier
            uWORD current_Tile_Map_Adr = Tile_Map_Ptr + Tile_Col + Tile_Row;
            
            //Determine the Tile ID
            sBYTE sTile_ID;
            uBYTE uTile_ID;
            if(unsigned_ID)
                uTile_ID = memoryRef->DMA_read(current_Tile_Map_Adr);
            else
                sTile_ID = memoryRef->DMA_read(current_Tile_Map_Adr);
            
            //Determine the current pixel data from the tile data
            uBYTE Tile_Line_offset = (yPos % 8) * 2; //Each line is 2 bytes
            uWORD current_uTile_Data_adr;
            sWORD current_sTile_Data_adr;
            uBYTE data1, data2;
            if(unsigned_ID)
            {
                current_uTile_Data_adr = Tile_Data_Ptr + (uTile_ID)*16;
                data1 = memoryRef->DMA_read(current_uTile_Data_adr+Tile_Line_offset);
                data2 = memoryRef->DMA_read(current_uTile_Data_adr+Tile_Line_offset+1);
            }
            else
            {
                current_sTile_Data_adr = Tile_Data_Ptr + (sTile_ID)*16;
                data1 = memoryRef->DMA_read(current_sTile_Data_adr+Tile_Line_offset);
                data2 = memoryRef->DMA_read(current_sTile_Data_adr+Tile_Line_offset+1);
            }
            
            int currentBitPosition = (((pixel % 8) - 7)* -1);
            
            std::bitset<8> d1(data1);
            std::bitset<8> d2(data2);
            
            std::bitset<2> ColorCode;
            if(d2.test(currentBitPosition))
                ColorCode.set(1);
            else
                ColorCode.reset(1);
            
            if(d1.test(currentBitPosition))
                ColorCode.set(0);
            else
                ColorCode.reset(0);
            
            int R = 0x0;
            int G = 0x0;
            int B = 0x0;

            std::bitset<2> Color_00(memoryRef->DMA_read(0xFF47) & 0x03);
            std::bitset<2> Color_01((memoryRef->DMA_read(0xFF47)>>2) & 0x03);
            std::bitset<2> Color_10((memoryRef->DMA_read(0xFF47)>>4) & 0x03);
            std::bitset<2> Color_11((memoryRef->DMA_read(0xFF47)>>6) & 0x03);
            
            //Determine actual color for pixel via Color Pallete register
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
                continue;
            
            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);
        }
    }
    
    void PPU::renderSprites()
    {
        LCDC = GetLCDC();
        
        bool u_8x16 = false;
        
        if(LCDC.test(2))
            u_8x16 = true;
        
        for (int sprite = 0; sprite < 40; sprite++)
        {
            uBYTE index = sprite*4;
            uBYTE yPos = memoryRef->DMA_read(0xFE00 + index);
            uBYTE xPos = memoryRef->DMA_read(0xFE00 + index + 1);
            uBYTE tilelocation = memoryRef->DMA_read(0xFFE0 + index + 2);
            uBYTE attributes = memoryRef->DMA_read(0xFFE0 + index + 3);
            
            std::bitset<8> attr(attributes);
            
            bool yFlip = attr.test(6);
            bool xFlip = attr.test(5);
            
            currentScanline = memoryRef->DMA_read(0xFF44);
            
            int ysize = 8;
            if(u_8x16)
                ysize = 16;
            
            if((currentScanline >= yPos) && (currentScanline < (yPos + ysize)))
            {
                int line = currentScanline - yPos;
                
                if(yFlip)
                {
                    line -= ysize;
                    line *= -1;
                }
                
                line *= 2;
                
                uWORD dataaddr = (0x8000 + (tilelocation * 16)) + line;
                uBYTE data1 = memoryRef->DMA_read(dataaddr);
                uBYTE data2 = memoryRef->DMA_read(dataaddr + 1);
                
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
                    
                    std::bitset<2> Color_00(memoryRef->DMA_read(coloradr) & 0x03);
                    std::bitset<2> Color_01((memoryRef->DMA_read(coloradr) >> 2) & 0x03);
                    std::bitset<2> Color_10((memoryRef->DMA_read(coloradr) >> 4) & 0x03);
                    std::bitset<2> Color_11((memoryRef->DMA_read(coloradr) >> 6) & 0x03);
                    
                    //Determine actual color for pixel via Color Pallete register
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
                        continue;
                    
                    int xPix = 0 - tilepixel;
                    
                    xPix += 7;
                    
                    int pixel = xPos+xPix;
                    
                    SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
                    SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
                    SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);
                }
            }
        }
    }
    
    void PPU::setLCDStatus()
    {
        LCDC = GetLCDC();
        STAT = GetSTAT();
        
        if(!LCDC.test(7))
        {
            scanlineCounter = 456;
            memoryRef->DMA_write(0xFF44, 0x00);
            STAT.reset(0);
            STAT.reset(0);
            memoryRef->DMA_write(STAT_ADR, STAT.to_ulong());
            return;
        }
        
        currentScanline = memoryRef->DMA_read(0xFF44);
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
            
        if(memoryRef->DMA_read(0xFF44) == memoryRef->DMA_read(0xFF45))
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

        memoryRef->DMA_write(STAT_ADR, STAT.to_ulong());
    }

    std::bitset<8> PPU::GetLCDC() {
        return std::bitset<8>(memoryRef->DMA_read(LCDC_ADR));
    }

    std::bitset<8> PPU::GetSTAT() {
        return std::bitset<8>(memoryRef->DMA_read(STAT_ADR));
    }
}
