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
        test_add16BitRegister();
        test_add8BitRegister();
        test_add8BitRegisterWithCarry();
    }

    void Test::test_increment16BitRegister() {

        resetRegisters();

        uWORD expected[0x10000];

        for (auto i = 0x0; i <= MAX_WORD; i++) {
            expected[i] = i + 1;
        }
        
        for (auto i = 0x0; i <= MAX_WORD; i++) {
            increment16BitRegister(AF.data);
            increment16BitRegister(BC.data);
            increment16BitRegister(DE.data);
            increment16BitRegister(HL.data);
            increment16BitRegister(SP);
            increment16BitRegister(PC);
            assert(AF.data == expected[i]);
            assert(BC.data == expected[i]);
            assert(DE.data == expected[i]);
            assert(HL.data == expected[i]);
            assert(SP == expected[i]);
            assert(PC == expected[i]);
        }
    }

    void Test::test_increment8BitRegister() {

        resetRegisters();

        uBYTE expected[0x100];
        
        for (auto i = 0x0; i <= MAX_BYTE; i++) {
            expected[i] = i + 1;
        }
        
        for (auto i = 0x0; i <= MAX_BYTE; i++) {
            increment8BitRegister(AF.hi);
            increment8BitRegister(BC.hi);
            increment8BitRegister(BC.lo);
            increment8BitRegister(DE.hi);
            increment8BitRegister(DE.lo);
            increment8BitRegister(HL.hi);
            increment8BitRegister(HL.lo);
            assert(AF.hi == expected[i]);
            assert(BC.hi == expected[i]);
            assert(BC.lo == expected[i]);
            assert(DE.hi == expected[i]);
            assert(DE.lo == expected[i]);
            assert(HL.hi == expected[i]);
            assert(HL.lo == expected[i]);

            if (expected[i] == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (((((expected[i] - 0x01) & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);
        }
    }

    void Test::test_decrement16BitRegister() {

        resetRegisters();

        uWORD expected[0x10000];

        for (auto i = 0x0000 ; i <= MAX_WORD; i++) {
            expected[i] = MAX_WORD - i;
        }
        
        for (auto i = 0x0; i <= MAX_WORD; i++) {
            decrement16BitRegister(AF.data);
            decrement16BitRegister(BC.data);
            decrement16BitRegister(DE.data);
            decrement16BitRegister(HL.data);
            decrement16BitRegister(SP);
            decrement16BitRegister(PC);
            assert(AF.data == expected[i]);
            assert(BC.data == expected[i]);
            assert(DE.data == expected[i]);
            assert(HL.data == expected[i]);
            assert(SP == expected[i]);
            assert(PC == expected[i]);
        }
    }

    void Test::test_decrement8BitRegister() {

        resetRegisters();

        uBYTE expected[0x100];

        for (auto i = 0x00; i <= MAX_BYTE; i++) {
            expected[i] = MAX_BYTE - i;
        }
        
        for (auto i = 0x0; i <= MAX_BYTE; i++) {
            decrement8BitRegister(AF.hi);
            decrement8BitRegister(BC.hi);
            decrement8BitRegister(BC.lo);
            decrement8BitRegister(DE.hi);
            decrement8BitRegister(DE.lo);
            decrement8BitRegister(HL.hi);
            decrement8BitRegister(HL.lo);
            assert(AF.hi == expected[i]);
            assert(BC.hi == expected[i]);
            assert(BC.lo == expected[i]);
            assert(DE.hi == expected[i]);
            assert(DE.lo == expected[i]);
            assert(HL.hi == expected[i]);
            assert(HL.lo == expected[i]);

            if (expected[i] == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (((((expected[i] + 0x01) & 0x0F) - (0x01 & 0x0F))) < 0x0) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 1);
        }
    }

    void Test::test_add16BitRegister() {

        resetRegisters();

        uWORD operand1[0x1000];
        uWORD operand2[0x1000];
        uWORD expected[0x1000];

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1[i] = std::rand()%0xFFFF;
            operand2[i] = std::rand()%0xFFFF;
            expected[i] = operand1[i] + operand2[i];
            add16BitRegister(BC.data, operand1[i]);
            add16BitRegister(BC.data, operand2[i]);
            add16BitRegister(DE.data, operand1[i]);
            add16BitRegister(DE.data, operand2[i]);
            add16BitRegister(HL.data, operand1[i]);
            add16BitRegister(HL.data, operand2[i]);
            add16BitRegister(SP, operand1[i]);
            add16BitRegister(SP, operand2[i]);
            add16BitRegister(PC, operand1[i]);
            add16BitRegister(PC, operand2[i]);
            assert(BC.data == expected[i]);
            assert(DE.data == expected[i]);
            assert(HL.data == expected[i]);
            assert(SP == expected[i]);
            assert(PC == expected[i]);

            if (((operand1[i] & 0x0FFF) + (operand2[i] & 0x0FFF) > 0x0FFF)) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if ((operand1[i] + operand2[i]) > 0xFFFF) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_add8BitRegister() {

        resetRegisters();

        uBYTE operand1[0x1000];
        uBYTE operand2[0x1000];
        uBYTE expected[0x1000];

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1[i] = std::rand()%0xFF;
            operand2[i] = std::rand()%0xFF;
            expected[i] = operand1[i] + operand2[i];
            add8BitRegister(BC.hi, operand1[i]);
            add8BitRegister(BC.hi, operand2[i]);
            add8BitRegister(DE.hi, operand1[i]);
            add8BitRegister(DE.hi, operand2[i]);
            add8BitRegister(HL.hi, operand1[i]);
            add8BitRegister(HL.hi, operand2[i]);
            assert(BC.hi == expected[i]);
            assert(DE.hi == expected[i]);
            assert(HL.hi == expected[i]);

            if (((operand1[i] & 0x0F) + (operand2[i] & 0x0F)) > 0x0F) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if ((operand1[i] + operand2[i]) > 0xFF) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_add8BitRegisterWithCarry() {

        resetRegisters();

        uBYTE operand1[0x1000];
        uBYTE operand2[0x1000];
        uBYTE expected[0x1000];

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1[i] = std::rand()%0xFF;
            operand2[i] = std::rand()%0xFF;
            expected[i] = operand1[i] + operand2[i];
            uBYTE c1;
            uBYTE c2;

            if (CPU_FLAG_BIT_TEST(C_FLAG) == 1) {
                c1 = 0x1;
                expected[i] += c1;
            } else {
                c1 = 0x0;
            }

            add8BitRegister(BC.hi, operand1[i], CPU_FLAG_BIT_TEST(C_FLAG));

            if (CPU_FLAG_BIT_TEST(C_FLAG) == 1) {
                c2 = 0x1;
                expected[i] += c2;
            } else {
                c2 = 0x0;
            }

            add8BitRegister(BC.hi, operand2[i], CPU_FLAG_BIT_TEST(C_FLAG));
            assert(BC.hi == expected[i]);

            if ((((operand1[i] + c1) & 0x0F) + ((operand2[i] + c2) & 0x0F)) > 0x0F) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if ((operand1[i] + c1 + operand2[i] + c2) > 0xFF) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);

            resetRegisters();

            // Set Random Carry Flag
            AF.lo = std::rand()%0xFF;
        }
    }
}
