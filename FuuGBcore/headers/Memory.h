//
//  Memory.h
//  FuuGBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef Memory_h
#define Memory_h
#include "Core.h"
#include "Cartridge.h"
#include "Logger.h"

#define RAM_CLOCK_PERIOD_NS 952
#define ECHO_RAM_OFFSET 0x2000
#define SLEEP_CLOCK_CYCLE() std::this_thread::sleep_for(std::chrono::nanoseconds(RAM_CLOCK_PERIOD_NS))

namespace FuuGB
{
    class FUUGB_API Memory
    {
    public:
        Memory(Cartridge*);
        virtual ~Memory();
		void closeBootRom();
        void stop();
        void writeMemory(uWORD, uBYTE);
        uBYTE& readMemory(uWORD);

		uBYTE & DMA_read(uWORD addr);

		void DMA_write(uWORD addr, uBYTE data);

		void changeROMBank(uWORD addr, uBYTE data);

		void toggleRAM(uWORD addr, uBYTE data);

		void changeMode(uBYTE data);
        
        void RequestInterupt(int code);

		void changeRAMBank(uWORD addr, uBYTE data);
		std::mutex              ramKey;
		std::mutex              vramKey;
		std::condition_variable ramCond;
		std::condition_variable vramCond;
        int                     timer_counter;
        
    
    private:
        uBYTE*                  M_MEM;
        Cartridge*              cart;
        FILE*                   bootROM;
        std::thread*            _ramTHR;
        bool                    CycleDone;
		bool					bootRomClosed;
        void                    DMA_Transfer(uBYTE);
	
    
        void ramClock();
    };
}
#endif /* Memory_h */
