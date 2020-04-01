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
        test_sub8BitRegister();
        test_sub8BitRegisterWithCarry();
        test_and8BitRegister();
        test_xor8BitRegister();
        test_or8BitRegister();
        test_cmp8BitRegister();
        test_TestBitInByte();
        test_TestBitInWord();
        test_twoComp_Word();
        test_twoComp_Byte();
    }

    void Test::test_increment16BitRegister() {

        resetRegisters();
        uWORD expected;
        
        for (auto i = 0x0; i <= MAX_WORD; i++) {
            expected = i + 1;
            increment16BitRegister(AF.data);
            increment16BitRegister(BC.data);
            increment16BitRegister(DE.data);
            increment16BitRegister(HL.data);
            increment16BitRegister(SP);
            increment16BitRegister(PC);
            assert(AF.data == expected);
            assert(BC.data == expected);
            assert(DE.data == expected);
            assert(HL.data == expected);
            assert(SP == expected);
            assert(PC == expected);
        }
    }

    void Test::test_increment8BitRegister() {

        resetRegisters();
        uBYTE expected;
        
        for (auto i = 0x0; i <= MAX_BYTE; i++) {
            expected = i + 1;
            increment8BitRegister(AF.hi);
            increment8BitRegister(BC.hi);
            increment8BitRegister(BC.lo);
            increment8BitRegister(DE.hi);
            increment8BitRegister(DE.lo);
            increment8BitRegister(HL.hi);
            increment8BitRegister(HL.lo);
            assert(AF.hi == expected);
            assert(BC.hi == expected);
            assert(BC.lo == expected);
            assert(DE.hi == expected);
            assert(DE.lo == expected);
            assert(HL.hi == expected);
            assert(HL.lo == expected);

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (((((expected - 0x01) & 0x0F) + (0x01 & 0x0F)) & 0x10) == 0x10) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);
        }
    }

    void Test::test_decrement16BitRegister() {

        resetRegisters();
        uWORD expected;
        
        for (auto i = 0x0; i <= MAX_WORD; i++) {
            expected = MAX_WORD - i;
            decrement16BitRegister(AF.data);
            decrement16BitRegister(BC.data);
            decrement16BitRegister(DE.data);
            decrement16BitRegister(HL.data);
            decrement16BitRegister(SP);
            decrement16BitRegister(PC);
            assert(AF.data == expected);
            assert(BC.data == expected);
            assert(DE.data == expected);
            assert(HL.data == expected);
            assert(SP == expected);
            assert(PC == expected);
        }
    }

    void Test::test_decrement8BitRegister() {

        resetRegisters();
        uBYTE expected;
        
        for (auto i = 0x0; i <= MAX_BYTE; i++) {
            expected = MAX_BYTE - i;
            decrement8BitRegister(AF.hi);
            decrement8BitRegister(BC.hi);
            decrement8BitRegister(BC.lo);
            decrement8BitRegister(DE.hi);
            decrement8BitRegister(DE.lo);
            decrement8BitRegister(HL.hi);
            decrement8BitRegister(HL.lo);
            assert(AF.hi == expected);
            assert(BC.hi == expected);
            assert(BC.lo == expected);
            assert(DE.hi == expected);
            assert(DE.lo == expected);
            assert(HL.hi == expected);
            assert(HL.lo == expected);

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (((((expected + 0x01) & 0x0F) - (0x01 & 0x0F))) < 0x0) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 1);
        }
    }

    void Test::test_add16BitRegister() {

        resetRegisters();
        uWORD operand1;
        uWORD operand2;
        uWORD expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFFFF;
            operand2 = std::rand()%0xFFFF;
            expected = operand1 + operand2;
            add16BitRegister(BC.data, operand1);
            add16BitRegister(BC.data, operand2);
            add16BitRegister(DE.data, operand1);
            add16BitRegister(DE.data, operand2);
            add16BitRegister(HL.data, operand1);
            add16BitRegister(HL.data, operand2);
            add16BitRegister(SP, operand1);
            add16BitRegister(SP, operand2);
            add16BitRegister(PC, operand1);
            add16BitRegister(PC, operand2);
            assert(BC.data == expected);
            assert(DE.data == expected);
            assert(HL.data == expected);
            assert(SP == expected);
            assert(PC == expected);

            if (((operand1 & 0x0FFF) + (operand2 & 0x0FFF) > 0x0FFF)) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if ((operand1 + operand2) > 0xFFFF) {
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
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;
            expected = operand1 + operand2;
            add8BitRegister(BC.hi, operand1);
            add8BitRegister(BC.hi, operand2);
            add8BitRegister(DE.hi, operand1);
            add8BitRegister(DE.hi, operand2);
            add8BitRegister(HL.hi, operand1);
            add8BitRegister(HL.hi, operand2);
            assert(BC.hi == expected);
            assert(DE.hi == expected);
            assert(HL.hi == expected);

            if (((operand1 & 0x0F) + (operand2 & 0x0F)) > 0x0F) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if ((operand1 + operand2) > 0xFF) {
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
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;
            expected = operand1 + operand2;
            uBYTE c1;
            uBYTE c2;

            // Set Random Flags
            AF.lo = (std::rand()%0xFF) & 0xF0;

            if ((AF.lo & 0x10) > 0) {
                c1 = 0x1;
                expected += c1;
            } else {
                c1 = 0x0;
            }

            add8BitRegister(BC.hi, operand1, CPU_FLAG_BIT_TEST(C_FLAG));

            if ((AF.lo & 0x10) > 0) {
                c2 = 0x1;
                expected += c2;
            } else {
                c2 = 0x0;
            }

            add8BitRegister(BC.hi, operand2, CPU_FLAG_BIT_TEST(C_FLAG));
            assert(BC.hi == expected);

            if ((((operand1 + c1) & 0x0F) + ((operand2 + c2) & 0x0F)) > 0x0F) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if ((operand1 + c1 + operand2 + c2) > 0xFF) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_sub8BitRegister() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;
            expected = operand1 - operand2;

            BC.hi = operand1;

            sub8BitRegister(BC.hi, operand2);
            assert(BC.hi == expected);

            if (((operand1 & 0x0F) < (operand2 & 0x0F))) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (operand1 < operand2) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 1);

            resetRegisters();
        }
    }

    void Test::test_sub8BitRegisterWithCarry() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;
            uBYTE c;

            // Set Random Flags
            AF.lo = (std::rand()%0xFF) & 0xF0;

            if ((AF.lo & 0x10) > 0) {
                c = 0x1;
                expected += c;
            } else {
                c = 0x0;
            }

            expected = operand1 - operand2 - c;
            BC.hi = operand1;

            sub8BitRegister(BC.hi, operand2, CPU_FLAG_BIT_TEST(C_FLAG));
            assert(BC.hi == expected);

            if (((operand1 & 0x0F) < ((operand2 + c) & 0x0F))) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (operand1 < (operand2 + c)) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 1);

            resetRegisters();
        }
    }

    void Test::test_and8BitRegister() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;

            expected = operand1 & operand2;
            BC.hi = operand1;

            and8BitRegister(BC.hi, operand2);
            assert(BC.hi == expected);

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);
            assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_xor8BitRegister() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;

            expected = operand1 ^ operand2;
            BC.hi = operand1;

            xor8BitRegister(BC.hi, operand2);
            assert(BC.hi == expected);

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);
            assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_or8BitRegister() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;
        uBYTE expected;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;

            expected = operand1 | operand2;
            BC.hi = operand1;

            or8BitRegister(BC.hi, operand2);
            assert(BC.hi == expected);

            if (expected == 0x0) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 0);
            assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);

            resetRegisters();
        }
    }

    void Test::test_cmp8BitRegister() {

        resetRegisters();
        uBYTE operand1;
        uBYTE operand2;

        std::srand(std::time(NULL));

        for(auto i = 0x0; i < 0x1000; i++) {
            operand1 = std::rand()%0xFF;
            operand2 = std::rand()%0xFF;
            BC.hi = operand1;

            cmp8BitRegister(BC.hi, operand2);

            if (((operand1 & 0x0F) < (operand2 & 0x0F))) {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(H_FLAG) == 0);
            }

            if (operand1 == operand2) {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(Z_FLAG) == 0);
            }

            if (operand1 < operand2) {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 1);
            } else {
                assert(CPU_FLAG_BIT_TEST(C_FLAG) == 0);
            }

            assert(CPU_FLAG_BIT_TEST(N_FLAG) == 1);

            resetRegisters();
        }
    }

    void Test::test_TestBitInByte() {

        resetRegisters();
        uBYTE operand = 0x7B;

        assert(TestBitInByte(operand, 0) == 1);
        assert(TestBitInByte(operand, 1) == 1);
        assert(TestBitInByte(operand, 2) == 0);
        assert(TestBitInByte(operand, 3) == 1);
        assert(TestBitInByte(operand, 4) == 1);
        assert(TestBitInByte(operand, 5) == 1);
        assert(TestBitInByte(operand, 6) == 1);
        assert(TestBitInByte(operand, 7) == 0);
    }

    void Test::test_TestBitInWord() {

        resetRegisters();
        uWORD operand = 0x7B7B;

        assert(TestBitInWord(operand, 0) == 1);
        assert(TestBitInWord(operand, 1) == 1);
        assert(TestBitInWord(operand, 2) == 0);
        assert(TestBitInWord(operand, 3) == 1);
        assert(TestBitInWord(operand, 4) == 1);
        assert(TestBitInWord(operand, 5) == 1);
        assert(TestBitInWord(operand, 6) == 1);
        assert(TestBitInWord(operand, 7) == 0);
        assert(TestBitInWord(operand, 8) == 1);
        assert(TestBitInWord(operand, 9) == 1);
        assert(TestBitInWord(operand, 10) == 0);
        assert(TestBitInWord(operand, 11) == 1);
        assert(TestBitInWord(operand, 12) == 1);
        assert(TestBitInWord(operand, 13) == 1);
        assert(TestBitInWord(operand, 14) == 1);
        assert(TestBitInWord(operand, 15) == 0);
    }

    void Test::test_twoComp_Byte() {

        resetRegisters();
        std::srand(std::time(NULL));
        uBYTE operand = std::rand()%0xFF;
        uBYTE expected = (~operand) + 0x01;

        assert(twoComp_Byte(operand) == expected);
    }

    void Test::test_twoComp_Word() {

        resetRegisters();
        std::srand(std::time(NULL));
        uWORD operand = std::rand()%0xFFFF;
        uWORD expected = (~operand) + 0x01;

        assert(twoComp_Word(operand) == expected);
    }

    void Test::test_rotateReg() {

        resetRegisters();
    }
}
