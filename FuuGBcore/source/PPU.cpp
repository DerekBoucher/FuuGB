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

		for (int i = 0;i < NATIVE_SIZE_X; ++i)
		{
			for (int j = 0; j < NATIVE_SIZE_Y; ++j)
			{
				pixels[i][j].h = SCALE_FACTOR;
				pixels[i][j].w = SCALE_FACTOR;
				pixels[i][j].x = i * SCALE_FACTOR;
				pixels[i][j].y = j * SCALE_FACTOR;
			}
		}
		for (int i = 0;i < 160; ++i)
		{
			for (int j = 0; j < 144; ++j)
			{
				display[i][j].h = SCALE_FACTOR;
				display[i][j].w = SCALE_FACTOR;
				display[i][j].x = i * SCALE_FACTOR;
				display[i][j].y = j * SCALE_FACTOR;
			}
		}
		//_ppuTHR = new std::thread(&PPU::clock, this);
		OAM_Pointer = 0x8800;
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
			if (LCDC.test(7))
				updateGraphics();
			frametime = SDL_GetTicks() - framestart;
			if (frameDelay > frametime)
			{
				SDL_Delay(frameDelay - frametime);
			}
		}
	}

	void PPU::updateGraphics()
	{
		std::bitset<8> LCDC(MEM->DMA_read(0xFF40));

		if (LCDC.test(4))
			BGW_Pointer = 0x8000;
		else
			BGW_Pointer = 0x8800;

		if (LCDC.test(3))
			BG_Map_Pointer = 0x9C00;
		else
			BG_Map_Pointer = 0x9800;

		//Determine the Pixel Palette prior to rendering the frame
		std::bitset<8> BGP(MEM->DMA_read(0xFF47));
		uBYTE PixelColors[4][3]; //[0][x] = 00, [1][x] = 01, [2][x] = 10, [3][x] = 11
		for (int i = 0; i < 8; i+=2)
		{
			if (!BGP[i] & !BGP[i + 1])
			{
				PixelColors[i / 2][0] = 255; //8-bitx3 RGB encoding for WHITE
				PixelColors[i / 2][1] = 255;
				PixelColors[i / 2][2] = 255;
			}
			else if (!BGP[i] & BGP[i + 1])
			{
				PixelColors[i / 2][0] = 211; //8-Bitx3 RGB encoding for light Gray
				PixelColors[i / 2][1] = 211;
				PixelColors[i / 2][2] = 211;
			}
			else if (BGP[i] & !BGP[i + 1])
			{
				PixelColors[i / 2][0] = 169; //8-Bitx3 RGB encoding for Gray
				PixelColors[i / 2][1] = 169;
				PixelColors[i / 2][2] = 169;
			}
			else
			{
				PixelColors[i / 2][0] = 0; //8-bitx3 RGB encoding for black
				PixelColors[i / 2][1] = 0;
				PixelColors[i / 2][2] = 0;
			}
		}

		if (BGW_Pointer == 0x8000)
		{
			MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) & 0xFC);
			MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) | 0x03);
		}
		else
		{
			MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) & 0xFC);
			MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) | 0x02);
		}

		for (int i = 0;i < 32; ++i)
		{
			for (int j = 0; j < 32; ++j)
			{
				uWORD TileDataOffset = MEM->DMA_read(BG_Map_Pointer++);
				if (BGW_Pointer != 0x8000)
					TileDataOffset -= 128;
				for (int k = 0; k < 8; ++k)
				{
					uBYTE hiByte = MEM->DMA_read(BGW_Pointer + TileDataOffset * TILE_BYTE_LENGTH + k++);
					uBYTE loByte = MEM->DMA_read(BGW_Pointer + TileDataOffset * TILE_BYTE_LENGTH + k);
					std::bitset<8> hi(hiByte);
					std::bitset<8> lo(loByte);
					int dummyz = 0;
					for (int z = 7; z >= 0; --z)
					{
						if (!hi[z] & !lo[z])
						{
							//00
							SDL_SetRenderDrawColor(renderer, PixelColors[0][0], PixelColors[0][1], PixelColors[0][2], SDL_ALPHA_OPAQUE);
						}
						else if (!hi[z] & lo[z])
						{
							//01
							SDL_SetRenderDrawColor(renderer, PixelColors[1][0], PixelColors[1][1], PixelColors[1][2], SDL_ALPHA_OPAQUE);
						}
						else if (hi[z] & !lo[z])
						{
							//10
							SDL_SetRenderDrawColor(renderer, PixelColors[2][0], PixelColors[2][1], PixelColors[2][2], SDL_ALPHA_OPAQUE);
						}
						else if (hi[z] & lo[z])
						{
							//11
							SDL_SetRenderDrawColor(renderer, PixelColors[3][0], PixelColors[3][1], PixelColors[3][2], SDL_ALPHA_OPAQUE);
						}
						SDL_RenderFillRect(renderer, &pixels[k + i*8][dummyz++ + j*8]);
					}
				}
			}

			MEM->DMA_write(0xFF44, 144);
			//Produce V-Blank Interrupt if full frame rendered
			if (MEM->DMA_read(0xFF44) == 144)
			{
				MEM->DMA_write(0xFF0F, MEM->DMA_read(0xFF0F) | 0x01);
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) & 0xFC);
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) | 0x01);
			}


			if (MEM->DMA_read(0xFF44) == MEM->DMA_read(0xFF45))
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) | 0x04);
			else
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) & 0xFB);

			if (MEM->DMA_read(0xFF44) == 143)
			{
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) & 0xFC);
				MEM->DMA_write(0xFF41, MEM->DMA_read(0xFF41) | 0x00);
			}
		}

		int x = 0;
		int y = 0;
		for (int i = 256 - MEM->DMA_read(0xFF42); i < 160 + MEM->DMA_read(0xFF42); ++i)
		{
			for (int j = 256 - MEM->DMA_read(0xFF43); j < 144 + MEM->DMA_read(0xFF43); ++i)
			{
				display[x][y++] = pixels[i][j];
				display[x][y].x = x * SCALE_FACTOR;
				display[x][y].y = y * SCALE_FACTOR;
				SDL_RenderDrawRect(renderer, &display[x][y]);
			}
			++x;
		}
		if(LCDC.test(7))
			SDL_RenderPresent(renderer);
	}
}
