#include "Core.h"
#include "Gameboy.h"

namespace FuuGB
{
    Gameboy::Gameboy(SDL_Window* windowPtr, Cartridge* cart)
    {
        running = true;
        this->MemoryUnit = new Memory(cart);
        this->PpuUnit = new PPU(windowPtr, this->MemoryUnit, false);
        this->CpuUnit = new CPU(this->MemoryUnit);
        globalPause = false;
        GameboyThread = new std::thread(&Gameboy::Run, this);
    }

    Gameboy::~Gameboy()
    {
        running = false;
        GameboyThread->join();
        delete GameboyThread;
        delete PpuUnit;
        delete MemoryUnit;
        delete CpuUnit;
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
                if (CpuUnit->CpuHalted)
                {
                    CpuUnit->Halt();
                    cycles = 4;
                }
                else
                    cycles = CpuUnit->ExecuteNextOpCode();
                cyclesthisupdate += cycles;
                CpuUnit->UpdateTimers(cycles);
                PpuUnit->updateGraphics(cycles);
                if(!CpuUnit->CpuHalted)
                    CpuUnit->CheckInterupts();
            }
            PpuUnit->renderscreen();
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
