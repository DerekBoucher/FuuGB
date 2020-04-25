#ifndef PPUTEST_H
#define PPUTEST_H

#include <cassert>
#include "PPU.h"

namespace PpuTests {

    class Test : public FuuGB::PPU {

    public:
        Test();
        ~Test();
        void RunAllTests();

    private:
    };
}

#endif