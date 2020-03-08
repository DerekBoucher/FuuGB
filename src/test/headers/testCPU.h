#ifndef CPUTEST_H
#define CPUTEST_H

#include <cassert>
#include "CPU.h"
#include <stdio.h>

namespace CpuTests {

    class Test : public FuuGB::CPU {

        public:
        Test();
        ~Test();
        void RunAllTests();

        void test_increment16BitRegister();
        //void test_increment8BitRegister();
        //void test_decrement8BitRegister();
    };
}

#endif