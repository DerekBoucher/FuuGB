//
//  PPU.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#include "Fuupch.h"
#include "PPU.h"

namespace FuuGB
{
	PPU::PPU(SDL_Window* windowRef, Memory* mem)
	{
		MEM = mem;
		_ppuRunning = true;
		renderer = SDL_GetRenderer(windowRef);
		if (renderer == NULL)
			renderer = SDL_CreateRenderer(windowRef, -1, SDL_RENDERER_ACCELERATED);
        
        SDL_RenderClear(renderer);

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
		//_ppuTHR = new std::thread(&PPU::clock, this);
		OAM_Pointer = 0x8800;
        currentScanLine = 1;
        scanline_counter = 456;
		FUUGB_PPU_LOG("PPU Initialized.");
	}

	PPU::~PPU()
	{
		//_ppuTHR->join();
		//ppuCond.notify_all();
		//delete _ppuTHR;
		SDL_DestroyRenderer(this->renderer);
		FUUGB_PPU_LOG("PPU Destroyed.");
	}

	void PPU::stop()
	{
		_ppuRunning = false;
	}

	void PPU::clock() //Obsolete - Not in use currently
	{
		while (_ppuRunning)
		{
			std::bitset<8> LCDC(MEM->DMA_read(0xFF40));
			std::bitset<8> STAT(MEM->DMA_read(0xFF41));
			std::bitset<8> SCY(MEM->DMA_read(0xFF42));
			std::bitset<8> SCX(MEM->DMA_read(0xFF43));
			std::bitset<8> LY(MEM->DMA_read(0xFF44));
			std::bitset<8> LYC(MEM->DMA_read(0xFF45));
			std::bitset<8> DMA(MEM->DMA_read(0xFF46));
			std::bitset<8> OBP0(MEM->DMA_read(0xFF48));
			std::bitset<8> OBP1(MEM->DMA_read(0xFF49));
			std::bitset<8> WY(MEM->DMA_read(0xFF4A));
			std::bitset<8> WX(MEM->DMA_read(0xFF4B));

			if (LCDC.test(4))
				BGW_Pointer = 0x8000;
			else
				BGW_Pointer = 0x8800;

			if (LCDC.test(3))
				BG_Map_Pointer = 0x9C00;
			else
				BG_Map_Pointer = 0x9800;

			int frametime;
			const int FPS = 60;
			const int frameDelay = 1000 / FPS;
			Uint32 framestart;
			framestart = SDL_GetTicks();
			//if (LCDC.test(7))
				//updateGraphics();
			frametime = SDL_GetTicks() - framestart;
			if (frameDelay > frametime)
			{
				SDL_Delay(frameDelay - frametime);
			}
		}
	}

	void PPU::updateGraphics(int cycles)
	{
        std::bitset<8> LCDC(MEM->readMemory(0xFF40));
        setLCDStatus();
        
        if(LCDC.test(7))
        {
            scanline_counter -= cycles;
        }
        else
            return;
        
        if(scanline_counter <= 0) //Time to render new frame
        {
            currentScanLine = MEM->DMA_read(0xFF44);
            
            scanline_counter = 456;
            
            if(currentScanLine == 144)//V-Blank
                MEM->RequestInterupt(0);
            else if(currentScanLine > 153)
                MEM->DMA_write(0xFF44, 0x00);
            else
                this->DrawScanLine();
            
            MEM->DMA_write(0xFF44, MEM->DMA_read(0xFF44)+1);
        }
	}
    
    void PPU::DrawScanLine()
    {
        std::bitset<8> LCDC(MEM->DMA_read(0xFF40));
            
        if(LCDC.test(0))
            RenderTiles();
        
        if(LCDC.test(1))
            RenderSprites();
    }
    
