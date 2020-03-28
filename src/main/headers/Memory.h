#ifndef Memory_h
#define Memory_h
#include "Core.h"
#include "Cartridge.h"

#define RAM_CLOCK_PERIOD_NS 952
#define ECHO_RAM_OFFSET 0x2000
#define SLEEP_CLOCK_CYCLE() std::this_thread::sleep_for(std::chrono::nanoseconds(RAM_CLOCK_PERIOD_NS))

namespace FuuGB
{
    class Memory
    {
    public:
        Memory(Cartridge*);
        virtual ~Memory();

        void closeBootRom();
        void writeMemory(uWORD, uBYTE);
        void DMA_write(uWORD addr, uBYTE data);
        void changeROMBank(uWORD addr, uBYTE data);
        void toggleRAM(uWORD addr, uBYTE data);
        void changeMode(uBYTE data);
        void RequestInterupt(int code);
        void changeRAMBank(uWORD addr, uBYTE data);
        uBYTE& readMemory(uWORD);
        uBYTE& DMA_read(uWORD addr);

    public:
        int                     timer_counter;
    private:
        uBYTE*                  M_MEM;
        Cartridge*              cart;
        FILE*                   bootROM;
        std::thread*            _timerTHR;
        bool                    CycleDone;
        bool                    bootRomClosed;
        void                    DMA_Transfer(uBYTE);
        uBYTE                   dummy = 0x00;
    };
}
#endif /* Memory_h */
