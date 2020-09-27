#include "Gameboy.h"
using namespace std::chrono;
namespace FuuGB
{
    std::condition_variable Shared::cv_GB;
    std::mutex Shared::mu_GB;

    Gameboy::Gameboy(SDL_Window *windowPtr, Cartridge *cart)
    {
        running = true;
        memoryUnit = new Memory(cart);
        ppuUnit = new PPU(windowPtr, this->memoryUnit);
        cpuUnit = new CPU(this->memoryUnit);
        pause = false;
        thread = new std::thread(&Gameboy::Run, this);
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
        const int MaxCycles = 69905;
        duration<double> screenRefreshCycle = std::chrono::nanoseconds(120 * MaxCycles);
        while (running)
        {
            high_resolution_clock::time_point start = high_resolution_clock::now();
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
            high_resolution_clock::time_point end = high_resolution_clock::now();
            duration<double> timeSpan = duration_cast<duration<double>>(end - start);
            if (timeSpan < screenRefreshCycle)
            {
                std::this_thread::sleep_for(screenRefreshCycle - timeSpan);
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
} // namespace FuuGB
