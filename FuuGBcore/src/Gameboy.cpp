#include "Fuupch.h"
#include "Gameboy.h"

pthread_mutex_t FuuGB::Shared::mu_GB;
pthread_cond_t FuuGB::Shared::cv_GB;

namespace FuuGB
{
Gameboy::Gameboy(SDL_Window *windowPtr, Cartridge *cart)
{
	running = true;
	this->memory = new Memory(cart);
	this->ppu = new PPU(windowPtr, this->memory, false);
	this->cpu = new CPU(this->memory);
	globalPause = false;
	pthread_create(&_gameboyTHR, NULL, Run, (void *)this);
}

Gameboy::~Gameboy()
{
	running = false;
	cpu->stop();
	pthread_join(_gameboyTHR, 0);
	delete ppu;
	delete memory;
	delete cpu;
}

void *Gameboy::Run(void *arg)
{
	Gameboy *gameboy = (Gameboy *)arg;
	while (gameboy->running)
	{
		const int MAXCYCLES = 69905;
		int cyclesthisupdate = 0;
		while (cyclesthisupdate <= MAXCYCLES)
		{
			int cycles = 0;
			if (gameboy->globalPause)
			{
				pthread_mutex_lock(&FuuGB::Shared::mu_GB);
				pthread_cond_wait(&FuuGB::Shared::cv_GB, &FuuGB::Shared::mu_GB);
				pthread_mutex_unlock(&FuuGB::Shared::mu_GB);
				gameboy->globalPause = false;
			}
			if (gameboy->cpu->_cpuHalted)
			{
				gameboy->cpu->halt();
				cycles = 4;
			}
			else
				cycles = gameboy->cpu->executeNextOpCode();
			cyclesthisupdate += cycles;
			gameboy->cpu->updateTimers(cycles);
			gameboy->ppu->updateGraphics(cycles);
			if (!gameboy->cpu->_cpuHalted)
				gameboy->cpu->checkInterupts();
		}
		gameboy->ppu->renderscreen();
	}
	return 0;
}

void Gameboy::Pause()
{
	globalPause = true;
}

void Gameboy::Resume()
{
	pthread_cond_signal(&FuuGB::Shared::cv_GB);
}
} // namespace FuuGB
