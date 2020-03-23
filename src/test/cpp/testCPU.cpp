#include "testCPU.h"

namespace CpuTests {

    const uWORD MAX_WORD = 0xFFFF;
    const uBYTE MAX_BYTE = 0xFF;

    Test::Test():FuuGB::CPU(NULL) {}

    Test::~Test() {}

    void Test::resetRegisters() {
        AF.data = 0x0000;
        BC.data = 0x0000;
        DE.data = 0x0000;
        HL.data = 0x0000;
        PC = 0x0000;
        SP = 0x0000;
    }

    void Test::RunAllTests() {
        test_increment16BitRegister();
        test_increment8BitRegister();
        test_decrement16BitRegister();
        test_decrement8BitRegister();
    }

    void Test::test_increment16BitRegister() {

        resetRegisters();

        uWORD expected[0x10000];

        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            expected[i] = i + 1;
        }
        
        // AF Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(AF.data);
            assert(AF.data == expected[i]);
        }
        // BC Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(BC.data);
            assert(BC.data == expected[i]);
        }
        // DE Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(DE.data);
            assert(DE.data == expected[i]);
        }
        // HL Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(HL.data);
            assert(HL.data == expected[i]);
        }
        // SP Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(SP);
            assert(SP == expected[i]);
        }
        // PC Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            increment16BitRegister(PC);
            assert(PC == expected[i]);
        }
    }

    void Test::test_increment8BitRegister() {

        resetRegisters();

        uBYTE expected[0x100];
        
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            expected[i] = i + 1;
        }
        
        // A Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(AF.hi);
            assert(AF.hi == expected[i]);
        }
        // F Register : Need to come up with specific expected for F
        //for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
        //    increment8BitRegister(AF.lo);
        //    assert(AF.lo == expected[i]);
        //}
        // B Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(BC.hi);
            assert(BC.hi == expected[i]);
        }
        // C Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(BC.lo);
            assert(BC.lo == expected[i]);
        }
        // D Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(DE.hi);
            assert(DE.hi == expected[i]);
        }
        // E Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(DE.lo);
            assert(DE.lo == expected[i]);
        }
        // H Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(HL.hi);
            assert(HL.hi == expected[i]);
        }
        // L Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            increment8BitRegister(HL.lo);
            assert(HL.lo == expected[i]);
        }
    }

    void Test::test_decrement16BitRegister() {

        resetRegisters();

        uWORD expected[0x10000];

        for (uWORD i = 0x0000 ; i < MAX_WORD; i++) {
            expected[i] = MAX_WORD - i;
        }
        
        // AF Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(AF.data);
            assert(AF.data == expected[i]);
        }
        // BC Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(BC.data);
            assert(BC.data == expected[i]);
        }
        // DE Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(DE.data);
            assert(DE.data == expected[i]);
        }
        // HL Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(HL.data);
            assert(HL.data == expected[i]);
        }
        // SP Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(SP);
            assert(SP == expected[i]);
        }
        // PC Register
        for (uWORD i = 0x0; i < MAX_WORD; i++) {
            decrement16BitRegister(PC);
            assert(PC == expected[i]);
        }
    }

    void Test::test_decrement8BitRegister() {

        resetRegisters();

        uBYTE expected[0x100];

        for (uBYTE i = 0x00; i < MAX_BYTE; i++) {
            expected[i] = MAX_BYTE - i;
        }
        
        // A Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(AF.hi);
            assert(AF.hi == expected[i]);
        }
        // F Register: need special test case
        //for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
        //    decrement8BitRegister(AF.lo);
        //    assert(AF.lo == expected[i]);
        //}
        // B Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(BC.hi);
            assert(BC.hi == expected[i]);
        }
        // C Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(BC.lo);
            assert(BC.lo == expected[i]);
        }
        // D Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(DE.hi);
            assert(DE.hi == expected[i]);
        }
        // E Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(DE.lo);
            assert(DE.lo == expected[i]);
        }
        // H Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(HL.hi);
            assert(HL.hi == expected[i]);
        }
        // L Register
        for (uBYTE i = 0x0; i < MAX_BYTE; i++) {
            decrement8BitRegister(HL.lo);
            assert(HL.lo == expected[i]);
        }
    }
}
