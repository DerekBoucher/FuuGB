#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "Defines.h"
#include "CPU.h"
#include "PPU.h"

#include <thread>
#include <condition_variable>

class Gameboy
{
public:
    Gameboy(wxWindow*, Cartridge*);
    Gameboy(Gameboy&) = delete;
    virtual ~Gameboy();

    void Pause();
    void Resume();
    void Run();
    void Stop();

private:
    CPU* cpuUnit;
    Memory* memoryUnit;
    PPU* ppuUnit;
    std::thread* thread;
    std::mutex mtx;
    std::condition_variable cv;
    bool pause;
    bool running;

    void wait();
};

#endif