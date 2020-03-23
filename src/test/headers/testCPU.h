#ifndef CPUTEST_H
#define CPUTEST_H

#include <cassert>
#include <stdio.h>
#include "CPU.h"

namespace CpuTests {

    class Test : public FuuGB::CPU {

    public:
        Test();
        ~Test();
        void RunAllTests();

    private:
        void test_increment16BitRegister();
        void test_increment8BitRegister();
        void test_decrement16BitRegister();
        void test_decrement8BitRegister();
        void resetRegisters();
    };
}

#endif