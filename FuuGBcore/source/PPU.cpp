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
		OAM_Pointer = 0x8800;
        currentScanLine = 1;
        scanline_counter = 456;
		FUUGB_PPU_LOG("PPU Initialized.");
		test = 0x64;
	}

	PPU::~PPU()
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
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
			else if (currentScanLine > 153)
			{
				MEM->DMA_write(0xFF44, 0x00);
				return;
			}
            else if(currentScanLine < 144)
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
        uWORD Tile_Data_Ptr = 0x0;
        uWORD Tile_Map_Ptr = 0x0;
        uWORD Win_Map_Ptr = 0x0;
        
        bool WinEnabled = false;
        bool unsigned_ID = false;
        
        //Determine The offsets to use when retrieving the Tile Identifiers from
        //Tile Map address space.
        uBYTE ScrollX = MEM->readMemory(0xFF43);
        uBYTE ScrollY = MEM->readMemory(0xFF42);
        uBYTE WinX = MEM->readMemory(0xFF4B) - 7;
        uBYTE WinY = MEM->readMemory(0xFF4A);
        
        
        //Determine Address of Tile Data depending on the LCDC bit 4
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
        
        //Determine the Address of the Tile Mapping For BG & Window
        if(LCDC.test(3))
            Tile_Map_Ptr = 0x9C00;
        else
            Tile_Map_Ptr = 0x9800;
        
        //Determine if the window is to be rendered
        //The window has is rendered above the background
        if(LCDC.test(5))
            WinEnabled = true;
        else
            WinEnabled = false;
        
        //If Window is to be rendered, determine its Maping ptr in memory
        if(WinEnabled)
        {
            if(LCDC.test(6))
                Win_Map_Ptr = 0x9C00;
            else
                Win_Map_Ptr = 0x9800;
        }
        
        //Determine the current scanline we are on
        int current_Scanline = MEM->readMemory(0xFF44);
		uWORD yPos = ScrollY + current_Scanline;
		uWORD Tile_Row = (yPos / 8)* 32;
        //Start Rendering the scanline
        for(int pixel = 0;pixel < 160; pixel++)
        {
            if(WinEnabled)
            {
                //To do
            }
            
			uWORD xPos = pixel + ScrollX;
			uWORD Tile_Col = xPos / 8;

            //Determine the address for the tile identifier
			uWORD current_Tile_Map_Adr = Tile_Map_Ptr + Tile_Col + Tile_Row;
            
            //Determine the Tile ID
            sBYTE sTile_ID;
            uBYTE uTile_ID;
            if(unsigned_ID)
                uTile_ID = MEM->readMemory(current_Tile_Map_Adr);
            else
                sTile_ID = MEM->readMemory(current_Tile_Map_Adr);
            
            //Determine the current pixel data from the tile data
            uBYTE Tile_Line_offset = (yPos % 8) * 2; //Each line is 2 bytes
            uWORD current_uTile_Data_adr;
            sWORD current_sTile_Data_adr;
            uBYTE data1, data2;
            if(unsigned_ID)
            {
                current_uTile_Data_adr = Tile_Data_Ptr + (uTile_ID)*16;
                data1 = MEM->readMemory(current_uTile_Data_adr+Tile_Line_offset);
                data2 = MEM->readMemory(current_uTile_Data_adr+Tile_Line_offset+1);
            }
            else
            {
                current_sTile_Data_adr = Tile_Data_Ptr + (sTile_ID)*16;
                data1 = MEM->readMemory(current_sTile_Data_adr+Tile_Line_offset);
                data2 = MEM->readMemory(current_sTile_Data_adr+Tile_Line_offset+1);
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

            std::bitset<2> Color_00(MEM->readMemory(0xFF47) & 0x03);
            std::bitset<2> Color_01((MEM->readMemory(0xFF47)>>2) & 0x03);
            std::bitset<2> Color_10((MEM->readMemory(0xFF47)>>4) & 0x03);
            std::bitset<2> Color_11((MEM->readMemory(0xFF47)>>6) & 0x03);
            
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
            
            if(current_Scanline < 0 || current_Scanline > 143 || pixel < 0 || pixel > 159)
                continue;
            
            SDL_SetRenderDrawColor(renderer, R, G, B, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixels[pixel][current_Scanline-1]);
            SDL_RenderDrawRect(renderer, &pixels[pixel][current_Scanline-1]);
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
                    std::bitset<2> Color_01((MEM->readMemory(coloradr) >> 2) & 0x03);
                    std::bitset<2> Color_10((MEM->readMemory(coloradr) >> 4) & 0x03);
                    std::bitset<2> Color_11((MEM->readMemory(coloradr) >> 6) & 0x03);
                    
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
