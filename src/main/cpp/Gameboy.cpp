#include "Fuupch.h"
#include "Gameboy.h"

namespace FuuGB
{
    Gameboy::Gameboy(SDL_Window* windowPtr, Cartridge* cart)
    {
        running = true;
        this->MemoryUnit = new Memory(cart);
        this->ppu = new PPU(windowPtr, this->MemoryUnit, false);
        this->cpu = new CPU(this->MemoryUnit);
        globalPause = false;
        _gameboyTHR = new std::thread(&Gameboy::Run, this);
    }

    Gameboy::~Gameboy()
    {
        running = false;
        _gameboyTHR->join();
        delete _gameboyTHR;
        delete ppu;
        delete MemoryUnit;
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
                if (cpu->CpuHalted)
                {
                    cpu->Halt();
                    cycles = 4;
                }
                else
                    cycles = cpu->ExecuteNextOpCode();
                cyclesthisupdate += cycles;
                cpu->UpdateTimers(cycles);
                ppu->updateGraphics(cycles);
                if(!cpu->CpuHalted)
                    cpu->CheckInterupts();
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
