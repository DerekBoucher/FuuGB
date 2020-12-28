#include "Gameboy.h"

Gameboy::Gameboy(wxWindow* screen, Cartridge* cart)
{
    running = true;
    memoryUnit = new Memory(cart);
    ppuUnit = new PPU(screen, this->memoryUnit);
    cpuUnit = new CPU(this->memoryUnit);
    pause = false;
    thread = new std::thread(&Gameboy::Run, this);
}

Gameboy::~Gameboy()
{
    delete thread;
    delete ppuUnit;
    delete memoryUnit;
    delete cpuUnit;
}

void Gameboy::Run()
{
    const int MaxCycles = 69905;
    while (running)
    {
        int cyclesThisUpdate = 0;
        while (cyclesThisUpdate <= MaxCycles)
        {
            int cycles = 0;

            if (pause)
            {
                wait();
            }

            if (cpuUnit->Halted)
            {
                cycles = 4;
                memoryUnit->UpdateTimers(cycles);
                cpuUnit->Halt();
            }
            else
            {
                cycles = cpuUnit->ExecuteNextOpCode();
            }

            cyclesThisUpdate += cycles;
            ppuUnit->UpdateGraphics(cycles);
            memoryUnit->UpdateDmaCycles(cycles);

            if (!cpuUnit->Halted)
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
    cv.notify_all();
}

void Gameboy::Stop() {
    running = false;
    thread->join();
}

void Gameboy::wait()
{
    std::unique_lock<std::mutex> pauseLock(mtx);
    cv.wait(pauseLock);
    pauseLock.unlock();
    pause = false;
}
