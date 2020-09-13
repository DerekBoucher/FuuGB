#include "Core.h"
#include "PPU.h"

namespace FuuGB
{
    PPU::PPU(SDL_Window *windowRef, Memory *mem)
    {
        memoryRef = mem;
        renderer = SDL_GetRenderer(windowRef);

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
                pixelData[i][j] = 0x00;
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

        if (!(LCDC & (1 << 7)))
            return;

        SDL_RenderPresent(renderer);
    }

    void PPU::UpdateGraphics(int cycles)
    {
        LCDC = getLCDC();

        setLCDStatus();

        if (LCDC & (1 << 7))
        {
            scanlineCounter -= cycles;
        }
        else
        {
            return;
        }

        if (scanlineCounter <= 0) // Time to render new frame
        {
            currentScanline = memoryRef->DmaRead(0xFF44);

            scanlineCounter = 456;

            if (currentScanline < 144)
            {
                drawScanline();
            }
            else if (currentScanline > 144 && currentScanline < 153)
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

        if (LCDC & (1 << 0))
        {
            renderTiles();
        }
        if ((LCDC & (1 << 5)) && (LCDC & (1 << 0)))
        {
            renderWindow();
        }
        if (LCDC & (1 << 1))
        {
            renderSprites();
        }
    }

    void PPU::renderTiles()
    {
        LCDC = getLCDC();

        uWORD tileDataPtr = 0x0;
        uWORD tileMapPtr = 0x0;

        bool unsignedID = false;

        // Determine The offsets to use when retrieving the Tile Identifiers from
        // Tile Map address space.
        uBYTE scrollX = memoryRef->DmaRead(0xFF43);
        uBYTE scrollY = memoryRef->DmaRead(0xFF42);

        // Determine base address of tile data for BG & window
        if (LCDC & (1 << 4))
        {
            tileDataPtr = 0x8000;
            unsignedID = true;
        }
        else
        {
            tileDataPtr = 0x9000;
            unsignedID = false;
        }

        // Determine the base address tile Mappings for BG & window
        if (LCDC & (1 << 3))
        {
            tileMapPtr = 0x9C00;
        }
        else
        {
            tileMapPtr = 0x9800;
        }

        // Determine the current scanline we are on
        currentScanline = memoryRef->DmaRead(0xFF44);

        // Calculate which row in the tile to render
        uBYTE yPos = scrollY + currentScanline;
        uWORD tileRow = (yPos / 8) * 32;

        // Start Rendering the scanline
        for (int pixel = 0; pixel < 160; pixel++)
        {
            if (currentScanline < 0 || currentScanline > 143)
            {
                continue;
            }

            uBYTE xPos = pixel + scrollX;

            uWORD tileColumn = xPos / 8;

            // Determine the address for the tile ID
            uWORD currentTileIDAdr = tileMapPtr + tileColumn + tileRow;

            // Fetch the tile ID
            uBYTE tileID = memoryRef->DmaRead(currentTileIDAdr);

            // Determine the current pixel data from the tile data
            uWORD tileLineOffset = (yPos % 8) * 2; //Each line is 2 bytes
            uWORD tileDataAdr;

            if (unsignedID)
            {
                tileDataAdr = tileDataPtr + (tileID * 16);
            }
            else
            {
                if (tileID & 0x80)
                {
                    tileID = ~tileID;
                    tileID += 1;
                    tileDataAdr = tileDataPtr - (tileID * 16);
                }
                else
                {
                    tileDataAdr = tileDataPtr + (tileID * 16);
                }
            }

            uBYTE data1 = memoryRef->DmaRead(tileDataAdr + tileLineOffset);
            uBYTE data2 = memoryRef->DmaRead(tileDataAdr + tileLineOffset + 1);

            int currentBitPosition = (((pixel % 8) - 7) * -1);

            uBYTE ColorCode = 0x00;

            if (data2 & (1 << currentBitPosition))
            {
                ColorCode |= 0x02;
            }

            if (data1 & (1 << currentBitPosition))
            {
                ColorCode |= 0x01;
            }

            uBYTE R = 0x00;
            uBYTE G = 0x00;
            uBYTE B = 0x00;

            uBYTE Color_00 = memoryRef->DmaRead(0xFF47) & 0x03;
            uBYTE Color_01 = ((memoryRef->DmaRead(0xFF47) >> 2) & 0x03);
            uBYTE Color_10 = ((memoryRef->DmaRead(0xFF47) >> 4) & 0x03);
            uBYTE Color_11 = ((memoryRef->DmaRead(0xFF47) >> 6) & 0x03);

            // Determine actual color for pixel via Color Pallete reg
            switch (ColorCode)
            {
            case 0x00:
                if (Color_00 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_00 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_00 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_00 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x01:
                if (Color_01 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_01 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_01 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_01 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x02:
                if (Color_10 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_10 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_10 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_10 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x03:
                if (Color_11 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_11 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_11 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_11 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            default:
                break;
            }

            if (currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159)
            {
                continue;
            }

            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);

            // Update pixel data
            pixelData[pixel][currentScanline] = ColorCode;
        }
    }

    void PPU::renderWindow()
    {
        LCDC = getLCDC();

        uBYTE winY = memoryRef->DmaRead(0xFF4A);
        uBYTE winX = memoryRef->DmaRead(0xFF4B) - 7;
        uWORD tileMapPtr = 0x9800;
        uWORD tileDataPtr = 0x9000;
        bool unsignedID = false;

        if (LCDC & (1 << 6))
        {
            tileMapPtr = 0x9C00;
        }

        if (LCDC & (1 << 4))
        {
            tileDataPtr = 0x8000;
            unsignedID = true;
        }

        // Determine the current scanline we are on
        currentScanline = memoryRef->DmaRead(0xFF44);

        // Calculate which row in the tile to render
        uBYTE yPos = winY + currentScanline;
        uWORD tileRow = (yPos / 8) * 32;

        // Start Rendering the scanline
        for (int pixel = 0; pixel < 160; pixel++)
        {
            uBYTE xPos = pixel + winX;

            uWORD tileColumn = xPos / 8;

            // Determine the address for the tile ID
            uWORD currentTileMapAdr = tileMapPtr + tileColumn + tileRow;

            // Fetch the tile ID
            uBYTE tileID = memoryRef->DmaRead(currentTileMapAdr);

            // Determine the current pixel data from the tile data
            uWORD tileLineOffset = (yPos % 8) * 2; //Each line is 2 bytes
            uWORD tileDataAdr;

            if (unsignedID)
            {
                tileDataAdr = tileDataPtr + (tileID * 16);
            }
            else
            {
                if (tileID & 0x80)
                {
                    tileID = ~tileID;
                    tileID += 1;
                    tileDataAdr = tileDataPtr - (tileID * 16);
                }
                else
                {
                    tileDataAdr = tileDataPtr + (tileID * 16);
                }
            }

            uBYTE data1 = memoryRef->DmaRead(tileDataAdr + tileLineOffset);
            uBYTE data2 = memoryRef->DmaRead(tileDataAdr + tileLineOffset + 1);

            int currentBitPosition = (((pixel % 8) - 7) * -1);

            uBYTE ColorCode = 0x00;

            if (data2 & (1 << currentBitPosition))
            {
                ColorCode |= 0x02;
            }

            if (data1 & (1 << currentBitPosition))
            {
                ColorCode |= 0x01;
            }

            uBYTE R = 0x00;
            uBYTE G = 0x00;
            uBYTE B = 0x00;

            uBYTE Color_00 = memoryRef->DmaRead(0xFF47) & 0x03;
            uBYTE Color_01 = ((memoryRef->DmaRead(0xFF47) >> 2) & 0x03);
            uBYTE Color_10 = ((memoryRef->DmaRead(0xFF47) >> 4) & 0x03);
            uBYTE Color_11 = ((memoryRef->DmaRead(0xFF47) >> 6) & 0x03);

            // Determine actual color for pixel via Color Pallete reg
            switch (ColorCode)
            {
            case 0x00:
                if (Color_00 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_00 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_00 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_00 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x01:
                if (Color_01 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_01 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_01 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_01 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x02:
                if (Color_10 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_10 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_10 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_10 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            case 0x03:
                if (Color_11 == 0x00)
                {
                    R = 245;
                    G = 245;
                    B = 245;
                }
                else if (Color_11 == 0x1)
                {
                    R = 211;
                    G = 211;
                    B = 211;
                }
                else if (Color_11 == 0x2)
                {
                    R = 169;
                    G = 169;
                    B = 169;
                }
                else if (Color_11 == 0x3)
                {
                    R = 0;
                    G = 0;
                    B = 0;
                }
                break;
            default:
                break;
            }

            if (currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159)
            {
                continue;
            }

            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][currentScanline]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanline]);

            // Update pixel data
            pixelData[pixel][currentScanline] = ColorCode;
        }
    }

    void PPU::renderSprites()
    {
        LCDC = getLCDC();

        bool u_8x16 = false;

        if (LCDC & (1 << 2))
        {
            u_8x16 = true;
        }

        sprite *sprites = processSprites();

        for (uBYTE i = 0; i < 40; i++)
        {
            bool yFlip = (sprites[i].attributes & (1 << 6));
            bool xFlip = (sprites[i].attributes & (1 << 5));
            bool priority = !(sprites[i].attributes & (1 << 7));

            currentScanline = memoryRef->DmaRead(0xFF44);

            uBYTE ysize = 8;

            if (u_8x16)
            {
                sprites[i].patternNumber &= 0xFE;
                ysize = 16;
            }

            if ((currentScanline >= sprites[i].yPos) && (currentScanline < (sprites[i].yPos + ysize)))
            {
                int line = currentScanline - sprites[i].yPos;

                if (yFlip)
                {
                    line -= ysize;
                    line *= -1;
                }

                line *= 2;

                uWORD dataaddr = (0x8000 + (sprites[i].patternNumber * 16)) + line;
                uBYTE data1 = memoryRef->DmaRead(dataaddr);
                uBYTE data2 = memoryRef->DmaRead(dataaddr + 1);

                for (int tilepixel = 7; tilepixel >= 0; tilepixel--)
                {
                    int ColorBit = tilepixel;

                    if (xFlip)
                    {
                        ColorBit -= 7;
                        ColorBit *= -1;
                    }

                    uBYTE ColorCode = 0x00;

                    if (data2 & (1 << ColorBit))
                    {
                        ColorCode |= 0x02;
                    }

                    if (data1 & (1 << ColorBit))
                    {
                        ColorCode |= 0x01;
                    }

                    uWORD coloradr = 0x0000;

                    if (sprites[i].attributes & (1 << 4))
                    {
                        coloradr = 0xFF49;
                    }
                    else
                    {
                        coloradr = 0xFF48;
                    }

                    uBYTE R = 0x0;
                    uBYTE G = 0x0;
                    uBYTE B = 0x0;

                    uBYTE Color_00 = (memoryRef->DmaRead(coloradr) & 0x03);
                    uBYTE Color_01 = ((memoryRef->DmaRead(coloradr) >> 2) & 0x03);
                    uBYTE Color_10 = ((memoryRef->DmaRead(coloradr) >> 4) & 0x03);
                    uBYTE Color_11 = ((memoryRef->DmaRead(coloradr) >> 6) & 0x03);

                    //Determine actual color for pixel via Color Pallete reg
                    switch (ColorCode)
                    {
                    case 0x00:
                        if (Color_00 == 0x00)
                        {
                            R = 245;
                            G = 245;
                            B = 245;
                        }
                        else if (Color_00 == 0x1)
                        {
                            R = 211;
                            G = 211;
                            B = 211;
                        }
                        else if (Color_00 == 0x2)
                        {
                            R = 169;
                            G = 169;
                            B = 169;
                        }
                        else if (Color_00 == 0x3)
                        {
                            R = 0;
                            G = 0;
                            B = 0;
                        }
                        break;
                    case 0x01:
                        if (Color_01 == 0x00)
                        {
                            R = 245;
                            G = 245;
                            B = 245;
                        }
                        else if (Color_01 == 0x1)
                        {
                            R = 211;
                            G = 211;
                            B = 211;
                        }
                        else if (Color_01 == 0x2)
                        {
                            R = 169;
                            G = 169;
                            B = 169;
                        }
                        else if (Color_01 == 0x3)
                        {
                            R = 0;
                            G = 0;
                            B = 0;
                        }
                        break;
                    case 0x02:
                        if (Color_10 == 0x00)
                        {
                            R = 245;
                            G = 245;
                            B = 245;
                        }
                        else if (Color_10 == 0x1)
                        {
                            R = 211;
                            G = 211;
                            B = 211;
                        }
                        else if (Color_10 == 0x2)
                        {
                            R = 169;
                            G = 169;
                            B = 169;
                        }
                        else if (Color_10 == 0x3)
                        {
                            R = 0;
                            G = 0;
                            B = 0;
                        }
                        break;
                    case 0x03:
                        if (Color_11 == 0x00)
                        {
                            R = 245;
                            G = 245;
                            B = 245;
                        }
                        else if (Color_11 == 0x1)
                        {
                            R = 211;
                            G = 211;
                            B = 211;
                        }
                        else if (Color_11 == 0x2)
                        {
                            R = 169;
                            G = 169;
                            B = 169;
                        }
                        else if (Color_11 == 0x3)
                        {
                            R = 0;
                            G = 0;
                            B = 0;
                        }
                        break;
                    default:
                        break;
                    }

                    if (R == 224 && G == 248 && B == 208)
                    {
                        continue;
                    }

                    int xPix = 0 - tilepixel - 1;

                    int pixel = sprites[i].xPos + xPix;

                    if (currentScanline < 0 || currentScanline > 143 || pixel < 0 || pixel > 159 || ColorCode == 0x00)
                    {
                        continue;
                    }

                    // Determine if sprite pixel has priority over background or window
                    if (!priority && pixelData[pixel][currentScanline] != 0x00)
                    {
                        continue;
                    }

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

        if (!(LCDC & (1 << 7)))
        {
            scanlineCounter = 456;
            memoryRef->DmaWrite(0xFF44, 0x00);
            STAT &= 0xFC;
            memoryRef->DmaWrite(STAT_ADR, STAT);
            return;
        }

        currentScanline = memoryRef->DmaRead(0xFF44);
        uBYTE currentMode = STAT & 0x03;

        uBYTE mode = 0;
        bool reqInt = false;

        // Mode 1
        if (currentScanline >= 144)
        {
            mode = 0x01;
            STAT &= 0xFC;
            STAT |= mode;
            reqInt = (STAT & (1 << 5));
        }
        else
        {
            int mode2BOUND = 456 - 80;
            int mode3BOUND = mode2BOUND - 172;

            // Mode 2
            if (scanlineCounter >= mode2BOUND)
            {
                mode = 0x02;
                STAT &= 0xFC;
                STAT |= mode;
                reqInt = (STAT & (1 << 5));
            }
            // Mode 3
            else if (scanlineCounter >= mode3BOUND)
            {
                mode = 0x03;
                STAT &= 0xFC;
                STAT |= mode;
            }
            // Mode 0
            else
            {
                mode = 0x00;
                STAT &= 0xFC;
                STAT |= mode;
                reqInt = (STAT & (1 << 3));
            }
        }

        if (reqInt && (mode != currentMode))
        {
            memoryRef->RequestInterupt(1);
        }

        if (memoryRef->DmaRead(0xFF44) == memoryRef->DmaRead(0xFF45))
        {
            STAT |= 0x04;
            if (STAT & (1 << 6))
            {
                memoryRef->RequestInterupt(1);
            }
        }
        else
        {
            STAT &= 0xFB;
        }

        memoryRef->DmaWrite(STAT_ADR, STAT);
    }

    uBYTE PPU::getLCDC()
    {
        return memoryRef->DmaRead(LCDC_ADR);
    }

    uBYTE PPU::getStat()
    {
        return memoryRef->DmaRead(STAT_ADR);
    }

    PPU::sprite *PPU::processSprites()
    {

        sprite *processedSprites = new sprite[40];

        for (uBYTE i = 40; i != 255; i--)
        {

            uBYTE index = i * 4;

            processedSprites[i].yPos = memoryRef->DmaRead(0xFE00 + index) - 0x10;
            processedSprites[i].xPos = memoryRef->DmaRead(0xFE00 + index + 1);
            processedSprites[i].patternNumber = memoryRef->DmaRead(0xFE00 + index + 2);
            processedSprites[i].attributes = memoryRef->DmaRead(0xFE00 + index + 3);
        }

        // for(uBYTE i = 0; i < 40; i++) {
        //     for(uBYTE j = 0; j < 40; j++) {
        //         if (i == j) {
        //             continue;
        //         }

        //         for (uBYTE k = 1; k < 8; k++) {
        //             if ((processedSprites[j].xPos + k) == processedSprites[i].xPos) {
        //                 temp = processedSprites[i];
        //                 processedSprites[i] = processedSprites[j];
        //                 processedSprites[j] = temp;
        //                 break;
        //             }
        //         }
        //     }
        // }

        return processedSprites;
    }
} // namespace FuuGB