    void PPU::RenderTiles()
    {
        std::bitset<8> LCDC(MEM->DMA_read(0xFF40));
        uWORD BGW_Data_Ptr = 0x0;
        uWORD BG_Map_Ptr = 0x0;
        
        //Determine The X-Y Coordinates to draw from the BG-Window tile map
        uBYTE ScrollX = MEM->readMemory(0xFF43);
        uBYTE ScrollY = MEM->readMemory(0xFF42);
        uBYTE WinX = MEM->readMemory(0xFF4B) - 7;
        uBYTE WinY = MEM->readMemory(0xFF4A);
        
        bool winInRenderTarget = false;
        bool unsignedAdr = false;
        //Determine if Window is to be rendered (It's Y-Coordinate is less than currentscanline
        if(LCDC.test(5))
        {
            if(WinY <= MEM->readMemory(0xFF44))
            {
                winInRenderTarget = true;
            }
        }
        
        //Determine the Tile data set to be used
        if(LCDC.test(4))
        {
            BGW_Data_Ptr = 0x8000;
            unsignedAdr = true;
        }
        else
        {
            BGW_Data_Ptr = 0x8800;
            unsignedAdr = false;
        }
        
        //Determine Win Map Address
        if(winInRenderTarget)
        {
            if(LCDC.test(6))
                BG_Map_Ptr = 0x9C00;
            else
                BG_Map_Ptr = 0x9800;
        }
        else
        {
        //Determine BG Map Address
            if(LCDC.test(3))
                BG_Map_Ptr = 0x9C00;
            else
                BG_Map_Ptr = 0x9800;
        }
        
        //Determine the Y-Pos of the tile that the current scan line is on
        //Also differentiate if the tile is a window tile or BG tile
        uBYTE Tile_ypos = 0x00;
        if(!winInRenderTarget)
            Tile_ypos = ScrollY + MEM->readMemory(0xFF44);
        else
            Tile_ypos = MEM->readMemory(0xFF44) - WinY;
        
        //Determine the Row of the tile on which the scanline is on
        uBYTE Tile_Row = (Tile_ypos/8)*32;
        
        //Render the Scanline
        for(int pixel = 0;pixel < 160; pixel++)
        {
            uBYTE xPos = pixel + ScrollX;
            
            //If Window is currently in the scanline, translate to window address space
            if(winInRenderTarget)
            {
                //Check if xPos is >= Winx
                if(xPos >= WinX)
                {
                    xPos = pixel - WinX;
                }
            }
            
            //Determine in which of the 32 horizontal tiles where xPos lies
            uWORD tileCol = xPos/8;
            sWORD tilenum;
            
            //Obtain the Tile identifier number from the BG MAP
            uWORD tileAdr = BG_Map_Ptr + tileCol + Tile_Row;
            if(unsignedAdr)
                tilenum = (uBYTE)MEM->readMemory(tileAdr);
            else
                tilenum = (sBYTE)MEM->readMemory(tileAdr);
            
            //Determine Tile Location in the actual data tables
            uWORD tileLocation = BGW_Data_Ptr;
            if(unsignedAdr)
                tileLocation += tilenum*16;
            else
                tileLocation += (tilenum+128)*16; //Since the identifier is unsigned, must add a 128 offset
            
            //Determine which vertical line of the tile we are on
            uBYTE line = (Tile_ypos % 8) * 2;
            uBYTE data1 = MEM->readMemory(tileLocation + line);
            uBYTE data2 = MEM->readMemory(tileLocation + line + 1);
           
            if(data1 != 0x00 || data2 != 0x00)
                printf("");
            
            //Determine The color encoding for the current pixel
            int ColorBit = xPos % 8;
            ColorBit -= 7;
            ColorBit *= -1;
            
            std::bitset<8> d1(data1);
            std::bitset<8> d2(data2);
            
            int ColorEncoding = d2.test(ColorBit);
            ColorEncoding <<= 1;
            ColorEncoding |= d1.test(ColorBit);
            
            int R = 0x0;
            int G = 0x0;
            int B = 0x0;
            
            std::bitset<2> Color_00(MEM->readMemory(0xFF47) & 0x03);
            std::bitset<2> Color_01(MEM->readMemory(0xFF47) & 0x0C);
            std::bitset<2> Color_10(MEM->readMemory(0xFF47) & 0x30);
            std::bitset<2> Color_11(MEM->readMemory(0xFF47) & 0xC0);
            
            //Determine actual color for pixel via Color Pallete register
            switch(ColorEncoding)
            {
                case 0x00:
                    if(Color_00.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_00.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                    else if(Color_00.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                    else if(Color_00.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                    break;
                case 0x01:
                    if(Color_01.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_01.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                    else if(Color_01.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                    else if(Color_01.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                    break;
                case 0x10:
                    if(Color_10.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_10.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                    else if(Color_10.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                    else if(Color_10.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                    break;
                case 0x11:
                    if(Color_11.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                    else if(Color_11.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                    else if(Color_11.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                    else if(Color_11.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                    break;
            }
            
            currentScanLine = MEM->readMemory(0xFF44);
            
            if(currentScanLine < 0 || currentScanLine > 143 || pixel < 0 || pixel > 159)
                continue;
            
            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][currentScanLine]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][currentScanLine]);
        }
    }
    
    void PPU::RenderSprites()
    {
        std::bitset<8> LCDC(MEM->readMemory(0xFF40));
        
        bool u_8x16 = false;
        
        if(LCDC.test(2))
            u_8x16 = true;
        
        for (int sprite = 0; sprite < 40; sprite++)
        {
            uBYTE index = sprite*4;
            uBYTE yPos = MEM->readMemory(0xFE00 + index) - 16;
            uBYTE xPos = MEM->readMemory(0xFE00 + index +1) - 8;
            uBYTE tilelocation = MEM->readMemory(0xFFE0 + index + 2);
            uBYTE attributes = MEM->readMemory(0xFFE0 + index + 3);
            
            std::bitset<8> attr(attributes);
            
            bool yFlip = attr.test(6);
            bool xFlip = attr.test(5);
            
            int scanline = MEM->readMemory(0xFF44);
            
            int ysize = 8;
            if(u_8x16)
                ysize = 16;
            
            if((scanline >= yPos) && (scanline < (yPos + ysize)))
            {
                int line = scanline - yPos;
                
                if(yFlip)
                {
                    line -= ysize;
                    line *= -1;
                }
                
                line *= 2;
                
                uWORD dataaddr = (0x8000 + (tilelocation * 16)) + line;
                uBYTE data1 = MEM->readMemory(dataaddr);
                uBYTE data2 = MEM->readMemory(dataaddr + 1);
                
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
                    
                    std::bitset<2> Color_00(MEM->readMemory(coloradr) & 0x03);
                    std::bitset<2> Color_01(MEM->readMemory(coloradr) & 0x0C);
                    std::bitset<2> Color_10(MEM->readMemory(coloradr) & 0x30);
                    std::bitset<2> Color_11(MEM->readMemory(coloradr) & 0xC0);
                    
                    //Determine actual color for pixel via Color Pallete register
                    switch(ColorEncoding)
                    {
                        case 0x00:
                            if(Color_00.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_00.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_00.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                            else if(Color_00.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x01:
                            if(Color_01.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_01.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_01.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                            else if(Color_01.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x10:
                            if(Color_10.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_10.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_10.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                            else if(Color_10.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                            break;
                        case 0x11:
                            if(Color_11.to_ulong() == 0x00) { R = 255; G = 255; B = 255; }
                            else if(Color_11.to_ulong() == 0x01) { R = 211; G = 211; B = 211; }
                            else if(Color_11.to_ulong() == 0x10) { R = 169; G = 169; B = 169; }
                            else if(Color_11.to_ulong() == 0x11) { R = 0; G = 0; B = 0; }
                            break;
                    }
                    
                    if(R == 255 && G == 255 && B == 255)
                        continue;
                    
                    int xPix = 0 - tilepixel;
                    
                    xPix += 7;
                    
                    int pixel = xPos+xPix;
                    
                    if((scanline<0) || (scanline>143) || (pixel <0) || (pixel > 159))
                        continue;
                    
                    SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
                    SDL_RenderFillRect(renderer, &pixels[pixel][scanline]);
                    SDL_RenderDrawRect(renderer, &pixels[pixel][scanline]);
                }
            }
        }
    }
    
    void PPU::setLCDStatus()
    {
        std::bitset<8> LCDC(MEM->readMemory(0xFF40));
        uBYTE status = MEM->readMemory(0xFF41);
        std::bitset<8> STAT(status);
        
        if(!LCDC.test(7))
        {
            scanline_counter = 456;
            MEM->DMA_write(0xFF44, 0x00);
            status &= 252;
            status |= 0x01;
            MEM->writeMemory(0xFF41, status);
            return;
        }
        
        currentScanLine = MEM->readMemory(0xFF44);
        uBYTE currentMode = status & 0x3;
        
        uBYTE mode = 0;
        bool reqInt = false;
        
        if(currentScanLine >= 144)
        {
            mode = 1;
            STAT.set(1);
            STAT.reset(0);
            reqInt = STAT.test(5);
        }
        else
        {
            int mode2BOUND = 456 - 80;
            int mode3BOUND = mode2BOUND - 172;
            
            //Mode 2
            if(scanline_counter >= mode2BOUND)
            {
                mode = 2;
                STAT.set(1);
                STAT.reset(0);
                reqInt = STAT.test(5);
            }
            else if(scanline_counter >= mode3BOUND)
            {
                mode = 3;
                STAT.set(1);
                STAT.set(0);
            }
            else
            {
                mode = 0;
                STAT.reset(1);
                STAT.reset(0);
                reqInt = STAT.test(3);
            }
            
            if(reqInt && (mode != currentMode))
            {
                MEM->RequestInterupt(1);
            }
            
            if(MEM->readMemory(0xFF44) == MEM->readMemory(0xFF45))
            {
                STAT.set(2);
                if(STAT.test(6))
                {
                    MEM->RequestInterupt(1);
                }
            }
            else
            {
                STAT.reset(2);
            }
            MEM->writeMemory(0xFF41, (uBYTE)STAT.to_ulong());
        }
    }
    
    void PPU::renderscreen()
    {
        std::bitset<8> LCDC(MEM->readMemory(0xFF40));
        
        if(!LCDC.test(7))
            return;
        
        SDL_RenderPresent(renderer);
    }
}
