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

#define RAM_CLOCK_PERIOD_NS 952

namespace FuuGB
{
    class FUUGB_API Memory
    {
    public:
        Memory(Cartridge*);
        virtual ~Memory();
        void stop();
        void writeMemory(uWORD, uBYTE);
        uBYTE& readMemory(uWORD);
    
    private:
        uBYTE*                  M_MEM;
        Cartridge*              cart;
        FILE*                   bootROM;
        std::thread*            _ramTHR;
        std::mutex              ramKey;
        std::mutex              vramKey;
        std::condition_variable ramCond;
        std::condition_variable vramCond;
        bool                    CycleDone;
        bool                    _memoryRunning;
    
        void ramClock();
    };
}
#endif /* Memory_h */
