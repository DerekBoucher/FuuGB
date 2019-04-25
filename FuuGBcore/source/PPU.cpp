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
	PPU::PPU(SDL_Window* windowRef)
	{
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

		_ppuTHR = new std::thread(&PPU::clock, this);
		renderScreen();
		FUUGB_PPU_LOG("PPU Initialized.");
	}

	PPU::~PPU()
	{
		_ppuTHR->join();
		ppuCond.notify_all();
		delete _ppuTHR;
		SDL_DestroyRenderer(this->renderer);
		FUUGB_PPU_LOG("PPU Destroyed.");
	}

	void PPU::stop()
	{
		_ppuRunning = false;
	}

	void PPU::clock()
	{
		while (_ppuRunning)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(PPU_CLOCK_PERIOD_NS));
			ppuCond.notify_all();
		}
	}

	void PPU::renderScreen()
	{
		srand(time(NULL));
		for (int i = 0;i < NATIVE_SIZE_X; ++i)
		{
			for (int j = 0; j < NATIVE_SIZE_Y; ++j)
			{
				SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(renderer, &pixels[i][j]);
                SDL_RenderFillRect(renderer, &pixels[i][j]);
				SDL_RenderDrawRect(renderer, &pixels[i][j]);
			}
		}
		SDL_RenderPresent(renderer);
	}
}
