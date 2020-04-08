#include "Fuupch.h"
#include "CPU.h"

namespace FuuGB
{
    std::condition_variable Shared::cv_GB;
    std::mutex Shared::mu_GB;

    CPU::CPU(Memory* mem)
    {
        this->PC = 0x0000;
        this->SP = 0x0000;
        this->AF = 0x0000;
        this->BC = 0x0000;
        this->DE = 0x0000;
        this->HL = 0x0000;

        MemoryUnit = mem;

        TimerUpdateCounter = 0;
        DividerRegisterCounter = 0;
        
        CpuPaused = false;
        CpuHalted = false;
        IME = false;
    }

    CPU::~CPU()
    {

    }

    void CPU::Pause()
    {
        CpuPaused = true;
    }

    int CPU::ExecuteNextOpCode()
    {
        TimerUpdateCounter = 0;
        uBYTE byte = MemoryUnit->readMemory(PC++);
        uBYTE SP_data = 0x0;
        Register* temp = new Register();
        
        switch (byte)
        {
        case NOP:
            //4 CPU Cycle
            TimerUpdateCounter += 4;
            break;

        case LD_16IMM_BC:
            //12 CPU Cycles
            BC.lo = MemoryUnit->readMemory(PC++);
            BC.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 12;
            break;

        case LD_A_adrBC:
            //8 CPU Cycles
            MemoryUnit->writeMemory(BC.data, AF.hi);
            TimerUpdateCounter += 8;
            break;

        case INC_BC:
            //8 CPU Cycles
            increment16BitRegister(BC.data);
            TimerUpdateCounter += 8;
            break;

        case INC_B:
            //4 CPU Cycles
            increment8BitRegister(BC.hi);
            TimerUpdateCounter += 4;
            break;

        case DEC_B:
            //4 CPU Cycles
            decrement8BitRegister(BC.hi);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_B:
            //8 CPU Cycles
            BC.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case RLC_A:
            //4 CPU Cycles
            rotateReg(true, false, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case LD_SP_adr:
            //20 CPU cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            SP_data = MemoryUnit->readMemory(SP);
            MemoryUnit->writeMemory(temp->data, SP_data);
            TimerUpdateCounter += 20;
            break;

        case ADD_BC_HL:
            //8 CPU Cycles
            add16BitRegister(HL.data, BC.data);
            TimerUpdateCounter += 8;
            break;

        case LD_adrBC_A:
            //8 CPU Cycles
            AF.hi = MemoryUnit->readMemory(BC.data);
            TimerUpdateCounter += 8;
            break;

        case DEC_BC:
            //8 CPU Cycles
            decrement16BitRegister(BC.data);
            TimerUpdateCounter += 8;
            break;

        case INC_C:
            //4 CPU Cycles
            increment8BitRegister(BC.lo);
            TimerUpdateCounter += 4;
            break;

        case DEC_C:
            //4 CPU Cycles
            decrement8BitRegister(BC.lo);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_C:
            //8 CPU Cycles
            BC.lo = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case RRC_A:
            //4 CPU Cycles
            rotateReg(false, false, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case STOP:
            //4 Clock Cycles
            CpuPaused = true;
            break;

        case LD_16IMM_DE:
            //12 Clock Cycles
            DE.lo = MemoryUnit->readMemory(PC++);
            DE.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 12;
            break;

        case LD_A_adrDE:
            //8 Clock Cycles
            MemoryUnit->writeMemory(DE.data, AF.hi);
            TimerUpdateCounter += 8;
            break;

        case INC_DE:
            //8 Clock Cycles
            increment16BitRegister(DE.data);
            TimerUpdateCounter += 8;
            break;

        case INC_D:
            //4 Clock Cycles
            increment8BitRegister(DE.hi);
            TimerUpdateCounter += 4;
            break;

        case DEC_D:
            //4 Clock Cycles
            decrement8BitRegister(DE.hi);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_D:
            //8 Clock Cycles
            DE.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case RL_A:
            //4 Clock Cycles
            rotateReg(true, true, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case RJmp_IMM:
            //8 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (TestBitInByte(byte, 7))
                PC = PC - twoComp_Byte(byte);
            else
                PC = PC + byte;
            TimerUpdateCounter += 8;
            break;

        case ADD_DE_HL:
            //8 Clock Cycles
            add16BitRegister(HL.data, DE.data);
            TimerUpdateCounter += 8;
            break;

        case LD_adrDE_A:
            //8 Clock Cycles
            AF.hi = MemoryUnit->readMemory(DE.data);
            TimerUpdateCounter += 8;
            break;

        case DEC_DE:
            //4 Clock Cycles
            decrement16BitRegister(DE.data);
            TimerUpdateCounter += 4;
            break;

        case INC_E:
            //4 Clock Cycles
            increment8BitRegister(DE.lo);
            TimerUpdateCounter += 4;
            break;

        case DEC_E:
            //4 clock cycles
            decrement8BitRegister(DE.lo);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_E:
            //8 Clock Cycles
            DE.lo = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case RR_A:
            //4 clock cycles
            rotateReg(false, true, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case RJmp_NOTZERO:
            //8 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                if (TestBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            TimerUpdateCounter += 8;
            break;

        case LD_16IMM_HL:
            //12 Clock Cycles
            HL.lo = MemoryUnit->readMemory(PC++);
            HL.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 12;
            break;

        case LDI_A_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data++, AF.hi);
            TimerUpdateCounter += 8;
            break;

        case INC_HL:
            //4 Clock Cycles
            increment16BitRegister(HL.data);
            TimerUpdateCounter += 4;
            break;

        case INC_H:
            //4 Clock Cycles
            increment8BitRegister(HL.hi);
            TimerUpdateCounter += 4;
            break;

        case DEC_H:
            //4 Clock Cycles
            decrement8BitRegister(DE.hi);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_H:
            //8 Clock Cycles
            HL.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case DAA:
            //4 Clock Cycles
            adjustDAA(AF.hi);
            TimerUpdateCounter += 4;
            break;

        case RJmp_ZERO:
            //8 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                if (TestBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            TimerUpdateCounter += 8;
            break;

        case ADD_HL_HL:
            //8 Clock Cycles
            add16BitRegister(HL.data, HL.data);
            TimerUpdateCounter += 8;
            break;

        case LDI_adrHL_A:
            //8 Clock Cycles
            AF.hi = MemoryUnit->readMemory(HL.data++);
            TimerUpdateCounter += 8;
            break;

        case DEC_HL:
            //4 Clock Cycles
            decrement16BitRegister(HL.data);
            TimerUpdateCounter += 4;
            break;

        case INC_L:
            //4 Clock Cycles
            increment8BitRegister(HL.lo);
            TimerUpdateCounter += 4;
            break;

        case DEC_L:
            //4 Clock Cycles
            decrement8BitRegister(HL.lo);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_L:
            //8 Clock Cycles
            HL.lo = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case NOT_A:
            //4 Clock Cycles
            AF.hi = ~AF.hi;
            CPU_FLAG_BIT_SET(N_FLAG);
            CPU_FLAG_BIT_SET(H_FLAG);
            TimerUpdateCounter += 4;
            break;

        case RJmp_NOCARRY:
            //8 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {
                if (TestBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            TimerUpdateCounter += 8;
            break;

        case LD_16IM_SP:
            //12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            SP = temp->data;
            TimerUpdateCounter += 12;
            break;

        case LDD_A_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data--, AF.hi);
            TimerUpdateCounter += 8;
            break;

        case INC_SP:
            //8 Clock Cycles
            increment16BitRegister(SP);
            TimerUpdateCounter += 8;
            break;

        case INC_valHL:
            //12 Clock Cycles
            increment8BitRegister(MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 12;
            break;
            
        case DEC_valHL:
            //12 Clock Cycles
            decrement8BitRegister(MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 12;
            break;

        case LD_8IMM_adrHL:
            //12 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            MemoryUnit->writeMemory(HL.data, byte);
            TimerUpdateCounter += 12;
            break;

        case SET_CARRY_FLAG:
            //4 Clock Cycles
            CPU_FLAG_BIT_RESET(N_FLAG);
            CPU_FLAG_BIT_RESET(H_FLAG);
            CPU_FLAG_BIT_SET(C_FLAG);
            TimerUpdateCounter += 4;
            break;

        case RJmp_CARRY:
            //8 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                if (TestBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            TimerUpdateCounter += 8;
            break;

        case ADD_SP_HL:
            //8 Clock Cycles
            add16BitRegister(HL.data, SP);
            TimerUpdateCounter += 8;
            break;

        case LDD_adrHL_A:
            //8 Clock Cycles
            AF.hi = MemoryUnit->readMemory(HL.data--);
            TimerUpdateCounter += 8;
            break;

        case DEC_SP:
            //8 Clock Cycles;
            decrement16BitRegister(SP);
            TimerUpdateCounter += 8;
            break;

        case INC_A:
            //4 Clock Cycles
            increment8BitRegister(AF.hi);
            TimerUpdateCounter += 4;
            break;

        case DEC_A:
            //4 Clock Cycles
            decrement8BitRegister(AF.hi);
            TimerUpdateCounter += 4;
            break;

        case LD_8IMM_A:
            //8 Clock Cycles
            AF.hi = MemoryUnit->readMemory(PC++);
            TimerUpdateCounter += 8;
            break;

        case COMP_CARRY_FLAG:
            //4 Clock Cycles
            if (CPU_FLAG_BIT_TEST(C_FLAG))
                CPU_FLAG_BIT_RESET(C_FLAG);
            else
                CPU_FLAG_BIT_SET(C_FLAG);
            
            CPU_FLAG_BIT_RESET(N_FLAG);
            CPU_FLAG_BIT_RESET(H_FLAG);
            TimerUpdateCounter += 4;
            break;

        case LD_B_B:
            //4 Clock Cycles
            BC.hi = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_B:
            //4 Clock Cycles
            BC.hi = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_B:
            //4 Clock Cycles
            BC.hi = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_B:
            //4 Clock Cycles
            BC.hi = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_B:
            //4 Clock Cycles
            BC.hi = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_B:
            //4 Clock Cycles
            BC.hi = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_B:
            //8 Clock Cycles
            BC.hi = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_B:
            //4 Clock Cycles
            BC.hi = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_C:
            //4 Clock Cycles
            BC.lo = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_C:
            //4 Clock Cycles
            BC.lo = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_C:
            //4 Clock Cycles
            BC.lo = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_C:
            //4 Clock Cycles
            BC.lo = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_C:
            //4 Clock Cycles
            BC.lo = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_C:
            //4 Clock Cycles
            BC.lo = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_C:
            //8 Clock Cycles
            BC.lo = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_C:
            //4 Clock Cycles
            BC.lo = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_D:
            //4 Clock Cycles
            DE.hi = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_D:
            //4 Clock Cycles
            DE.hi = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_D:
            //4 Clock Cycles
            DE.hi = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_D:
            //4 Clock Cycles
            DE.hi = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_D:
            //4 Clock Cycles
            DE.hi = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_D:
            //4 Clock Cycles
            DE.hi = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_D:
            //8 Clock Cycles
            DE.hi = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_D:
            //4 Clock Cycles
            DE.hi = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_E:
            //4 Clock Cycles
            DE.lo = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_E:
            //4 Clock Cycles
            DE.lo = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_E:
            //4 Clock Cycles
            DE.lo = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_E:
            //4 Clock Cycles
            DE.lo = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_E:
            //4 Clock Cycles
            DE.lo = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_E:
            //4 Clock Cycles
            DE.lo = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_E:
            //8 Clock Cycles
            DE.lo = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_E:
            //4 Clock Cycles
            DE.lo = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_H:
            //4 Clock Cycles
            HL.hi = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_H:
            //4 Clock Cycles
            HL.hi = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_H:
            //4 Clock Cycles
            HL.hi = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_H:
            //4 Clock Cycles
            HL.hi = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_H:
            //4 Clock Cycles
            HL.hi = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_H:
            //4 Clock Cycles
            HL.hi = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_H:
            //8 Clock Cycles
            HL.hi = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_H:
            //4 Clock Cycles
            HL.hi = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_L:
            //4 Clock Cycles
            HL.lo = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_L:
            //4 Clock Cycles
            HL.lo = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_L:
            //4 Clock Cycles
            HL.lo = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_L:
            //4 Clock Cycles
            HL.lo = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_L:
            //4 Clock Cycles
            HL.lo = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_L:
            //4 Clock Cycles
            HL.lo = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_L:
            //8 Clock Cycles
            HL.lo = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_L:
            //4 Clock Cycles
            HL.lo = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_B_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, BC.hi);
            TimerUpdateCounter += 8;
            break;

        case LD_C_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, BC.lo);
            TimerUpdateCounter += 8;
            break;

        case LD_D_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, DE.hi);
            TimerUpdateCounter += 8;
            break;
            
        case LD_E_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, DE.lo);
            TimerUpdateCounter += 8;
            break;

        case LD_H_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, HL.hi);
            TimerUpdateCounter += 8;
            break;

        case LD_L_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, HL.lo);
            TimerUpdateCounter += 8;
            break;

        case HALT:
            //4 Clock Cycles
            CpuHalted = true;
            Halt();
            TimerUpdateCounter += 4;
            break;

        case LD_A_adrHL:
            //8 Clock Cycles
            MemoryUnit->writeMemory(HL.data, AF.hi);
            TimerUpdateCounter += 8;
            break;

        case LD_B_A:
            //4 Clock Cycles
            AF.hi = BC.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_C_A:
            //4 Clock Cycles
            AF.hi = BC.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_D_A:
            //4 Clock Cycles
            AF.hi = DE.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_E_A:
            //4 Clock Cycles
            AF.hi = DE.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_H_A:
            //4 Clock Cycles
            AF.hi = HL.hi;
            TimerUpdateCounter += 4;
            break;

        case LD_L_A:
            //4 Clock Cycles
            AF.hi = HL.lo;
            TimerUpdateCounter += 4;
            break;

        case LD_adrHL_A:
            //8 Clock Cycles
            AF.hi = MemoryUnit->readMemory(HL.data);
            TimerUpdateCounter += 8;
            break;

        case LD_A_A:
            //4 Clock Cycles
            AF.hi = AF.hi;
            TimerUpdateCounter += 4;
            break;

        case ADD_B_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case ADD_C_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break;

        case ADD_D_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case ADD_E_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case ADD_H_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case ADD_L_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case ADD_adrHL_A:
            //8 Clock Cycles
            add8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 8;
            break;

        case ADD_A_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case ADC_B_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_C_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_D_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_E_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_H_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_L_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case ADC_adrHL_A:
            //8 Clock Cycles
            add8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 8;
            break;

        case ADC_A_A:
            //4 Clock Cycles
            add8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SUB_B_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case SUB_C_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break; 

        case SUB_D_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case SUB_E_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case SUB_H_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case SUB_L_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case SUB_adrHL_A:
            //8 Clock Cycles
            sub8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 8;
            break;

        case SUB_A_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case SBC_B_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_C_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_D_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_E_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_H_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_L_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case SBC_adrHL_A:
            //8 Clock Cycles
            sub8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 8;
            break;

        case SBC_A_A:
            //4 Clock Cycles
            sub8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 4;
            break;

        case AND_B_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case AND_C_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break;

        case AND_D_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case AND_E_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case AND_H_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case AND_L_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case AND_adrHL_A:
            //8 Clock Cycles
            and8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 8;
            break;

        case AND_A_A:
            //4 Clock Cycles
            and8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case XOR_B_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case XOR_C_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break;

        case XOR_D_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case XOR_E_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case XOR_H_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case XOR_L_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case XOR_adrHL_A:
            //8 Clock Cycles
            xor8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 8;
            break;

        case XOR_A_A:
            //4 Clock Cycles
            xor8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case OR_B_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case OR_C_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break;

        case OR_D_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case OR_E_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case OR_H_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case OR_L_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case OR_adrHL_A:
            //8 Clock Cycles
            or8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 4;
            break;

        case OR_A_A:
            //4 Clock Cycles
            or8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case CMP_B_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, BC.hi);
            TimerUpdateCounter += 4;
            break;

