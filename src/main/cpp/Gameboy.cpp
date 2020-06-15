#include "Core.h"
#include "Gameboy.h"

namespace FuuGB
{
    Gameboy::Gameboy(SDL_Window* windowPtr, Cartridge* cart)
    {
        running     = true;
        memoryUnit  = new Memory(cart);
        ppuUnit     = new PPU(windowPtr, this->memoryUnit);
        cpuUnit     = new CPU(this->memoryUnit);
        pause       = false;
        thread      = new std::thread(&Gameboy::Run, this);
    }

    Gameboy::~Gameboy()
    {
        running = false;
        thread->join();
        delete thread;
        delete ppuUnit;
        delete memoryUnit;
        delete cpuUnit;
    }

    void Gameboy::Run()
    {
        const int MAXCYCLES = 69905;

        while (running)
        {
            int cyclesThisUpdate = 0;
            while (cyclesThisUpdate <= MAXCYCLES)
            {
                int cycles = 0;
                if (pause)
                {
                    wait();
                }
                if (cpuUnit->Halted)
                {
                    cpuUnit->Halt();
                    cycles = 4;
                }
                else 
                {
                    cycles = cpuUnit->ExecuteNextOpCode();
                }

                cyclesThisUpdate += cycles;

                cpuUnit->UpdateTimers(cycles);
                ppuUnit->UpdateGraphics(cycles);
                memoryUnit->UpdateDmaCycles(cycles);

                if(!cpuUnit->Halted) 
                {
                    cpuUnit->CheckInterupts();
                }
            }
            ppuUnit->RenderScreen();
        }
    }

    void Gameboy::Pause()
    {
        pause = true;
    }

    void Gameboy::Resume()
    {
        Shared::cv_GB.notify_all();
    }

    void Gameboy::wait()
    {
        std::unique_lock<std::mutex> pauseLock(Shared::mu_GB);
        Shared::cv_GB.wait(pauseLock);
        pauseLock.unlock();
        pause = false;
    }
}
