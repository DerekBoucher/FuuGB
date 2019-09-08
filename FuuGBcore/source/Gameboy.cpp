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
		running = true;
		this->memory = new Memory(cart);
		this->ppu = new PPU(windowPtr, this->memory, false);
		this->cpu = new CPU(this->memory);
		globalPause = false;
		_gameboyTHR = new std::thread(&Gameboy::Run, this);
	}

	Gameboy::~Gameboy()
	{
		running = false;
		cpu->stop();
		_gameboyTHR->join();
		delete _gameboyTHR;
		delete ppu;
		delete memory;
		delete cpu;
	}

	void Gameboy::Run()
	{
		while (running)
		{
			const int MAXCYCLES = 69905;
			int cyclesthisupdate = 0;
			while (cyclesthisupdate <= MAXCYCLES)
			{
                int cycles = 0;
				if (globalPause)
				{
					std::unique_lock<std::mutex> pauseLock(Shared::mu_GB);
					Shared::cv_GB.wait(pauseLock);
					pauseLock.unlock();
					globalPause = false;
				}
				if (cpu->_cpuHalted)
				{
					cpu->halt();
					cycles = 4;
				}
                else
                    cycles = cpu->executeNextOpCode();
                cyclesthisupdate += cycles;
				cpu->updateTimers(cycles);
                ppu->updateGraphics(cycles);
                if(!cpu->_cpuHalted)
                    cpu->checkInterupts();
			}
            ppu->renderscreen();
		}
	}

	void Gameboy::Pause()
	{
		globalPause = true;
	}

	void Gameboy::Resume()
	{
		Shared::cv_GB.notify_all();
	}
}