        case CMP_C_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, BC.lo);
            TimerUpdateCounter += 4;
            break;

        case CMP_D_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, DE.hi);
            TimerUpdateCounter += 4;
            break;

        case CMP_E_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, DE.lo);
            TimerUpdateCounter += 4;
            break;

        case CMP_H_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, HL.hi);
            TimerUpdateCounter += 4;
            break;

        case CMP_L_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, HL.lo);
            TimerUpdateCounter += 4;
            break;

        case CMP_adrHL_A:
            //8 Clock Cycles
            cmp8BitRegister(AF.hi, MemoryUnit->readMemory(HL.data));
            TimerUpdateCounter += 8;
            break;

        case CMP_A_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, AF.hi);
            TimerUpdateCounter += 4;
            break;

        case RET_NOT_ZERO:
            //20/8 Clock Cycles
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                temp->lo = MemoryUnit->readMemory(SP++);
                temp->hi = MemoryUnit->readMemory(SP++);
                PC = temp->data;
                
                TimerUpdateCounter += 20;
            }
            else
                TimerUpdateCounter += 8;
            break;

        case POP_BC:
            //12 Clock Cycles
            BC.lo = MemoryUnit->readMemory(SP++);
            BC.hi = MemoryUnit->readMemory(SP++);
            TimerUpdateCounter += 12;
            break;

        case JMP_NOT_ZERO:
            //16/12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {    
                PC = temp->data;
                TimerUpdateCounter += 16;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case JMP:
            //16 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            
            PC = temp->data;
            TimerUpdateCounter += 16;
            break;

        case CALL_NOT_ZERO:
            //24/12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                Register temp2;
                temp2.data = PC;
                MemoryUnit->writeMemory(--SP, temp2.hi);
                MemoryUnit->writeMemory(--SP, temp2.lo);
                PC = temp->data;
                TimerUpdateCounter += 24;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case PUSH_BC:
            //16 clock cycles
            MemoryUnit->writeMemory(--SP, BC.hi);
            MemoryUnit->writeMemory(--SP, BC.lo);
            TimerUpdateCounter += 16;
            break;

        case ADD_IMM_A:
            //8 Clock Cycles
            add8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;

        case RST_0:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0000;
            TimerUpdateCounter += 16;
            break;

        case RET_ZERO:
            //8 Clock Cycles if cc false else 20 clock cycles
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                temp->lo = MemoryUnit->readMemory(SP++);
                temp->hi = MemoryUnit->readMemory(SP++);
                PC = temp->data;
                TimerUpdateCounter += 20;
            }
            else
                TimerUpdateCounter += 8;
            break;

        case RETURN:
            //16 Clock Cycles
            temp->lo = MemoryUnit->readMemory(SP++);
            temp->hi = MemoryUnit->readMemory(SP++);
            PC = temp->data;
            TimerUpdateCounter += 16;
            break;

        case JMP_ZERO:
            //16/12 Clock cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                PC = temp->data;
                TimerUpdateCounter += 16;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case EXT_OP:
            //4 Clock Cycles, this opcode is special, it allows for 16 bit opcodes
            TimerUpdateCounter += 4;
            byte = MemoryUnit->readMemory(PC++);
            switch (byte)
            {
            case RLC_B:
                //4 clock Cycles
                rotateReg(true, false, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RLC_C:
                //4 clock Cycles
                rotateReg(true, false, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RLC_D:
                //4 clock Cycles
                rotateReg(true, false, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RLC_E:
                //4 clock Cycles
                rotateReg(true, false, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RLC_H:
                //4 clock Cycles
                rotateReg(true, false, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RLC_L:
                //4 clock Cycles
                rotateReg(true, false, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RLC_adrHL:
                //8 clock Cycles
                rotateReg(true, false, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case eRLC_A:
                //4 clock Cycles
                rotateReg(true, false, AF.hi);
                TimerUpdateCounter += 4;
                break;
                
            case RRC_B:
                //4 clock Cycles
                rotateReg(false, false, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RRC_C:
                //4 clock Cycles
                rotateReg(false, false, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RRC_D:
                //4 clock Cycles
                rotateReg(false, false, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RRC_E:
                //4 clock Cycles
                rotateReg(false, false, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RRC_H:
                //4 clock Cycles
                rotateReg(false, false, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RRC_L:
                //4 clock Cycles
                rotateReg(false, false, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RRC_adrHL:
                //8 clock Cycles
                rotateReg(false, false, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case eRRC_A:
                //4 clock Cycles
                rotateReg(false, false, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RL_B:
                //4 clock Cycles
                rotateReg(true, true, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RL_C:
                //4 clock Cycles
                rotateReg(true, true, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RL_D:
                //4 clock Cycles
                rotateReg(true, true, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RL_E:
                //4 clock Cycles
                rotateReg(true, true, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RL_H:
                //4 clock Cycles
                rotateReg(true, true, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RL_L:
                //4 clock Cycles
                rotateReg(true, true, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RL_adrHL:
                //8 clock Cycles
                rotateReg(true, true, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case eRL_A:
                //4 clock Cycles
                rotateReg(true, true, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RR_B:
                //4 clock Cycles
                rotateReg(false, true, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RR_C:
                //4 clock Cycles
                rotateReg(false, true, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RR_D:
                //4 clock Cycles
                rotateReg(false, true, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RR_E:
                //4 clock Cycles
                rotateReg(false, true, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RR_H:
                //4 clock Cycles
                rotateReg(false, true, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RR_L:
                //4 clock Cycles
                rotateReg(false, true, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RR_adrHL:
                //8 clock Cycles
                rotateReg(false, true, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case eRR_A:
                //4 clock Cycles
                rotateReg(false, true, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SLA_B:
                //4 clock cycles
                shiftReg(true, true, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SLA_C:
                //4 clock cycles
                shiftReg(true, true, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SLA_D:
                //4 clock cycles
                shiftReg(true, true, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SLA_E:
                //4 clock cycles
                shiftReg(true, true, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SLA_H:
                //4 clock cycles
                shiftReg(true, true, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SLA_L:
                //4 clock cycles
                shiftReg(true, true, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SLA_adrHL:
                //8 Clock Cycles
                shiftReg(true, true, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SLA_A:
                //8 Clock Cycles
                shiftReg(true, true, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SRA_B:
                //4 clock Cycles
                shiftReg(false, true, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SRA_C:
                //4 clock Cycles
                shiftReg(false, true, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SRA_D:
                //4 clock Cycles
                shiftReg(false, true, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SRA_E:
                //4 clock Cycles
                shiftReg(false, true, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SRA_H:
                //4 clock Cycles
                shiftReg(false, true, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SRA_L:
                //4 clock Cycles
                shiftReg(false, true, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SRA_adrHL:
                //8 clock Cycles
                shiftReg(false, true, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SRA_A:
                //4 clock Cycles
                shiftReg(false, true, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SWAP_B:
                //4 Clock Cycles
                swapReg(BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SWAP_C:
                //4 Clock Cycles
                swapReg(BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SWAP_D:
                //4 Clock Cycles
                swapReg(DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SWAP_E:
                //4 Clock Cycles
                swapReg(DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SWAP_H:
                //4 Clock Cycles
                swapReg(HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SWAP_L:
                //4 Clock Cycles
                swapReg(HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SWAP_adrHL:
                //8 Clock Cycles
                swapReg(MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SWAP_A:
                //4 Clock Cycles
                swapReg(AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SRL_B:
                //4 Clock Cycles
                shiftReg(false, false, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SRL_C:
                //4 Clock Cycles
                shiftReg(false, false, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SRL_D:
                //4 Clock Cycles
                shiftReg(false, false, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SRL_E:
                //4 Clock Cycles
                shiftReg(false, false, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SRL_H:
                //4 Clock Cycles
                shiftReg(false, false, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SRL_L:
                //4 Clock Cycles
                shiftReg(false, false, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SRL_adrHL:
                //8 Clock Cycles
                shiftReg(false, false, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SRL_A:
                //4 Clock Cycles
                shiftReg(false, false, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_B:
                //4 Clock Cycles
                test_bit(0, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_C:
                //4 Clock Cycles
                test_bit(0, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_D:
                //4 Clock Cycles
                test_bit(0, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_E:
                //4 Clock Cycles
                test_bit(0, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_H:
                //4 Clock Cycles
                test_bit(0, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_L:
                //4 Clock Cycles
                test_bit(0, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_1_adrHL:
                //8 Clock Cycles
                test_bit(0, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_1_A:
                //4 Clock Cycles
                test_bit(0, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_B:
                //4 Clock Cycles
                test_bit(1, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_C:
                //4 Clock Cycles
                test_bit(1, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_D:
                //4 Clock Cycles
                test_bit(1, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_E:
                //4 Clock Cycles
                test_bit(1, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_H:
                //4 Clock Cycles
                test_bit(1, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_L:
                //4 Clock Cycles
                test_bit(1, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_2_adrHL:
                //8 Clock Cycles
                test_bit(1, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_2_A:
                //4 Clock Cycles
                test_bit(1, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_B:
                //4 Clock Cycles
                test_bit(2, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_C:
                //4 Clock Cycles
                test_bit(2, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_D:
                //4 Clock Cycles
                test_bit(2, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_E:
                //4 Clock Cycles
                test_bit(2, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_H:
                //4 Clock Cycles
                test_bit(2, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_L:
                //4 Clock Cycles
                test_bit(2, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_3_adrHL:
                //8 Clock Cycles
                test_bit(2, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_3_A:
                //4 Clock Cycles
                test_bit(2, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_B:
                //4 Clock Cycles
                test_bit(3, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_C:
                //4 Clock Cycles
                test_bit(3, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_D:
                //4 Clock Cycles
                test_bit(3, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_E:
                //4 Clock Cycles
                test_bit(3, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_H:
                //4 Clock Cycles
                test_bit(3, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_L:
                //4 Clock Cycles
                test_bit(3, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_4_adrHL:
                //8 Clock Cycles
                test_bit(3, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_4_A:
                //4 Clock Cycles
                test_bit(3, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_B:
                //4 Clock Cycles
                test_bit(4, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_C:
                //4 Clock Cycles
                test_bit(4, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_D:
                //4 Clock Cycles
                test_bit(4, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_E:
                //4 Clock Cycles
                test_bit(4, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_H:
                //4 Clock Cycles
                test_bit(4, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_5_L:
                //4 Clock Cycles
                test_bit(4, HL.lo);
                TimerUpdateCounter += 4;
                break;
            
            case BIT_5_adrHL:
                //8 Clock Cycles
                test_bit(4, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_5_A:
                //4 Clock Cycles
                test_bit(4, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_B:
                //4 Clock Cycles
                test_bit(5, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_C:
                //4 Clock Cycles
                test_bit(5, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_D:
                //4 Clock Cycles
                test_bit(5, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_E:
                //4 Clock Cycles
                test_bit(5, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_H:
                //4 Clock Cycles
                test_bit(5, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_L:
                //4 Clock Cycles
                test_bit(5, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_6_adrHL:
                //8 Clock Cycles
                test_bit(5, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;
                
            case BIT_6_A:
                //4 Clock Cycles
                test_bit(5, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_B:
                //4 Clock Cycles
                test_bit(6, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_C:
                //4 Clock Cycles
                test_bit(6, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_D:
                //4 Clock Cycles
                test_bit(6, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_E:
                //4 Clock Cycles
                test_bit(6, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_H:
                //4 Clock Cycles
                test_bit(6, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_L:
                //4 Clock Cycles
                test_bit(6, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_7_adrHL:
                //8 Clock Cycles
                test_bit(6, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_7_A:
                //4 Clock Cycles
                test_bit(6, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_B:
                //4 Clock Cycles
                test_bit(7, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_C:
                //4 Clock Cycles
                test_bit(7, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_D:
                //4 Clock Cycles
                test_bit(7, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_E:
                //4 Clock Cycles
                test_bit(7, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_H:
                //4 Clock Cycles
                test_bit(7, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_L:
                //4 Clock Cycles
                test_bit(7, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case BIT_8_adrHL:
                //8 Clock Cycles
                test_bit(7, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case BIT_8_A:
                //4 Clock Cycles
                test_bit(7, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_1_B:
                //4 Clock Cycles
                reset_bit(0, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_1_C:
                //4 Clock Cycles
                reset_bit(0, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_1_D:
                //4 Clock Cycles
                reset_bit(0, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_1_E:
                //4 Clock Cycles
                reset_bit(0, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_1_H:
                //4 Clock Cycles
                reset_bit(0, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_1_L:
                //4 Clock Cycles
                reset_bit(0, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_1_adrHL:
                //8 Clock Cycles
                reset_bit(0, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_1_A:
                //4 Clock Cycles
                reset_bit(0, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_2_B:
                //4 Clock Cycles
                reset_bit(1, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_2_C:
                //4 Clock Cycles
                reset_bit(1, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_2_D:
                //4 Clock Cycles
                reset_bit(1, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_2_E:
                //4 Clock Cycles
                reset_bit(1, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_2_H:
                //4 Clock Cycles
                reset_bit(1, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_2_L:
                //4 Clock Cycles
                reset_bit(1, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_2_adrHL:
                //8 Clock Cycles
                reset_bit(1, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_2_A:
                //4 Clock Cycles
                reset_bit(1, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_3_B:
                //4 Clock Cycles
                reset_bit(2, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_3_C:
                //4 Clock Cycles
                reset_bit(2, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_3_D:
                //4 Clock Cycles
                reset_bit(2, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_3_E:
                //4 Clock Cycles
                reset_bit(2, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_3_H:
                //4 Clock Cycles
                reset_bit(2, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_3_L:
                //4 Clock Cycles
                reset_bit(2, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_3_adrHL:
                //8 Clock Cycles
                reset_bit(2, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_3_A:
                //4 Clock Cycles
                reset_bit(2, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_4_B:
                //4 Clock Cycles
                reset_bit(3, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_4_C:
                //4 Clock Cycles
                reset_bit(3, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_4_D:
                //4 Clock Cycles
                reset_bit(3, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_4_E:
                //4 Clock Cycles
                reset_bit(3, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_4_H:
                //4 Clock Cycles
                reset_bit(3, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_4_L:
                //4 Clock Cycles
                reset_bit(3, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_4_adrHL:
                //8 Clock Cycles
                reset_bit(3, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_4_A:
                //4 Clock Cycles
                reset_bit(3, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_5_B:
                //4 Clock Cycles
                reset_bit(4, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_5_C:
                //4 Clock Cycles
                reset_bit(4, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_5_D:
                //4 Clock Cycles
                reset_bit(4, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_5_E:
                //4 Clock Cycles
                reset_bit(4, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_5_H:
                //4 Clock Cycles
                reset_bit(4, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_5_L:
                //4 Clock Cycles
                reset_bit(4, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_5_adrHL:
                //8 Clock Cycles
                reset_bit(4, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_5_A:
                //4 Clock Cycles
                reset_bit(4, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_6_B:
                //4 Clock Cycles
                reset_bit(5, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_6_C:
                //4 Clock Cycles
                reset_bit(5, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_6_D:
                //4 Clock Cycles
                reset_bit(5, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_6_E:
                //4 Clock Cycles
                reset_bit(5, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_6_H:
                //4 Clock Cycles
                reset_bit(5, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_6_L:
                //4 Clock Cycles
                reset_bit(5, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_6_adrHL:
                //8 Clock Cycles
                reset_bit(5, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_6_A:
                //4 Clock Cycles
                reset_bit(5, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_7_B:
                //4 Clock Cycles
                reset_bit(6, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_7_C:
                //4 Clock Cycles
                reset_bit(6, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_7_D:
                //4 Clock Cycles
                reset_bit(6, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_7_E:
                //4 Clock Cycles
                reset_bit(6, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_7_H:
                //4 Clock Cycles
                reset_bit(6, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_7_L:
                //4 Clock Cycles
                reset_bit(6, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_7_adrHL:
                //8 Clock Cycles
                reset_bit(6, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_7_A:
                //4 Clock Cycles
                reset_bit(6, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_8_B:
                //4 Clock Cycles
                reset_bit(7, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_8_C:
                //4 Clock Cycles
                reset_bit(7, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_8_D:
                //4 Clock Cycles
                reset_bit(7, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_8_E:
                //4 Clock Cycles
                reset_bit(7, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_8_H:
                //4 Clock Cycles
                reset_bit(7, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case RES_8_L:
                //4 Clock Cycles
                reset_bit(7, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case RES_8_adrHL:
                //8 Clock Cycles
                reset_bit(7, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case RES_8_A:
                //4 Clock Cycles
                reset_bit(7, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_1_B:
                //4 Clock Cycles
                set_bit(0, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_1_C:
                //4 Clock Cycles
                set_bit(0, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_1_D:
                //4 Clock Cycles
                set_bit(0, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_1_E:
                //4 Clock Cycles
                set_bit(0, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_1_H:
                //4 Clock Cycles
                set_bit(0, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_1_L:
                //4 Clock Cycles
                set_bit(0, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_1_adrHL:
                //8 Clock Cycles
                set_bit(0, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_1_A:
                //4 Clock Cycles
                set_bit(0, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_2_B:
                //4 Clock Cycles
                set_bit(1, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_2_C:
                //4 Clock Cycles
                set_bit(1, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_2_D:
                //4 Clock Cycles
                set_bit(1, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_2_E:
                //4 Clock Cycles
                set_bit(1, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_2_H:
                //4 Clock Cycles
                set_bit(1, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_2_L:
                //4 Clock Cycles
                set_bit(1, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_2_adrHL:
                //8 Clock Cycles
                set_bit(1, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_2_A:
                //4 Clock Cycles
                set_bit(1, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_3_B:
                //4 Clock Cycles
                set_bit(2, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_3_C:
                //4 Clock Cycles
                set_bit(2, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_3_D:
                //4 Clock Cycles
                set_bit(2, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_3_E:
                //4 Clock Cycles
                set_bit(2, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_3_H:
                //4 Clock Cycles
                set_bit(2, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_3_L:
                //4 Clock Cycles
                set_bit(2, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_3_adrHL:
                //8 Clock Cycles
                set_bit(2, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_3_A:
                //4 Clock Cycles
                set_bit(2, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_4_B:
                //4 Clock Cycles
                set_bit(3, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_4_C:
                //4 Clock Cycles
                set_bit(3, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_4_D:
                //4 Clock Cycles
                set_bit(3, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_4_E:
                //4 Clock Cycles
                set_bit(3, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_4_H:
                //4 Clock Cycles
                set_bit(3, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_4_L:
                //4 Clock Cycles
                set_bit(3, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_4_adrHL:
                //8 Clock Cycles
                set_bit(3, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_4_A:
                //4 Clock Cycles
                set_bit(3, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_5_B:
                //4 Clock Cycles
                set_bit(4, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_5_C:
                //4 Clock Cycles
                set_bit(4, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_5_D:
                //4 Clock Cycles
                set_bit(4, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_5_E:
                //4 Clock Cycles
                set_bit(4, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_5_H:
                //4 Clock Cycles
                set_bit(4, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_5_L:
                //4 Clock Cycles
                set_bit(4, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_5_adrHL:
                //8 Clock Cycles
                set_bit(4, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_5_A:
                //4 Clock Cycles
                set_bit(4, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_6_B:
                //4 Clock Cycles
                set_bit(5, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_6_C:
                //4 Clock Cycles
                set_bit(5, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_6_D:
                //4 Clock Cycles
                set_bit(5, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_6_E:
                //4 Clock Cycles
                set_bit(5, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_6_H:
                //4 Clock Cycles
                set_bit(5, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_6_L:
                //4 Clock Cycles
                set_bit(5, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_6_adrHL:
                //8 Clock Cycles
                set_bit(5, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_6_A:
                //4 Clock Cycles
                set_bit(5, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_7_B:
                //4 Clock Cycles
                set_bit(6, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_7_C:
                //4 Clock Cycles
                set_bit(6, BC.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_7_D:
                //4 Clock Cycles
                set_bit(6, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_7_E:
                //4 Clock Cycles
                set_bit(6, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_7_H:
                //4 Clock Cycles
                set_bit(6, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_7_L:
                //4 Clock Cycles
                set_bit(6, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_7_adrHL:
                //8 Clock Cycles
                set_bit(6, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_7_A:
                //4 Clock Cycles
                set_bit(6, AF.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_8_B:
                //4 Clock Cycles
                set_bit(7, BC.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_8_C:
                //4 Clock Cycles
                set_bit(7, BC.lo);
                TimerUpdateCounter += 4;
                break;
                
            case SET_8_D:
                //4 Clock Cycles
                set_bit(7, DE.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_8_E:
                //4 Clock Cycles
                set_bit(7, DE.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_8_H:
                //4 Clock Cycles
                set_bit(7, HL.hi);
                TimerUpdateCounter += 4;
                break;

            case SET_8_L:
                //4 Clock Cycles
                set_bit(7, HL.lo);
                TimerUpdateCounter += 4;
                break;

            case SET_8_adrHL:
                //8 Clock Cycles
                set_bit(7, MemoryUnit->readMemory(HL.data));
                TimerUpdateCounter += 8;
                break;

            case SET_8_A:
                //4 Clock Cycles
                set_bit(7, AF.hi);
                TimerUpdateCounter += 4;
                break;

            default:
                break;
            }
            break;

        case CALL_ZERO:
            //24/12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                Register temp2;
                temp2.data = PC;
                MemoryUnit->writeMemory(--SP, temp2.hi);
                MemoryUnit->writeMemory(--SP, temp2.lo);
                PC = temp->data;
                TimerUpdateCounter += 24;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case CALL:
            //24 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            {
                Register temp2;
                temp2.data = PC;
                MemoryUnit->writeMemory(--SP, temp2.hi);
                MemoryUnit->writeMemory(--SP, temp2.lo);
                PC = temp->data;
            }
            TimerUpdateCounter += 24;
            break;

        case ADC_8IMM_A:
            //8 Clock Cycles
            add8BitRegister(AF.hi, MemoryUnit->readMemory(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 8;
            break;

        case RST_8:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0008;
            TimerUpdateCounter += 16;
            break;

        case RET_NOCARRY:
            //20/8 Clock Cycles
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {
                temp->lo = MemoryUnit->readMemory(SP++);
                temp->hi = MemoryUnit->readMemory(SP++);
                PC = temp->data;
                TimerUpdateCounter += 20;
            }
            else
                TimerUpdateCounter += 8;
            break;

        case POP_DE:
            //12 Clock Cycles
            DE.lo = MemoryUnit->readMemory(SP++);
            DE.hi = MemoryUnit->readMemory(SP++);
            TimerUpdateCounter += 12;
            break;

        case JMP_NOCARRY:
            //16/12 Clock cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {    
                PC = temp->data;
                TimerUpdateCounter += 16;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case CALL_NOCARRY:
            //24/12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {    
                Register temp2;
                temp2.data = PC;
                MemoryUnit->writeMemory(--SP, temp2.hi);
                MemoryUnit->writeMemory(--SP, temp2.lo);
                PC = temp->data;
                TimerUpdateCounter += 24;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case PUSH_DE:
            //16 clock cycles
            MemoryUnit->writeMemory(--SP, DE.hi);
            MemoryUnit->writeMemory(--SP, DE.lo);
            TimerUpdateCounter += 16;
            break;

        case SUB_8IMM_A:
            //8 Clock Cycles
            sub8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;

        case RST_10:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0010;
            TimerUpdateCounter += 16;
            break;

        case RET_CARRY:
            //20/8 Clock Cycles
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                temp->lo = MemoryUnit->readMemory(SP++);
                temp->hi = MemoryUnit->readMemory(SP++);
                PC = temp->data;
                
                TimerUpdateCounter += 20;
            }else
                TimerUpdateCounter += 8;
            break;

        case RET_INT:
            //16 Clock Cycles
            temp->lo = MemoryUnit->readMemory(SP++);
            temp->hi = MemoryUnit->readMemory(SP++);
            PC = temp->data;
            IME = true;
            TimerUpdateCounter += 16;
            break;

        case JMP_CARRY:
            //16/12 Clock cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                PC = temp->data;
                TimerUpdateCounter += 16;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case CALL_CARRY:
            //24/12 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                Register temp2;
                temp2.data = PC;
                MemoryUnit->writeMemory(--SP, temp2.hi);
                MemoryUnit->writeMemory(--SP, temp2.lo);
                PC = temp->data;
                TimerUpdateCounter += 24;
            }
            else
                TimerUpdateCounter += 12;
            break;

        case SBC_8IMM_A:
            //8 Clock Cycles
            sub8BitRegister(AF.hi, MemoryUnit->readMemory(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
            TimerUpdateCounter += 8;
            break;

        case RST_18:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0018;
            TimerUpdateCounter += 16;
            break;

        case LDH_A_IMMadr:
            //12 Clock Cycles
            MemoryUnit->writeMemory((0xFF00 + MemoryUnit->readMemory(PC++)), AF.hi);
            TimerUpdateCounter += 12;
            break;

        case POP_HL:
            //12 Clock Cycles
            HL.lo = MemoryUnit->readMemory(SP++);
            HL.hi = MemoryUnit->readMemory(SP++);
            TimerUpdateCounter += 12;
            break;

        case LDH_A_C:
            //8 Clock Cycles
            MemoryUnit->writeMemory((0xFF00 + BC.lo), AF.hi);
            TimerUpdateCounter += 8;
            break;

        case PUSH_HL:
            //16 clock cycles
            MemoryUnit->writeMemory(--SP, HL.hi);
            MemoryUnit->writeMemory(--SP, HL.lo);
            TimerUpdateCounter += 16;
            break;

        case AND_8IMM_A:
            //8 Clock Cycles
            and8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;

        case RST_20:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0020;
            TimerUpdateCounter += 16;
            break;

        case ADD_SIMM_SP:
            //16 Clock Cycles
            byte = MemoryUnit->readMemory(PC++);
            if (TestBitInByte(byte, 7))
            {
                if (checkBorrowFromBit_Word(12, SP, twoComp_Byte(byte)))
                    CPU_FLAG_BIT_SET(H_FLAG);
                else
                    CPU_FLAG_BIT_RESET(H_FLAG);

                if (SP < byte)
                    CPU_FLAG_BIT_SET(C_FLAG);
                else
                    CPU_FLAG_BIT_RESET(C_FLAG);
                
                SP = SP - twoComp_Byte(byte);
            }
            else
            {
                if (!checkCarryFromBit_Word(12, SP, byte))
                    CPU_FLAG_BIT_SET(H_FLAG);
                else
                    CPU_FLAG_BIT_RESET(H_FLAG);
                
                if (SP > (0xFFFF - byte))
                    CPU_FLAG_BIT_SET(C_FLAG);
                else
                    CPU_FLAG_BIT_RESET(C_FLAG);
                SP = SP + byte;
            }
            CPU_FLAG_BIT_RESET(Z_FLAG);
            CPU_FLAG_BIT_RESET(N_FLAG);
            TimerUpdateCounter += 16;
            break;

        case JMP_adrHL:
            //4 Clock Cycles
            PC = HL.data;
            TimerUpdateCounter += 4;
            break;

        case LD_A_adr:
            //16 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            MemoryUnit->writeMemory(temp->data, AF.hi);
            TimerUpdateCounter += 16;
            break;

        case XOR_8IMM_A:
            //8 Clock Cycles
            xor8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;
            
        case RST_28:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0028;
            
            TimerUpdateCounter += 16;
            break;

        case LDH_IMMadr_A:
            //12 Clock Cycles
            AF.hi = MemoryUnit->readMemory(0xFF00 + MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 12;
            break;

        case POP_AF:
            //12 Clock Cycles
            AF.lo = 0xF0 & MemoryUnit->readMemory(SP++);
            AF.hi = MemoryUnit->readMemory(SP++);
            TimerUpdateCounter += 12;
            break;

        case DISABLE_INT:
            //4 Clock Cycles
            IME = false;
            TimerUpdateCounter += 4;
            break;

        case PUSH_AF:
            //16 clock cycles
            MemoryUnit->writeMemory(--SP, AF.hi);
            MemoryUnit->writeMemory(--SP, AF.lo);
            TimerUpdateCounter += 16;
            break;

        case OR_8IMM_A:
            //8 Clock Cycles
            or8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;

        case RST_30:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0030;
            TimerUpdateCounter += 16;
            break;

        case LDHL_S_8IMM_SP_HL:
            //12 Clock Cycles
            uBYTE TempByte;
            TempByte = MemoryUnit->readMemory(PC++);
            if (TestBitInByte(TempByte,7))
            {
                if (checkBorrowFromBit_Word(12, SP, twoComp_Byte(TempByte)))
                    CPU_FLAG_BIT_SET(H_FLAG);
                else
                    CPU_FLAG_BIT_RESET(H_FLAG);

                if (SP < twoComp_Word(TempByte))
                    CPU_FLAG_BIT_SET(C_FLAG);
                else
                    CPU_FLAG_BIT_RESET(C_FLAG);

                HL.data = SP - twoComp_Byte(TempByte);
            }
            else
            {
                if (checkCarryFromBit_Word(12, SP, TempByte))
                    CPU_FLAG_BIT_SET(H_FLAG);
                else
                    CPU_FLAG_BIT_RESET(H_FLAG);

                if (SP > (0xFFFF - TempByte))
                    CPU_FLAG_BIT_SET(C_FLAG);
                else
                    CPU_FLAG_BIT_RESET(C_FLAG);

                HL.data = SP + TempByte;
            }
            CPU_FLAG_BIT_RESET(Z_FLAG);
            CPU_FLAG_BIT_RESET(N_FLAG);
            TimerUpdateCounter += 12;
            break;

        case LD_HL_SP:
            //8 Clock Cycles
            SP = HL.data;
            TimerUpdateCounter += 8;
            break;

        case LD_16adr_A:
            //16 Clock Cycles
            temp->lo = MemoryUnit->readMemory(PC++);
            temp->hi = MemoryUnit->readMemory(PC++);
            AF.hi = MemoryUnit->readMemory(temp->data);
            TimerUpdateCounter += 16;
            break;

        case ENABLE_INT:
            //4 Clock Cycles
            IME = true;
            TimerUpdateCounter += 4;
            break;

        case CMP_8IMM_A:
            //8 Clock Cycles
            cmp8BitRegister(AF.hi, MemoryUnit->readMemory(PC++));
            TimerUpdateCounter += 8;
            break;

        case RST_38:
            //16 Clock Cycles
            temp->data = PC;
            MemoryUnit->writeMemory(--SP, temp->hi);
            MemoryUnit->writeMemory(--SP, temp->lo);
            PC = 0x0038;
            TimerUpdateCounter += 16;
            break;

        default:
            break;
        }
        delete temp;

#ifdef FUUGB_DEBUG
        if (MemoryUnit->DMA_read(0xFF02) == 0x81)
        {
            printf("%c", MemoryUnit->DMA_read(0xFF01));
            MemoryUnit->DMA_write(0xFF02, 0x00);
        }
#endif
        return TimerUpdateCounter;
    }

    void CPU::increment16BitRegister(uWORD& reg)
    {
        ++reg;
    }

    void CPU::increment8BitRegister(uBYTE& reg)
    {
        if (checkCarryFromBit_Byte(4, reg, 0x01))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);
        
        ++reg;
        if (reg == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
    }

    void CPU::decrement8BitRegister(uBYTE& reg)
    {
        if (checkBorrowFromBit_Byte(4, reg, 0x01))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);
        --reg;
        if (reg == 0x0)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_SET(N_FLAG);
    }

    void CPU::decrement16BitRegister(uWORD& reg)
    {
        --reg;
    }

    void CPU::add16BitRegister(uWORD& host, uWORD operand)
    {
        CPU_FLAG_BIT_RESET(N_FLAG);

        if (checkCarryFromBit_Word(12, host, operand))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host > 0xFFFF - operand)
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
        
        host += operand;
    }

    bool CPU::TestBitInByte(uBYTE byte, int pos)
    {
        std::bitset<8> BitField(byte);
        return BitField.test(pos);
    }

    bool CPU::TestBitInWord(uWORD word, int pos)
    {
        std::bitset<16> BitField(word);
        return BitField.test(pos);
    }

    bool CPU::checkCarryFromBit_Byte(int pos, uBYTE byte, uBYTE addedByte)
    {
        uBYTE mask;
        
        switch(pos)
        {
            case 1: mask = 0x01; break;
            case 2: mask = 0x03; break;
            case 3: mask = 0x07; break;
            case 4: mask = 0x0F; break;
            case 5: mask = 0x1F; break;
            case 6: mask = 0x3F; break;
            case 7: mask = 0x7F; break;
        }
        
        std::bitset<8> a(byte & mask);
        std::bitset<8> b(addedByte & mask);
        
        if(a.to_ulong() + b.to_ulong() > mask)
            return true;
        else
            return false;
    }
    
    bool CPU::checkCarryFromBit_Word(int pos, uWORD word, uWORD addedWord)
    {
        uWORD mask;
        
        switch(pos)
        {
            case 1: mask = 0x0001; break;
            case 2: mask = 0x0003; break;
            case 3: mask = 0x0007; break;
            case 4: mask = 0x000F; break;
            case 5: mask = 0x001F; break;
            case 6: mask = 0x003F; break;
            case 7: mask = 0x007F; break;
            case 8: mask = 0x00FF; break;
            case 9: mask = 0x01FF; break;
            case 10: mask = 0x03FF; break;
            case 11: mask = 0x07FF; break;
            case 12: mask = 0x0FFF; break;
            case 13: mask = 0x1FFF; break;
            case 14: mask = 0x3FFF; break;
            case 15: mask = 0x7FFF; break;
        }
        
        std::bitset<16> a(word & mask);
        std::bitset<16> b(addedWord & mask);
        
        if(a.to_ulong() + b.to_ulong() > mask)
            return true;
        else
            return false;
    }
    
    bool CPU::checkBorrowFromBit_Byte(int pos, uBYTE byte, uBYTE subtractedByte)
    {
        uBYTE mask;

        switch (pos)
        {
        case 1: mask = 0x01; break;
        case 2: mask = 0x03; break;
        case 3: mask = 0x07; break;
        case 4: mask = 0x0F; break;
        case 5: mask = 0x1F; break;
        case 6: mask = 0x3F; break;
        case 7: mask = 0x7F; break;
        }

        byte = byte & mask;
        subtractedByte = subtractedByte & mask;

        if (byte < subtractedByte)
            return true;
        else
            return false;
    }
    
    bool CPU::checkBorrowFromBit_Word(int pos, uWORD word, uWORD subtractedWord)
    {
        uWORD mask;

        switch (pos)
        {
        case 1: mask = 0x0001; break;
        case 2: mask = 0x0003; break;
        case 3: mask = 0x0007; break;
        case 4: mask = 0x000F; break;
        case 5: mask = 0x001F; break;
        case 6: mask = 0x003F; break;
        case 7: mask = 0x007F; break;
        case 8: mask = 0x00FF; break;
        case 9: mask = 0x01FF; break;
        case 10: mask = 0x03FF; break;
        case 11: mask = 0x07FF; break;
        case 12: mask = 0x0FFF; break;
        case 13: mask = 0x1FFF; break;
        case 14: mask = 0x3FFF; break;
        case 15: mask = 0x7FFF; break;
        }

        std::bitset<16> a(word & mask);
        std::bitset<16> b(subtractedWord & mask);

        if (a.to_ulong() < b.to_ulong())
            return true;
        else
            return false;
    }

    uBYTE CPU::twoComp_Byte(uBYTE byte)
    {
        std::bitset<8> twoCompByte(byte);

        twoCompByte.flip();
        
        uBYTE result = twoCompByte.to_ulong() + 0x01;

        return result;
    }

    uWORD CPU::twoComp_Word(uWORD word)
    {
        std::bitset<16> twoCompWord(word);

        twoCompWord.flip();

        uWORD result = twoCompWord.to_ulong() + 0x01;

        return result;
    }

    void CPU::add8BitRegister(uBYTE& host, uBYTE operand)
    {
        if (checkCarryFromBit_Byte(4, host, operand))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host > 0xFF - operand)
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
        
        host = host + operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
    }

    void CPU::add8BitRegister(uBYTE& host, uBYTE operand, bool carry)
    {
        uBYTE c;

        if (carry) {
            c = 0x1;
        } else {
            c = 0x0;
        }

        if (checkCarryFromBit_Byte(4, host, operand + c))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host > (0xFF - (c + operand)))
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
        
        host = host + operand + c;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
    }

    void CPU::sub8BitRegister(uBYTE& host, uBYTE operand)
    {
        if (checkBorrowFromBit_Byte(4, host, operand))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host < operand)
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
        
        host = host - operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_SET(N_FLAG);
    }

    void CPU::sub8BitRegister(uBYTE& host, uBYTE operand, bool carry)
    {
        if (checkBorrowFromBit_Byte(4, host, operand + carry))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host < (operand + carry))
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
        
        host = host - (operand + carry);

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_SET(N_FLAG);
    }

    void CPU::and8BitRegister(uBYTE& host, uBYTE operand)
    {
        host = host & operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_SET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);
    }

    void CPU::xor8BitRegister(uBYTE& host, uBYTE operand)
    {
        host = host ^ operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);
    }

    void CPU::or8BitRegister(uBYTE& host, uBYTE operand)
    {
        host = host | operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);
    }

    void CPU::cmp8BitRegister(uBYTE host, uBYTE operand)
    {
        if (host == operand)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_SET(N_FLAG);
        
        if (checkBorrowFromBit_Byte(4, host, operand))
            CPU_FLAG_BIT_SET(H_FLAG);
        else
            CPU_FLAG_BIT_RESET(H_FLAG);

        if (host < operand)
            CPU_FLAG_BIT_SET(C_FLAG);
        else
            CPU_FLAG_BIT_RESET(C_FLAG);
    }

    void CPU::rotateReg(bool direction, bool withCarry, uBYTE& reg)
    {
        std::bitset<8> BitField(reg);
        if (direction) //left
        {
            bool oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
            bool MSB = BitField[7];

            if (MSB)
                CPU_FLAG_BIT_SET(C_FLAG);
            else
                CPU_FLAG_BIT_RESET(C_FLAG);

            reg = reg << 1;

            if (withCarry && oldCarry)
                reg = reg | 0x01;
            else if (!withCarry)
            {
                reg |= MSB;
            }
        }
        else //Right
        {
            bool oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
            bool LSB = BitField[0];

            if (LSB)
                CPU_FLAG_BIT_SET(C_FLAG);
            else
                CPU_FLAG_BIT_RESET(C_FLAG);

            reg = reg >> 1;

            if (withCarry && oldCarry)
                reg = reg | 0x80;
            else if (!withCarry)
            {
                if (LSB)
                    reg |= 0x80;
                else
                    reg |= 0x00;
            }
        }

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);

        if (reg == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

    }

    void CPU::shiftReg(bool direction, bool keepMSB, uBYTE& reg)
    {
        std::bitset<8> BitField(reg);
        if (direction) //left
        {
            bool oldMSB = BitField[7];

            if (BitField[7])
                CPU_FLAG_BIT_SET(C_FLAG);
            else
                CPU_FLAG_BIT_RESET(C_FLAG);

            reg = reg << 1;
        }
        else //Right
        {
            bool oldMSB = BitField[7];
            if (BitField[0])
                CPU_FLAG_BIT_SET(C_FLAG);
            else
                CPU_FLAG_BIT_RESET(C_FLAG);

            reg = reg >> 1;
            
            if (keepMSB)
            {
                if (oldMSB)
                    reg |= 0x80;
                else
                    reg &= 0x7F;
            }
        }

        if (reg == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);


        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
    }

    void CPU::swapReg(uBYTE& reg)
    {
        std::bitset<8> BitField(reg);
        bool lowNibble[4];
        bool hiNibble[4];

        for (int i = 0; i < 4; ++i)
            lowNibble[i] = BitField[i];
        for (int i = 0; i < 4; ++i)
            hiNibble[i] = BitField[i + 4];

        for (int i = 0; i < 4; ++i)
            BitField[i] = hiNibble[i];

        for (int i = 0; i < 4; ++i)
            BitField[i + 4] = lowNibble[i];

        reg = BitField.to_ulong();

        if (reg == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);


        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);
    }

    void CPU::Flag_set(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        FlagBits.set(flag);
        AF.lo = FlagBits.to_ulong();
    }

    void CPU::Flag_reset(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        FlagBits.reset(flag);
        AF.lo = FlagBits.to_ulong();
    }

    bool CPU::Flag_test(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        return FlagBits.test(flag);
    }

    void CPU::test_bit(int pos, uBYTE reg)
    {
        std::bitset<8> BitField(reg);
        if (!BitField.test(pos))
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_SET(H_FLAG);
    }

    void CPU::reset_bit(int pos, uBYTE& reg)
    {
        std::bitset<8> BitField(reg);
        BitField.reset(pos);

        reg = BitField.to_ulong();
    }

    void CPU::set_bit(int pos, uBYTE& reg)
    {
        std::bitset<8> BitField(reg);
        BitField.set(pos);

        reg = BitField.to_ulong();
    }

    void CPU::CheckInterupts()
    {
        if (!IME)
            return;

        std::bitset<8> IE(MemoryUnit->readMemory(INTERUPT_EN_REGISTER_ADR));
        std::bitset<8> IF(MemoryUnit->readMemory(INTERUPT_FLAG_REG));
        Register Temp;

        if (IF[0] && IE[0]) //V-Blank
        {
            IME = false;
            IF.reset(0);
            MemoryUnit->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            MemoryUnit->DMA_write(--SP, Temp.hi);
            MemoryUnit->DMA_write(--SP, Temp.lo);
            PC = VBLANK_INT;
        }
        else if (IF[1] && IE[1]) // LCDC
        {
            IME = false;
            IF.reset(1);
            MemoryUnit->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            MemoryUnit->DMA_write(--SP, Temp.hi);
            MemoryUnit->DMA_write(--SP, Temp.lo);
            PC = LCDC_INT;
        }
        else if (IF[2] && IE[2]) // Timer Overflow
        {
            IME = false;
            IF.reset(2);
            MemoryUnit->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            MemoryUnit->DMA_write(--SP, Temp.hi);
            MemoryUnit->DMA_write(--SP, Temp.lo);
            PC = TIMER_OVER_INT;
        }
        else if (IF[3] && IE[3]) // Serial I/O Complete
        {
            IME = false;
            IF.reset(3);
            MemoryUnit->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            MemoryUnit->DMA_write(--SP, Temp.hi);
            MemoryUnit->DMA_write(--SP, Temp.lo);
            PC = SER_TRF_INT;
        }
        else if (IF[4] && IE[4]) //Pin 10 - 13 hi to lo (Control Input)
        {
            IME = false;
            IF.reset(4);
            MemoryUnit->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            MemoryUnit->DMA_write(--SP, Temp.hi);
            MemoryUnit->DMA_write(--SP, Temp.lo);
            PC = CONTROL_INT;
        }
    }

    void CPU::UpdateTimers(int cycles)
    {
        std::bitset<8> TMC(MemoryUnit->readMemory(0xFF07));
        std::bitset<8> TMA(MemoryUnit->readMemory(0xFF06));
        std::bitset<8> TIMA(MemoryUnit->readMemory(0xFF05));
        
        this->updateDivider(cycles);
        
        if(TMC.test(2)) //Check if clock is enabled
        {
            MemoryUnit->timer_counter -= cycles;
            
            if(MemoryUnit->timer_counter <= 0)
            {
                uBYTE frequency = MemoryUnit->readMemory(0xFF07) & 0x03;
                switch(frequency)
                {
                    case 0: MemoryUnit->timer_counter = 1024; break;
                    case 1: MemoryUnit->timer_counter = 16; break;
                    case 2: MemoryUnit->timer_counter = 64; break;
                    case 3: MemoryUnit->timer_counter = 256; break;
                }
                
                //Timer Overflow
                if(MemoryUnit->readMemory(0xFF05) == 255)
                {
                    MemoryUnit->writeMemory(0xFF05, MemoryUnit->readMemory(0xFF06));
                    MemoryUnit->RequestInterupt(2);
                }
                else
                    MemoryUnit->writeMemory(0xFF05, MemoryUnit->readMemory(0xFF05)+1);
            }
        }
        
    }

    void CPU::updateDivider(int cycles)
    {
        DividerRegisterCounter += cycles;
        if(DividerRegisterCounter >= 255)
        {
            MemoryUnit->DMA_write(0xFF04, MemoryUnit->DMA_read(0xFF04)+1);
            DividerRegisterCounter = 0;
        }
    }

    void CPU::adjustDAA(uBYTE & reg)
    {
        if (!CPU_FLAG_BIT_TEST(N_FLAG))
        {
            if (CPU_FLAG_BIT_TEST(C_FLAG) || (reg > 0x99))
            {
                reg += 0x60;
                CPU_FLAG_BIT_SET(C_FLAG);
            }

            if (CPU_FLAG_BIT_TEST(H_FLAG) || ((reg & 0x0F) > 0x09))
                reg += 0x06;
        }
        else
        {
            if (CPU_FLAG_BIT_TEST(C_FLAG))
                reg -= 0x60;

            if (CPU_FLAG_BIT_TEST(H_FLAG))
                reg -= 0x06;
        }

        if (reg == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(H_FLAG);
    }
    
    void CPU::Halt()
    {
        std::bitset<8> IE(MemoryUnit->DMA_read(INTERUPT_EN_REGISTER_ADR));
        std::bitset<8> IF(MemoryUnit->DMA_read(INTERUPT_FLAG_REG));
        if (IF[0] && IE[0]) //V-Blank
        {
            CpuHalted = false;
        }
        else if (IF[1] && IE[1]) // LCDC
        {
            CpuHalted = false;
        }
        else if (IF[2] && IE[2]) // Timer Overflow
        {
            CpuHalted = false;
        }
        else if (IF[3] && IE[3]) // Serial I/O Complete
        {
            CpuHalted = false;
        }
        else if (IF[4] && IE[4]) //Pin 10 - 13 hi to lo (Control Input)
        {
            CpuHalted = false;
        }
    }
}
