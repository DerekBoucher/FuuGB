#ifndef MEMORYTEST_H
#define MEMORYTEST_H

#include <cassert>
#include "Memory.h"

namespace MemoryTests {

    class Test : public FuuGB::Memory {

    public:
        Test();
        ~Test();
        void RunAllTests();

    private:
    };
}

#endif