#ifndef CPUTEST_H
#define CPUTEST_H

#include <cassert>
#include <stdio.h>
#include <ctime>
#include <fstream>
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
        void test_add16BitRegister();
        void test_add8BitRegister();
        void test_add8BitRegisterWithCarry();
        void test_sub8BitRegister();
        void test_sub8BitRegisterWithCarry();
        void test_and8BitRegister();
        void test_xor8BitRegister();
        void test_or8BitRegister();
        void test_cmp8BitRegister();
        void test_TestBitInByte();
        void test_TestBitInWord();
        void test_twoComp_Byte();
        void test_twoComp_Word();
        void test_rotateReg();
        void test_shiftReg();
        void test_swapReg();
        void test_adjustDAA();
        void test_executeNextOpCode();
        void resetRegisters();
    };
}

#endif