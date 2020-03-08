#include "testCPU.h"
#include "CPU.h"

namespace CpuTests {

    Test::Test():FuuGB::CPU(NULL) {}

    Test::~Test() {}

    void Test::RunAllTests() {
        test_increment16BitRegister();
    }

    void Test::test_increment16BitRegister() {
        uWORD expected[0x10000];
        for (auto i = 0x0; i < 0x10000; i++) {
            expected[i] = i + 1;
        }
        
        // AF Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(AF.data);
            assert(AF.data == expected[i]);
        }
        // BC Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(BC.data);
            assert(BC.data == expected[i]);
        }
        // DE Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(DE.data);
            assert(DE.data == expected[i]);
        }
        // HL Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(HL.data);
            assert(HL.data == expected[i]);
        }
        // SP Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(SP);
            assert(SP == expected[i]);
        }
        // PC Register
        for (auto i = 0x0; i < 0x10000; i++) {
            increment16BitRegister(PC);
            assert(PC == expected[i]);
        }
    }
}
