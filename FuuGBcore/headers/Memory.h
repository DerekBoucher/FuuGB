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

#define MEMORY_CLOCK_PERIOD_NS 952

namespace FuuGB
{
    class Memory
    {
    public:
        Memory(Cartridge*);
        virtual ~Memory();
        void stop();
        void writeMemory(uWORD, uBYTE);
        uBYTE& readMemory(uWORD);
        uBYTE& operator [](const uWORD address)
        {
            std::unique_lock<decltype(key)> lock(key);
            memCond.wait(lock);
            lock.unlock();
            return M_MEM[address];
        }
    
    private:
        uBYTE*                  M_MEM;
        Cartridge*              cart;
        FILE*                   bootROM;
        std::thread*            _memTHR;
        std::mutex              key;
        std::condition_variable memCond;
        bool                    CycleDone;
        bool                    _memoryRunning;
    
        void clock();
    };
}
#endif /* Memory_h */
