/*
 * Gameboy.cpp
 *
 *  Created on: Dec 29, 2018
 *      Author: Derek Boucher
 *		ID:40015459
 */

#include "Fuupch.h"
#include "Gameboy.h"

namespace FuuGB
{
	Gameboy::Gameboy(SDL_Window* windowPtr, Cartridge* cart)
	{
		this->ppu = new PPU(windowPtr);
		this->memory = new Memory(cart);
		this->cpu = new CPU(this->memory);
	}

	Gameboy::~Gameboy()
	{
		ppu->stop();
		memory->stop();
		cpu->stop();
		delete ppu;
		delete memory;
		delete cpu;
	}

	void Gameboy::Pause()
	{
		cpu->Pause();
	}

	void Gameboy::Resume()
	{
		Shared::cv_GB.notify_all();
	}
}