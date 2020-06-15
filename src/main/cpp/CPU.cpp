#include "Core.h"
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

        memoryUnit = mem;

        timerUpdateCounter = 0;
        dividerRegisterCounter = 0;
        
        Paused  = false;
        Halted  = false;
        IME     = false;
    }

    CPU::~CPU()
    {

    }

    void CPU::Pause()
    {
        Paused = true;
    }

    int CPU::ExecuteNextOpCode()
    {
        timerUpdateCounter = 0;
        uBYTE byte = memoryUnit->Read(PC++);
        uBYTE SP_data = 0x0;
        reg* temp = new reg();
        
        switch (byte)
        {
        case NOP:
            //4 CPU Cycle
            timerUpdateCounter += 4;
            break;

        case LD_16IMM_BC:
            //12 CPU Cycles
            BC.lo = memoryUnit->Read(PC++);
            BC.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 12;
            break;

        case LD_A_adrBC:
            //8 CPU Cycles
            memoryUnit->Write(BC.data, AF.hi);
            timerUpdateCounter += 8;
            break;

        case INC_BC:
            //8 CPU Cycles
            BC.data = increment16BitRegister(BC.data);
            timerUpdateCounter += 8;
            break;

        case INC_B:
            //4 CPU Cycles
            BC.hi = increment8BitRegister(BC.hi);
            timerUpdateCounter += 4;
            break;

        case DEC_B:
            //4 CPU Cycles
            BC.hi = decrement8BitRegister(BC.hi);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_B:
            //8 CPU Cycles
            BC.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case RLC_A:
            //4 CPU Cycles
            AF.hi = rotateReg(true, false, AF.hi);
            timerUpdateCounter += 4;
            break;

        case LD_SP_adr:
            //20 CPU cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            SP_data = memoryUnit->Read(SP);
            memoryUnit->Write(temp->data, SP_data);
            timerUpdateCounter += 20;
            break;

        case ADD_BC_HL:
            //8 CPU Cycles
            HL.data = add16BitRegister(HL.data, BC.data);
            timerUpdateCounter += 8;
            break;

        case LD_adrBC_A:
            //8 CPU Cycles
            AF.hi = memoryUnit->Read(BC.data);
            timerUpdateCounter += 8;
            break;

        case DEC_BC:
            //8 CPU Cycles
            BC.data = decrement16BitRegister(BC.data);
            timerUpdateCounter += 8;
            break;

        case INC_C:
            //4 CPU Cycles
            BC.lo = increment8BitRegister(BC.lo);
            timerUpdateCounter += 4;
            break;

        case DEC_C:
            //4 CPU Cycles
            BC.lo = decrement8BitRegister(BC.lo);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_C:
            //8 CPU Cycles
            BC.lo = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case RRC_A:
            //4 CPU Cycles
            AF.hi = rotateReg(false, false, AF.hi);
            timerUpdateCounter += 4;
            break;

        case STOP:
            //4 Clock Cycles
            Paused = true;
            break;

        case LD_16IMM_DE:
            //12 Clock Cycles
            DE.lo = memoryUnit->Read(PC++);
            DE.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 12;
            break;

        case LD_A_adrDE:
            //8 Clock Cycles
            memoryUnit->Write(DE.data, AF.hi);
            timerUpdateCounter += 8;
            break;

        case INC_DE:
            //8 Clock Cycles
            DE.data = increment16BitRegister(DE.data);
            timerUpdateCounter += 8;
            break;

        case INC_D:
            //4 Clock Cycles
            DE.hi = increment8BitRegister(DE.hi);
            timerUpdateCounter += 4;
            break;

        case DEC_D:
            //4 Clock Cycles
            DE.hi = decrement8BitRegister(DE.hi);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_D:
            //8 Clock Cycles
            DE.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case RL_A:
            //4 Clock Cycles
            AF.hi = rotateReg(true, true, AF.hi);
            timerUpdateCounter += 4;
            break;

        case RJmp_IMM:
            //12 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (testBitInByte(byte, 7))
                PC = PC - twoComp_Byte(byte);
            else
                PC = PC + byte;
            timerUpdateCounter += 12;
            break;

        case ADD_DE_HL:
            //8 Clock Cycles
            HL.data = add16BitRegister(HL.data, DE.data);
            timerUpdateCounter += 8;
            break;

        case LD_adrDE_A:
            //8 Clock Cycles
            AF.hi = memoryUnit->Read(DE.data);
            timerUpdateCounter += 8;
            break;

        case DEC_DE:
            //4 Clock Cycles
            DE.data = decrement16BitRegister(DE.data);
            timerUpdateCounter += 4;
            break;

        case INC_E:
            //4 Clock Cycles
            DE.lo = increment8BitRegister(DE.lo);
            timerUpdateCounter += 4;
            break;

        case DEC_E:
            //4 clock cycles
            DE.lo = decrement8BitRegister(DE.lo);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_E:
            //8 Clock Cycles
            DE.lo = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case RR_A:
            //4 clock cycles
            AF.hi = rotateReg(false, true, AF.hi);
            timerUpdateCounter += 4;
            break;

        case RJmp_NOTZERO:
            //8 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                if (testBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            timerUpdateCounter += 8;
            break;

        case LD_16IMM_HL:
            //12 Clock Cycles
            HL.lo = memoryUnit->Read(PC++);
            HL.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 12;
            break;

        case LDI_A_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data++, AF.hi);
            timerUpdateCounter += 8;
            break;

        case INC_HL:
            //4 Clock Cycles
            HL.data = increment16BitRegister(HL.data);
            timerUpdateCounter += 4;
            break;

        case INC_H:
            //4 Clock Cycles
            HL.hi = increment8BitRegister(HL.hi);
            timerUpdateCounter += 4;
            break;

        case DEC_H:
            //4 Clock Cycles
            DE.hi = decrement8BitRegister(DE.hi);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_H:
            //8 Clock Cycles
            HL.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case DAA:
            //4 Clock Cycles
            AF.hi = adjustDAA(AF.hi);
            timerUpdateCounter += 4;
            break;

        case RJmp_ZERO:
            //8 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                if (testBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            timerUpdateCounter += 8;
            break;

        case ADD_HL_HL:
            //8 Clock Cycles
            HL.data = add16BitRegister(HL.data, HL.data);
            timerUpdateCounter += 8;
            break;

        case LDI_adrHL_A:
            //8 Clock Cycles
            AF.hi = memoryUnit->Read(HL.data++);
            timerUpdateCounter += 8;
            break;

        case DEC_HL:
            //4 Clock Cycles
            HL.data = decrement16BitRegister(HL.data);
            timerUpdateCounter += 4;
            break;

        case INC_L:
            //4 Clock Cycles
            HL.lo = increment8BitRegister(HL.lo);
            timerUpdateCounter += 4;
            break;

        case DEC_L:
            //4 Clock Cycles
            HL.lo = decrement8BitRegister(HL.lo);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_L:
            //8 Clock Cycles
            HL.lo = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case NOT_A:
            //4 Clock Cycles
            AF.hi = ~AF.hi;
            CPU_FLAG_BIT_SET(N_FLAG);
            CPU_FLAG_BIT_SET(H_FLAG);
            timerUpdateCounter += 4;
            break;

        case RJmp_NOCARRY:
            //8 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {
                if (testBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            timerUpdateCounter += 8;
            break;

        case LD_16IM_SP:
            //12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            SP = temp->data;
            timerUpdateCounter += 12;
            break;

        case LDD_A_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data--, AF.hi);
            timerUpdateCounter += 8;
            break;

        case INC_SP:
            //8 Clock Cycles
            SP = increment16BitRegister(SP);
            timerUpdateCounter += 8;
            break;

        case INC_valHL:
            //12 Clock Cycles
            byte = increment8BitRegister(memoryUnit->Read(HL.data));
            memoryUnit->Write(HL.data, byte);
            timerUpdateCounter += 12;
            break;
            
        case DEC_valHL:
            //12 Clock Cycles
            byte = decrement8BitRegister(memoryUnit->Read(HL.data));
            memoryUnit->Write(HL.data, byte);
            timerUpdateCounter += 12;
            break;

        case LD_8IMM_adrHL:
            //12 Clock Cycles
            byte = memoryUnit->Read(PC++);
            memoryUnit->Write(HL.data, byte);
            timerUpdateCounter += 12;
            break;

        case SET_CARRY_FLAG:
            //4 Clock Cycles
            CPU_FLAG_BIT_RESET(N_FLAG);
            CPU_FLAG_BIT_RESET(H_FLAG);
            CPU_FLAG_BIT_SET(C_FLAG);
            timerUpdateCounter += 4;
            break;

        case RJmp_CARRY:
            //8 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                if (testBitInByte(byte, 7))
                    PC = PC - twoComp_Byte(byte);
                else
                    PC = PC + byte;
            }
            timerUpdateCounter += 8;
            break;

        case ADD_SP_HL:
            //8 Clock Cycles
            HL.data = add16BitRegister(HL.data, SP);
            timerUpdateCounter += 8;
            break;

        case LDD_adrHL_A:
            //8 Clock Cycles
            AF.hi = memoryUnit->Read(HL.data--);
            timerUpdateCounter += 8;
            break;

        case DEC_SP:
            //8 Clock Cycles;
            SP = decrement16BitRegister(SP);
            timerUpdateCounter += 8;
            break;

        case INC_A:
            //4 Clock Cycles
            AF.hi = increment8BitRegister(AF.hi);
            timerUpdateCounter += 4;
            break;

        case DEC_A:
            //4 Clock Cycles
            AF.hi = decrement8BitRegister(AF.hi);
            timerUpdateCounter += 4;
            break;

        case LD_8IMM_A:
            //8 Clock Cycles
            AF.hi = memoryUnit->Read(PC++);
            timerUpdateCounter += 8;
            break;

        case COMP_CARRY_FLAG:
            //4 Clock Cycles
            if (CPU_FLAG_BIT_TEST(C_FLAG))
                CPU_FLAG_BIT_RESET(C_FLAG);
            else
                CPU_FLAG_BIT_SET(C_FLAG);
            
            CPU_FLAG_BIT_RESET(N_FLAG);
            CPU_FLAG_BIT_RESET(H_FLAG);
            timerUpdateCounter += 4;
            break;

        case LD_B_B:
            //4 Clock Cycles
            BC.hi = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_B:
            //4 Clock Cycles
            BC.hi = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_B:
            //4 Clock Cycles
            BC.hi = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_B:
            //4 Clock Cycles
            BC.hi = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_B:
            //4 Clock Cycles
            BC.hi = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_B:
            //4 Clock Cycles
            BC.hi = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_B:
            //8 Clock Cycles
            BC.hi = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_B:
            //4 Clock Cycles
            BC.hi = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_C:
            //4 Clock Cycles
            BC.lo = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_C:
            //4 Clock Cycles
            BC.lo = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_C:
            //4 Clock Cycles
            BC.lo = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_C:
            //4 Clock Cycles
            BC.lo = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_C:
            //4 Clock Cycles
            BC.lo = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_C:
            //4 Clock Cycles
            BC.lo = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_C:
            //8 Clock Cycles
            BC.lo = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_C:
            //4 Clock Cycles
            BC.lo = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_D:
            //4 Clock Cycles
            DE.hi = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_D:
            //4 Clock Cycles
            DE.hi = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_D:
            //4 Clock Cycles
            DE.hi = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_D:
            //4 Clock Cycles
            DE.hi = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_D:
            //4 Clock Cycles
            DE.hi = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_D:
            //4 Clock Cycles
            DE.hi = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_D:
            //8 Clock Cycles
            DE.hi = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_D:
            //4 Clock Cycles
            DE.hi = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_E:
            //4 Clock Cycles
            DE.lo = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_E:
            //4 Clock Cycles
            DE.lo = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_E:
            //4 Clock Cycles
            DE.lo = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_E:
            //4 Clock Cycles
            DE.lo = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_E:
            //4 Clock Cycles
            DE.lo = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_E:
            //4 Clock Cycles
            DE.lo = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_E:
            //8 Clock Cycles
            DE.lo = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_E:
            //4 Clock Cycles
            DE.lo = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_H:
            //4 Clock Cycles
            HL.hi = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_H:
            //4 Clock Cycles
            HL.hi = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_H:
            //4 Clock Cycles
            HL.hi = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_H:
            //4 Clock Cycles
            HL.hi = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_H:
            //4 Clock Cycles
            HL.hi = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_H:
            //4 Clock Cycles
            HL.hi = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_H:
            //8 Clock Cycles
            HL.hi = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_H:
            //4 Clock Cycles
            HL.hi = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_L:
            //4 Clock Cycles
            HL.lo = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_L:
            //4 Clock Cycles
            HL.lo = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_L:
            //4 Clock Cycles
            HL.lo = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_L:
            //4 Clock Cycles
            HL.lo = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_L:
            //4 Clock Cycles
            HL.lo = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_L:
            //4 Clock Cycles
            HL.lo = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_L:
            //8 Clock Cycles
            HL.lo = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_L:
            //4 Clock Cycles
            HL.lo = AF.hi;
            timerUpdateCounter += 4;
            break;

        case LD_B_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, BC.hi);
            timerUpdateCounter += 8;
            break;

        case LD_C_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, BC.lo);
            timerUpdateCounter += 8;
            break;

        case LD_D_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, DE.hi);
            timerUpdateCounter += 8;
            break;
            
        case LD_E_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, DE.lo);
            timerUpdateCounter += 8;
            break;

        case LD_H_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, HL.hi);
            timerUpdateCounter += 8;
            break;

        case LD_L_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, HL.lo);
            timerUpdateCounter += 8;
            break;

        case HALT:
            //4 Clock Cycles
            Halted = true;
            Halt();
            timerUpdateCounter += 4;
            break;

        case LD_A_adrHL:
            //8 Clock Cycles
            memoryUnit->Write(HL.data, AF.hi);
            timerUpdateCounter += 8;
            break;

        case LD_B_A:
            //4 Clock Cycles
            AF.hi = BC.hi;
            timerUpdateCounter += 4;
            break;

        case LD_C_A:
            //4 Clock Cycles
            AF.hi = BC.lo;
            timerUpdateCounter += 4;
            break;

        case LD_D_A:
            //4 Clock Cycles
            AF.hi = DE.hi;
            timerUpdateCounter += 4;
            break;

        case LD_E_A:
            //4 Clock Cycles
            AF.hi = DE.lo;
            timerUpdateCounter += 4;
            break;

        case LD_H_A:
            //4 Clock Cycles
            AF.hi = HL.hi;
            timerUpdateCounter += 4;
            break;

        case LD_L_A:
            //4 Clock Cycles
            AF.hi = HL.lo;
            timerUpdateCounter += 4;
            break;

        case LD_adrHL_A:
            //8 Clock Cycles
            AF.hi = memoryUnit->Read(HL.data);
            timerUpdateCounter += 8;
            break;

        case LD_A_A:
            //4 Clock Cycles
            AF.hi = AF.hi;
            timerUpdateCounter += 4;
            break;

        case ADD_B_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case ADD_C_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break;

        case ADD_D_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case ADD_E_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case ADD_H_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case ADD_L_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case ADD_adrHL_A:
            //8 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 8;
            break;

        case ADD_A_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case ADC_B_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_C_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_D_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_E_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_H_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_L_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case ADC_adrHL_A:
            //8 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, memoryUnit->Read(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 8;
            break;

        case ADC_A_A:
            //4 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SUB_B_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case SUB_C_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break; 

        case SUB_D_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case SUB_E_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case SUB_H_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case SUB_L_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case SUB_adrHL_A:
            //8 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 8;
            break;

        case SUB_A_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case SBC_B_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_C_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_D_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_E_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_H_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_L_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case SBC_adrHL_A:
            //8 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, memoryUnit->Read(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 8;
            break;

        case SBC_A_A:
            //4 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 4;
            break;

        case AND_B_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case AND_C_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break;

        case AND_D_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case AND_E_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case AND_H_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case AND_L_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case AND_adrHL_A:
            //8 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 8;
            break;

        case AND_A_A:
            //4 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case XOR_B_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case XOR_C_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break;

        case XOR_D_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case XOR_E_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case XOR_H_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case XOR_L_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case XOR_adrHL_A:
            //8 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 8;
            break;

        case XOR_A_A:
            //4 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case OR_B_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case OR_C_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break;

        case OR_D_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case OR_E_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case OR_H_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case OR_L_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case OR_adrHL_A:
            //8 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 4;
            break;

        case OR_A_A:
            //4 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case CMP_B_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, BC.hi);
            timerUpdateCounter += 4;
            break;

        case CMP_C_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, BC.lo);
            timerUpdateCounter += 4;
            break;

        case CMP_D_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, DE.hi);
            timerUpdateCounter += 4;
            break;

        case CMP_E_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, DE.lo);
            timerUpdateCounter += 4;
            break;

        case CMP_H_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, HL.hi);
            timerUpdateCounter += 4;
            break;

        case CMP_L_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, HL.lo);
            timerUpdateCounter += 4;
            break;

        case CMP_adrHL_A:
            //8 Clock Cycles
            cmp8BitRegister(AF.hi, memoryUnit->Read(HL.data));
            timerUpdateCounter += 8;
            break;

        case CMP_A_A:
            //4 Clock Cycles
            cmp8BitRegister(AF.hi, AF.hi);
            timerUpdateCounter += 4;
            break;

        case RET_NOT_ZERO:
            //20/8 Clock Cycles
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                temp->lo = memoryUnit->Read(SP++);
                temp->hi = memoryUnit->Read(SP++);
                PC = temp->data;
                
                timerUpdateCounter += 20;
            }
            else
                timerUpdateCounter += 8;
            break;

        case POP_BC:
            //12 Clock Cycles
            BC.lo = memoryUnit->Read(SP++);
            BC.hi = memoryUnit->Read(SP++);
            timerUpdateCounter += 12;
            break;

        case JMP_NOT_ZERO:
            //16/12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {    
                PC = temp->data;
                timerUpdateCounter += 16;
            }
            else
                timerUpdateCounter += 12;
            break;

        case JMP:
            //16 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            
            PC = temp->data;
            timerUpdateCounter += 16;
            break;

        case CALL_NOT_ZERO:
            //24/12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                reg temp2;
                temp2.data = PC;
                memoryUnit->Write(--SP, temp2.hi);
                memoryUnit->Write(--SP, temp2.lo);
                PC = temp->data;
                timerUpdateCounter += 24;
            }
            else
                timerUpdateCounter += 12;
            break;

        case PUSH_BC:
            //16 clock cycles
            memoryUnit->Write(--SP, BC.hi);
            memoryUnit->Write(--SP, BC.lo);
            timerUpdateCounter += 16;
            break;

        case ADD_IMM_A:
            //8 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;

        case RST_0:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0000;
            timerUpdateCounter += 16;
            break;

        case RET_ZERO:
            //8 Clock Cycles if cc false else 20 clock cycles
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                temp->lo = memoryUnit->Read(SP++);
                temp->hi = memoryUnit->Read(SP++);
                PC = temp->data;
                timerUpdateCounter += 20;
            }
            else
                timerUpdateCounter += 8;
            break;

        case RETURN:
            //16 Clock Cycles
            temp->lo = memoryUnit->Read(SP++);
            temp->hi = memoryUnit->Read(SP++);
            PC = temp->data;
            timerUpdateCounter += 16;
            break;

        case JMP_ZERO:
            //16/12 Clock cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                PC = temp->data;
                timerUpdateCounter += 16;
            }
            else
                timerUpdateCounter += 12;
            break;

        case EXT_OP:
            //4 Clock Cycles, this opcode is special, it allows for 16 bit opcodes
            timerUpdateCounter += 4;
            byte = memoryUnit->Read(PC++);
            switch (byte)
            {
            case RLC_B:
                //4 clock Cycles
                BC.hi = rotateReg(true, false, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RLC_C:
                //4 clock Cycles
                BC.lo = rotateReg(true, false, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RLC_D:
                //4 clock Cycles
                DE.hi = rotateReg(true, false, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RLC_E:
                //4 clock Cycles
                DE.lo = rotateReg(true, false, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RLC_H:
                //4 clock Cycles
                HL.hi = rotateReg(true, false, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RLC_L:
                //4 clock Cycles
                HL.lo = rotateReg(true, false, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RLC_adrHL:
                //8 clock Cycles
                byte = rotateReg(true, false, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case eRLC_A:
                //4 clock Cycles
                AF.hi = rotateReg(true, false, AF.hi);
                timerUpdateCounter += 4;
                break;
                
            case RRC_B:
                //4 clock Cycles
                BC.hi = rotateReg(false, false, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RRC_C:
                //4 clock Cycles
                BC.lo = rotateReg(false, false, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RRC_D:
                //4 clock Cycles
                DE.hi = rotateReg(false, false, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RRC_E:
                //4 clock Cycles
                DE.lo = rotateReg(false, false, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RRC_H:
                //4 clock Cycles
                HL.hi = rotateReg(false, false, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RRC_L:
                //4 clock Cycles
                HL.lo = rotateReg(false, false, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RRC_adrHL:
                //8 clock Cycles
                byte = rotateReg(false, false, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case eRRC_A:
                //4 clock Cycles
                AF.hi = rotateReg(false, false, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RL_B:
                //4 clock Cycles
                BC.hi = rotateReg(true, true, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RL_C:
                //4 clock Cycles
                BC.lo = rotateReg(true, true, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RL_D:
                //4 clock Cycles
                DE.hi = rotateReg(true, true, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RL_E:
                //4 clock Cycles
                DE.lo = rotateReg(true, true, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RL_H:
                //4 clock Cycles
                HL.hi = rotateReg(true, true, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RL_L:
                //4 clock Cycles
                HL.lo = rotateReg(true, true, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RL_adrHL:
                //8 clock Cycles
                byte = rotateReg(true, true, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case eRL_A:
                //4 clock Cycles
                AF.hi = rotateReg(true, true, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RR_B:
                //4 clock Cycles
                BC.hi = rotateReg(false, true, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RR_C:
                //4 clock Cycles
                BC.lo = rotateReg(false, true, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RR_D:
                //4 clock Cycles
                DE.hi = rotateReg(false, true, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RR_E:
                //4 clock Cycles
                DE.lo = rotateReg(false, true, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RR_H:
                //4 clock Cycles
                HL.hi = rotateReg(false, true, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RR_L:
                //4 clock Cycles
                HL.lo = rotateReg(false, true, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RR_adrHL:
                //8 clock Cycles
                byte = rotateReg(false, true, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case eRR_A:
                //4 clock Cycles
                AF.hi = rotateReg(false, true, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SLA_B:
                //4 clock cycles
                BC.hi = shiftReg(true, true, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SLA_C:
                //4 clock cycles
                BC.lo = shiftReg(true, true, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SLA_D:
                //4 clock cycles
                DE.hi = shiftReg(true, true, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SLA_E:
                //4 clock cycles
                DE.lo = shiftReg(true, true, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SLA_H:
                //4 clock cycles
                HL.hi = shiftReg(true, true, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SLA_L:
                //4 clock cycles
                HL.lo = shiftReg(true, true, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SLA_adrHL:
                //8 Clock Cycles
                byte = shiftReg(true, true, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SLA_A:
                //8 Clock Cycles
                AF.hi = shiftReg(true, true, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SRA_B:
                //4 clock Cycles
                BC.hi = shiftReg(false, true, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SRA_C:
                //4 clock Cycles
                BC.lo = shiftReg(false, true, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SRA_D:
                //4 clock Cycles
                DE.hi = shiftReg(false, true, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SRA_E:
                //4 clock Cycles
                DE.lo = shiftReg(false, true, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SRA_H:
                //4 clock Cycles
                HL.hi = shiftReg(false, true, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SRA_L:
                //4 clock Cycles
                HL.lo = shiftReg(false, true, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SRA_adrHL:
                //8 clock Cycles
                byte = shiftReg(false, true, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SRA_A:
                //4 clock Cycles
                AF.hi = shiftReg(false, true, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SWAP_B:
                //4 Clock Cycles
                BC.hi = swapReg(BC.hi);
                timerUpdateCounter += 4;
                break;

            case SWAP_C:
                //4 Clock Cycles
                BC.lo = swapReg(BC.lo);
                timerUpdateCounter += 4;
                break;

            case SWAP_D:
                //4 Clock Cycles
                DE.hi = swapReg(DE.hi);
                timerUpdateCounter += 4;
                break;

            case SWAP_E:
                //4 Clock Cycles
                DE.lo = swapReg(DE.lo);
                timerUpdateCounter += 4;
                break;

            case SWAP_H:
                //4 Clock Cycles
                HL.hi = swapReg(HL.hi);
                timerUpdateCounter += 4;
                break;

            case SWAP_L:
                //4 Clock Cycles
                HL.lo = swapReg(HL.lo);
                timerUpdateCounter += 4;
                break;

            case SWAP_adrHL:
                //8 Clock Cycles
                byte = swapReg(memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SWAP_A:
                //4 Clock Cycles
                AF.hi = swapReg(AF.hi);
                timerUpdateCounter += 4;
                break;

            case SRL_B:
                //4 Clock Cycles
                BC.hi = shiftReg(false, false, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SRL_C:
                //4 Clock Cycles
                BC.lo = shiftReg(false, false, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SRL_D:
                //4 Clock Cycles
                DE.hi = shiftReg(false, false, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SRL_E:
                //4 Clock Cycles
                DE.lo = shiftReg(false, false, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SRL_H:
                //4 Clock Cycles
                HL.hi = shiftReg(false, false, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SRL_L:
                //4 Clock Cycles
                HL.lo = shiftReg(false, false, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SRL_adrHL:
                //8 Clock Cycles
                byte = shiftReg(false, false, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SRL_A:
                //4 Clock Cycles
                AF.hi = shiftReg(false, false, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_1_B:
                //4 Clock Cycles
                testBit(0, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_1_C:
                //4 Clock Cycles
                testBit(0, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_1_D:
                //4 Clock Cycles
                testBit(0, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_1_E:
                //4 Clock Cycles
                testBit(0, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_1_H:
                //4 Clock Cycles
                testBit(0, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_1_L:
                //4 Clock Cycles
                testBit(0, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_1_adrHL:
                //8 Clock Cycles
                testBit(0, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_1_A:
                //4 Clock Cycles
                testBit(0, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_2_B:
                //4 Clock Cycles
                testBit(1, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_2_C:
                //4 Clock Cycles
                testBit(1, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_2_D:
                //4 Clock Cycles
                testBit(1, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_2_E:
                //4 Clock Cycles
                testBit(1, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_2_H:
                //4 Clock Cycles
                testBit(1, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_2_L:
                //4 Clock Cycles
                testBit(1, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_2_adrHL:
                //8 Clock Cycles
                testBit(1, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_2_A:
                //4 Clock Cycles
                testBit(1, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_3_B:
                //4 Clock Cycles
                testBit(2, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_3_C:
                //4 Clock Cycles
                testBit(2, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_3_D:
                //4 Clock Cycles
                testBit(2, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_3_E:
                //4 Clock Cycles
                testBit(2, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_3_H:
                //4 Clock Cycles
                testBit(2, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_3_L:
                //4 Clock Cycles
                testBit(2, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_3_adrHL:
                //8 Clock Cycles
                testBit(2, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_3_A:
                //4 Clock Cycles
                testBit(2, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_4_B:
                //4 Clock Cycles
                testBit(3, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_4_C:
                //4 Clock Cycles
                testBit(3, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_4_D:
                //4 Clock Cycles
                testBit(3, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_4_E:
                //4 Clock Cycles
                testBit(3, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_4_H:
                //4 Clock Cycles
                testBit(3, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_4_L:
                //4 Clock Cycles
                testBit(3, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_4_adrHL:
                //8 Clock Cycles
                testBit(3, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_4_A:
                //4 Clock Cycles
                testBit(3, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_5_B:
                //4 Clock Cycles
                testBit(4, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_5_C:
                //4 Clock Cycles
                testBit(4, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_5_D:
                //4 Clock Cycles
                testBit(4, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_5_E:
                //4 Clock Cycles
                testBit(4, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_5_H:
                //4 Clock Cycles
                testBit(4, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_5_L:
                //4 Clock Cycles
                testBit(4, HL.lo);
                timerUpdateCounter += 4;
                break;
            
            case BIT_5_adrHL:
                //8 Clock Cycles
                testBit(4, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_5_A:
                //4 Clock Cycles
                testBit(4, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_6_B:
                //4 Clock Cycles
                testBit(5, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_6_C:
                //4 Clock Cycles
                testBit(5, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_6_D:
                //4 Clock Cycles
                testBit(5, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_6_E:
                //4 Clock Cycles
                testBit(5, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_6_H:
                //4 Clock Cycles
                testBit(5, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_6_L:
                //4 Clock Cycles
                testBit(5, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_6_adrHL:
                //8 Clock Cycles
                testBit(5, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;
                
            case BIT_6_A:
                //4 Clock Cycles
                testBit(5, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_7_B:
                //4 Clock Cycles
                testBit(6, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_7_C:
                //4 Clock Cycles
                testBit(6, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_7_D:
                //4 Clock Cycles
                testBit(6, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_7_E:
                //4 Clock Cycles
                testBit(6, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_7_H:
                //4 Clock Cycles
                testBit(6, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_7_L:
                //4 Clock Cycles
                testBit(6, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_7_adrHL:
                //8 Clock Cycles
                testBit(6, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_7_A:
                //4 Clock Cycles
                testBit(6, AF.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_8_B:
                //4 Clock Cycles
                testBit(7, BC.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_8_C:
                //4 Clock Cycles
                testBit(7, BC.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_8_D:
                //4 Clock Cycles
                testBit(7, DE.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_8_E:
                //4 Clock Cycles
                testBit(7, DE.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_8_H:
                //4 Clock Cycles
                testBit(7, HL.hi);
                timerUpdateCounter += 4;
                break;

            case BIT_8_L:
                //4 Clock Cycles
                testBit(7, HL.lo);
                timerUpdateCounter += 4;
                break;

            case BIT_8_adrHL:
                //8 Clock Cycles
                testBit(7, memoryUnit->Read(HL.data));
                timerUpdateCounter += 8;
                break;

            case BIT_8_A:
                //4 Clock Cycles
                testBit(7, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_1_B:
                //4 Clock Cycles
                BC.hi = resetBit(0, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_1_C:
                //4 Clock Cycles
                BC.lo = resetBit(0, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_1_D:
                //4 Clock Cycles
                DE.hi = resetBit(0, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_1_E:
                //4 Clock Cycles
                DE.lo = resetBit(0, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_1_H:
                //4 Clock Cycles
                HL.hi = resetBit(0, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_1_L:
                //4 Clock Cycles
                HL.lo = resetBit(0, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_1_adrHL:
                //8 Clock Cycles
                byte = resetBit(0, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_1_A:
                //4 Clock Cycles
                AF.hi = resetBit(0, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_2_B:
                //4 Clock Cycles
                BC.hi = resetBit(1, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_2_C:
                //4 Clock Cycles
                BC.lo = resetBit(1, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_2_D:
                //4 Clock Cycles
                DE.hi = resetBit(1, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_2_E:
                //4 Clock Cycles
                DE.lo = resetBit(1, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_2_H:
                //4 Clock Cycles
                HL.hi = resetBit(1, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_2_L:
                //4 Clock Cycles
                HL.lo = resetBit(1, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_2_adrHL:
                //8 Clock Cycles
                byte = resetBit(1, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_2_A:
                //4 Clock Cycles
                AF.hi = resetBit(1, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_3_B:
                //4 Clock Cycles
                BC.hi = resetBit(2, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_3_C:
                //4 Clock Cycles
                BC.lo = resetBit(2, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_3_D:
                //4 Clock Cycles
                DE.hi = resetBit(2, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_3_E:
                //4 Clock Cycles
                DE.lo = resetBit(2, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_3_H:
                //4 Clock Cycles
                HL.hi = resetBit(2, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_3_L:
                //4 Clock Cycles
                HL.lo = resetBit(2, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_3_adrHL:
                //8 Clock Cycles
                byte = resetBit(2, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_3_A:
                //4 Clock Cycles
                AF.hi = resetBit(2, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_4_B:
                //4 Clock Cycles
                BC.hi = resetBit(3, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_4_C:
                //4 Clock Cycles
                BC.lo = resetBit(3, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_4_D:
                //4 Clock Cycles
                DE.hi = resetBit(3, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_4_E:
                //4 Clock Cycles
                DE.lo = resetBit(3, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_4_H:
                //4 Clock Cycles
                HL.hi = resetBit(3, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_4_L:
                //4 Clock Cycles
                HL.lo = resetBit(3, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_4_adrHL:
                //8 Clock Cycles
                byte = resetBit(3, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_4_A:
                //4 Clock Cycles
                AF.hi = resetBit(3, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_5_B:
                //4 Clock Cycles
                BC.hi = resetBit(4, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_5_C:
                //4 Clock Cycles
                BC.lo = resetBit(4, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_5_D:
                //4 Clock Cycles
                DE.hi = resetBit(4, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_5_E:
                //4 Clock Cycles
                DE.lo = resetBit(4, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_5_H:
                //4 Clock Cycles
                HL.hi = resetBit(4, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_5_L:
                //4 Clock Cycles
                HL.lo = resetBit(4, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_5_adrHL:
                //8 Clock Cycles
                byte = resetBit(4, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_5_A:
                //4 Clock Cycles
                AF.hi = resetBit(4, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_6_B:
                //4 Clock Cycles
                BC.hi = resetBit(5, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_6_C:
                //4 Clock Cycles
                BC.lo = resetBit(5, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_6_D:
                //4 Clock Cycles
                DE.hi = resetBit(5, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_6_E:
                //4 Clock Cycles
                DE.lo = resetBit(5, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_6_H:
                //4 Clock Cycles
                HL.hi = resetBit(5, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_6_L:
                //4 Clock Cycles
                HL.lo = resetBit(5, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_6_adrHL:
                //8 Clock Cycles
                byte = resetBit(5, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_6_A:
                //4 Clock Cycles
                AF.hi = resetBit(5, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_7_B:
                //4 Clock Cycles
                BC.hi = resetBit(6, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_7_C:
                //4 Clock Cycles
                BC.lo = resetBit(6, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_7_D:
                //4 Clock Cycles
                DE.hi = resetBit(6, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_7_E:
                //4 Clock Cycles
                DE.lo = resetBit(6, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_7_H:
                //4 Clock Cycles
                HL.hi = resetBit(6, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_7_L:
                //4 Clock Cycles
                HL.lo = resetBit(6, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_7_adrHL:
                //8 Clock Cycles
                byte = resetBit(6, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_7_A:
                //4 Clock Cycles
                AF.hi = resetBit(6, AF.hi);
                timerUpdateCounter += 4;
                break;

            case RES_8_B:
                //4 Clock Cycles
                BC.hi = resetBit(7, BC.hi);
                timerUpdateCounter += 4;
                break;

            case RES_8_C:
                //4 Clock Cycles
                BC.lo = resetBit(7, BC.lo);
                timerUpdateCounter += 4;
                break;

            case RES_8_D:
                //4 Clock Cycles
                DE.hi = resetBit(7, DE.hi);
                timerUpdateCounter += 4;
                break;

            case RES_8_E:
                //4 Clock Cycles
                DE.lo = resetBit(7, DE.lo);
                timerUpdateCounter += 4;
                break;

            case RES_8_H:
                //4 Clock Cycles
                HL.hi= resetBit(7, HL.hi);
                timerUpdateCounter += 4;
                break;

            case RES_8_L:
                //4 Clock Cycles
                HL.lo = resetBit(7, HL.lo);
                timerUpdateCounter += 4;
                break;

            case RES_8_adrHL:
                //8 Clock Cycles
                byte = resetBit(7, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case RES_8_A:
                //4 Clock Cycles
                AF.hi = resetBit(7, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_1_B:
                //4 Clock Cycles
                BC.hi = setBit(0, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_1_C:
                //4 Clock Cycles
                BC.lo = setBit(0, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_1_D:
                //4 Clock Cycles
                DE.hi = setBit(0, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_1_E:
                //4 Clock Cycles
                DE.lo = setBit(0, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_1_H:
                //4 Clock Cycles
                HL.hi = setBit(0, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_1_L:
                //4 Clock Cycles
                HL.lo = setBit(0, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_1_adrHL:
                //8 Clock Cycles
                byte = setBit(0, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_1_A:
                //4 Clock Cycles
                AF.hi = setBit(0, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_2_B:
                //4 Clock Cycles
                BC.hi = setBit(1, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_2_C:
                //4 Clock Cycles
                BC.lo = setBit(1, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_2_D:
                //4 Clock Cycles
                DE.hi = setBit(1, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_2_E:
                //4 Clock Cycles
                DE.lo = setBit(1, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_2_H:
                //4 Clock Cycles
                HL.hi = setBit(1, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_2_L:
                //4 Clock Cycles
                HL.lo = setBit(1, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_2_adrHL:
                //8 Clock Cycles
                byte = setBit(1, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_2_A:
                //4 Clock Cycles
                AF.hi = setBit(1, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_3_B:
                //4 Clock Cycles
                BC.hi = setBit(2, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_3_C:
                //4 Clock Cycles
                BC.lo = setBit(2, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_3_D:
                //4 Clock Cycles
                DE.hi = setBit(2, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_3_E:
                //4 Clock Cycles
                DE.lo = setBit(2, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_3_H:
                //4 Clock Cycles
                HL.hi = setBit(2, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_3_L:
                //4 Clock Cycles
                HL.lo = setBit(2, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_3_adrHL:
                //8 Clock Cycles
                byte = setBit(2, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_3_A:
                //4 Clock Cycles
                AF.hi = setBit(2, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_4_B:
                //4 Clock Cycles
                BC.hi = setBit(3, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_4_C:
                //4 Clock Cycles
                BC.lo = setBit(3, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_4_D:
                //4 Clock Cycles
                DE.hi = setBit(3, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_4_E:
                //4 Clock Cycles
                DE.lo = setBit(3, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_4_H:
                //4 Clock Cycles
                HL.hi = setBit(3, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_4_L:
                //4 Clock Cycles
                HL.lo =setBit(3, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_4_adrHL:
                //8 Clock Cycles
                byte = setBit(3, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_4_A:
                //4 Clock Cycles
                AF.hi = setBit(3, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_5_B:
                //4 Clock Cycles
                BC.hi = setBit(4, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_5_C:
                //4 Clock Cycles
                BC.lo = setBit(4, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_5_D:
                //4 Clock Cycles
                DE.hi = setBit(4, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_5_E:
                //4 Clock Cycles
                DE.lo = setBit(4, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_5_H:
                //4 Clock Cycles
                HL.hi = setBit(4, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_5_L:
                //4 Clock Cycles
                HL.lo = setBit(4, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_5_adrHL:
                //8 Clock Cycles
                byte = setBit(4, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_5_A:
                //4 Clock Cycles
                AF.hi = setBit(4, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_6_B:
                //4 Clock Cycles
                BC.hi = setBit(5, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_6_C:
                //4 Clock Cycles
                BC.lo = setBit(5, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_6_D:
                //4 Clock Cycles
                DE.hi = setBit(5, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_6_E:
                //4 Clock Cycles
                DE.lo = setBit(5, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_6_H:
                //4 Clock Cycles
                HL.hi = setBit(5, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_6_L:
                //4 Clock Cycles
                HL.lo = setBit(5, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_6_adrHL:
                //8 Clock Cycles
                byte = setBit(5, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_6_A:
                //4 Clock Cycles
                 AF.hi = setBit(5, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_7_B:
                //4 Clock Cycles
                BC.hi = setBit(6, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_7_C:
                //4 Clock Cycles
                BC.lo = setBit(6, BC.lo);
                timerUpdateCounter += 4;
                break;

            case SET_7_D:
                //4 Clock Cycles
                DE.hi = setBit(6, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_7_E:
                //4 Clock Cycles
                DE.lo = setBit(6, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_7_H:
                //4 Clock Cycles
                HL.hi = setBit(6, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_7_L:
                //4 Clock Cycles
                HL.lo = setBit(6, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_7_adrHL:
                //8 Clock Cycles
                byte = setBit(6, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_7_A:
                //4 Clock Cycles
                AF.hi = setBit(6, AF.hi);
                timerUpdateCounter += 4;
                break;

            case SET_8_B:
                //4 Clock Cycles
                BC.hi = setBit(7, BC.hi);
                timerUpdateCounter += 4;
                break;

            case SET_8_C:
                //4 Clock Cycles
                BC.lo = setBit(7, BC.lo);
                timerUpdateCounter += 4;
                break;
                
            case SET_8_D:
                //4 Clock Cycles
                DE.hi = setBit(7, DE.hi);
                timerUpdateCounter += 4;
                break;

            case SET_8_E:
                //4 Clock Cycles
                DE.lo = setBit(7, DE.lo);
                timerUpdateCounter += 4;
                break;

            case SET_8_H:
                //4 Clock Cycles
                HL.hi = setBit(7, HL.hi);
                timerUpdateCounter += 4;
                break;

            case SET_8_L:
                //4 Clock Cycles
                HL.lo = setBit(7, HL.lo);
                timerUpdateCounter += 4;
                break;

            case SET_8_adrHL:
                //8 Clock Cycles
                byte = setBit(7, memoryUnit->Read(HL.data));
                memoryUnit->Write(HL.data, byte);
                timerUpdateCounter += 8;
                break;

            case SET_8_A:
                //4 Clock Cycles
                AF.hi = setBit(7, AF.hi);
                timerUpdateCounter += 4;
                break;

            default:
                break;
            }
            break;

        case CALL_ZERO:
            //24/12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(Z_FLAG))
            {
                reg temp2;
                temp2.data = PC;
                memoryUnit->Write(--SP, temp2.hi);
                memoryUnit->Write(--SP, temp2.lo);
                PC = temp->data;
                timerUpdateCounter += 24;
            }
            else
                timerUpdateCounter += 12;
            break;

        case CALL:
            //24 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            {
                reg temp2;
                temp2.data = PC;
                memoryUnit->Write(--SP, temp2.hi);
                memoryUnit->Write(--SP, temp2.lo);
                PC = temp->data;
            }
            timerUpdateCounter += 24;
            break;

        case ADC_8IMM_A:
            //8 Clock Cycles
            AF.hi = add8BitRegister(AF.hi, memoryUnit->Read(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 8;
            break;

        case RST_8:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0008;
            timerUpdateCounter += 16;
            break;

        case RET_NOCARRY:
            //20/8 Clock Cycles
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {
                temp->lo = memoryUnit->Read(SP++);
                temp->hi = memoryUnit->Read(SP++);
                PC = temp->data;
                timerUpdateCounter += 20;
            }
            else
                timerUpdateCounter += 8;
            break;

        case POP_DE:
            //12 Clock Cycles
            DE.lo = memoryUnit->Read(SP++);
            DE.hi = memoryUnit->Read(SP++);
            timerUpdateCounter += 12;
            break;

        case JMP_NOCARRY:
            //16/12 Clock cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {    
                PC = temp->data;
                timerUpdateCounter += 16;
            }
            else
                timerUpdateCounter += 12;
            break;

        case CALL_NOCARRY:
            //24/12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (!CPU_FLAG_BIT_TEST(C_FLAG))
            {    
                reg temp2;
                temp2.data = PC;
                memoryUnit->Write(--SP, temp2.hi);
                memoryUnit->Write(--SP, temp2.lo);
                PC = temp->data;
                timerUpdateCounter += 24;
            }
            else
                timerUpdateCounter += 12;
            break;

        case PUSH_DE:
            //16 clock cycles
            memoryUnit->Write(--SP, DE.hi);
            memoryUnit->Write(--SP, DE.lo);
            timerUpdateCounter += 16;
            break;

        case SUB_8IMM_A:
            //8 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;

        case RST_10:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0010;
            timerUpdateCounter += 16;
            break;

        case RET_CARRY:
            //20/8 Clock Cycles
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                temp->lo = memoryUnit->Read(SP++);
                temp->hi = memoryUnit->Read(SP++);
                PC = temp->data;
                
                timerUpdateCounter += 20;
            }else
                timerUpdateCounter += 8;
            break;

        case RET_INT:
            //16 Clock Cycles
            temp->lo = memoryUnit->Read(SP++);
            temp->hi = memoryUnit->Read(SP++);
            PC = temp->data;
            IME = true;
            timerUpdateCounter += 16;
            break;

        case JMP_CARRY:
            //16/12 Clock cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                PC = temp->data;
                timerUpdateCounter += 16;
            }
            else
                timerUpdateCounter += 12;
            break;

        case CALL_CARRY:
            //24/12 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            if (CPU_FLAG_BIT_TEST(C_FLAG))
            {
                reg temp2;
                temp2.data = PC;
                memoryUnit->Write(--SP, temp2.hi);
                memoryUnit->Write(--SP, temp2.lo);
                PC = temp->data;
                timerUpdateCounter += 24;
            }
            else
                timerUpdateCounter += 12;
            break;

        case SBC_8IMM_A:
            //8 Clock Cycles
            AF.hi = sub8BitRegister(AF.hi, memoryUnit->Read(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
            timerUpdateCounter += 8;
            break;

        case RST_18:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0018;
            timerUpdateCounter += 16;
            break;

        case LDH_A_IMMadr:
            //12 Clock Cycles
            memoryUnit->Write((0xFF00 + memoryUnit->Read(PC++)), AF.hi);
            timerUpdateCounter += 12;
            break;

        case POP_HL:
            //12 Clock Cycles
            HL.lo = memoryUnit->Read(SP++);
            HL.hi = memoryUnit->Read(SP++);
            timerUpdateCounter += 12;
            break;

        case LDH_A_C:
            //8 Clock Cycles
            memoryUnit->Write((0xFF00 + BC.lo), AF.hi);
            timerUpdateCounter += 8;
            break;

        case PUSH_HL:
            //16 clock cycles
            memoryUnit->Write(--SP, HL.hi);
            memoryUnit->Write(--SP, HL.lo);
            timerUpdateCounter += 16;
            break;

        case AND_8IMM_A:
            //8 Clock Cycles
            AF.hi = and8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;

        case RST_20:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0020;
            timerUpdateCounter += 16;
            break;

        case ADD_SIMM_SP:
            //16 Clock Cycles
            byte = memoryUnit->Read(PC++);
            if (testBitInByte(byte, 7))
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
            timerUpdateCounter += 16;
            break;

        case JMP_adrHL:
            //4 Clock Cycles
            PC = HL.data;
            timerUpdateCounter += 4;
            break;

        case LD_A_adr:
            //16 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            memoryUnit->Write(temp->data, AF.hi);
            timerUpdateCounter += 16;
            break;

        case XOR_8IMM_A:
            //8 Clock Cycles
            AF.hi = xor8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;
            
        case RST_28:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0028;
            
            timerUpdateCounter += 16;
            break;

        case LDH_IMMadr_A:
            //12 Clock Cycles
            AF.hi = memoryUnit->Read(0xFF00 + memoryUnit->Read(PC++));
            timerUpdateCounter += 12;
            break;

        case POP_AF:
            //12 Clock Cycles
            AF.lo = 0xF0 & memoryUnit->Read(SP++);
            AF.hi = memoryUnit->Read(SP++);
            timerUpdateCounter += 12;
            break;

        case DISABLE_INT:
            //4 Clock Cycles
            IME = false;
            timerUpdateCounter += 4;
            break;

        case PUSH_AF:
            //16 clock cycles
            memoryUnit->Write(--SP, AF.hi);
            memoryUnit->Write(--SP, AF.lo);
            timerUpdateCounter += 16;
            break;

        case OR_8IMM_A:
            //8 Clock Cycles
            AF.hi = or8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;

        case RST_30:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0030;
            timerUpdateCounter += 16;
            break;

        case LDHL_S_8IMM_SP_HL:
            //12 Clock Cycles
            uBYTE TempByte;
            TempByte = memoryUnit->Read(PC++);
            if (testBitInByte(TempByte,7))
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
            timerUpdateCounter += 12;
            break;

        case LD_HL_SP:
            //8 Clock Cycles
            SP = HL.data;
            timerUpdateCounter += 8;
            break;

        case LD_16adr_A:
            //16 Clock Cycles
            temp->lo = memoryUnit->Read(PC++);
            temp->hi = memoryUnit->Read(PC++);
            AF.hi = memoryUnit->Read(temp->data);
            timerUpdateCounter += 16;
            break;

        case ENABLE_INT:
            //4 Clock Cycles
            IME = true;
            timerUpdateCounter += 4;
            break;

        case CMP_8IMM_A:
            //8 Clock Cycles
            cmp8BitRegister(AF.hi, memoryUnit->Read(PC++));
            timerUpdateCounter += 8;
            break;

        case RST_38:
            //16 Clock Cycles
            temp->data = PC;
            memoryUnit->Write(--SP, temp->hi);
            memoryUnit->Write(--SP, temp->lo);
            PC = 0x0038;
            timerUpdateCounter += 16;
            break;

        default:
            break;
        }
        delete temp;

#ifdef FUUGB_DEBUG
        if (memoryUnit->DmaRead(0xFF02) == 0x81)
        {
            printf("%c", memoryUnit->DmaRead(0xFF01));
            memoryUnit->DmaWrite(0xFF02, 0x00);
        }
#endif
        return timerUpdateCounter;
    }

    uWORD CPU::increment16BitRegister(uWORD reg)
    {
        reg++;
        return reg;
    }

    uBYTE CPU::increment8BitRegister(uBYTE reg)
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

        return reg;
    }

    uBYTE CPU::decrement8BitRegister(uBYTE reg)
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

        return reg;
    }

    uWORD CPU::decrement16BitRegister(uWORD reg)
    {
        reg--;
        return reg;
    }

    uWORD CPU::add16BitRegister(uWORD host, uWORD operand)
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

        return host;
    }

    bool CPU::testBitInByte(uBYTE byte, int pos)
    {
        std::bitset<8> BitField(byte);
        return BitField.test(pos);
    }

    bool CPU::testBitInWord(uWORD word, int pos)
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

    uBYTE CPU::add8BitRegister(uBYTE host, uBYTE operand)
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

        return host;
    }

    uBYTE CPU::add8BitRegister(uBYTE host, uBYTE operand, bool carry)
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
        
        return host;
    }

    uBYTE CPU::sub8BitRegister(uBYTE host, uBYTE operand)
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

        return host;
    }

    uBYTE CPU::sub8BitRegister(uBYTE host, uBYTE operand, bool carry)
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

        return host;
    }

    uBYTE CPU::and8BitRegister(uBYTE host, uBYTE operand)
    {
        host = host & operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_SET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);

        return host;
    }

    uBYTE CPU::xor8BitRegister(uBYTE host, uBYTE operand)
    {
        host = host ^ operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);

        return host;
    }

    uBYTE CPU::or8BitRegister(uBYTE host, uBYTE operand)
    {
        host = host | operand;

        if (host == 0x00)
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_RESET(H_FLAG);
        CPU_FLAG_BIT_RESET(C_FLAG);

        return host;
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

    uBYTE CPU::rotateReg(bool direction, bool withCarry, uBYTE reg)
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

        return reg;
    }

    uBYTE CPU::shiftReg(bool direction, bool keepMSB, uBYTE reg)
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

        return reg;
    }

    uBYTE CPU::swapReg(uBYTE reg)
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

        return reg;
    }

    void CPU::flagSet(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        FlagBits.set(flag);
        AF.lo = FlagBits.to_ulong();
    }

    void CPU::flagReset(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        FlagBits.reset(flag);
        AF.lo = FlagBits.to_ulong();
    }

    bool CPU::flagTest(int flag)
    {
        std::bitset<8> FlagBits(AF.lo);
        return FlagBits.test(flag);
    }

    void CPU::testBit(int pos, uBYTE reg)
    {
        std::bitset<8> BitField(reg);
        if (!BitField.test(pos))
            CPU_FLAG_BIT_SET(Z_FLAG);
        else
            CPU_FLAG_BIT_RESET(Z_FLAG);

        CPU_FLAG_BIT_RESET(N_FLAG);
        CPU_FLAG_BIT_SET(H_FLAG);
    }

    uBYTE CPU::resetBit(int pos, uBYTE reg)
    {
        std::bitset<8> BitField(reg);
        BitField.reset(pos);

        reg = BitField.to_ulong();

        return reg;
    }

    uBYTE CPU::setBit(int pos, uBYTE reg)
    {
        std::bitset<8> BitField(reg);
        BitField.set(pos);

        reg = BitField.to_ulong();
        return reg;
    }

    void CPU::CheckInterupts()
    {
        if (!IME)
            return;

        std::bitset<8> IE(memoryUnit->Read(INTERUPT_EN_REGISTER_ADR));
        std::bitset<8> IF(memoryUnit->Read(INTERUPT_FLAG_REG));
        reg Temp;

        if (IF[0] && IE[0]) //V-Blank
        {
            IME = false;
            IF.reset(0);
            memoryUnit->Write(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            memoryUnit->DmaWrite(--SP, Temp.hi);
            memoryUnit->DmaWrite(--SP, Temp.lo);
            PC = VBLANK_INT;
        }
        else if (IF[1] && IE[1]) // LCDC
        {
            IME = false;
            IF.reset(1);
            memoryUnit->Write(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            memoryUnit->DmaWrite(--SP, Temp.hi);
            memoryUnit->DmaWrite(--SP, Temp.lo);
            PC = LCDC_INT;
        }
        else if (IF[2] && IE[2]) // Timer Overflow
        {
            IME = false;
            IF.reset(2);
            memoryUnit->Write(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            memoryUnit->DmaWrite(--SP, Temp.hi);
            memoryUnit->DmaWrite(--SP, Temp.lo);
            PC = TIMER_OVER_INT;
        }
        else if (IF[3] && IE[3]) // Serial I/O Complete
        {
            IME = false;
            IF.reset(3);
            memoryUnit->Write(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            memoryUnit->DmaWrite(--SP, Temp.hi);
            memoryUnit->DmaWrite(--SP, Temp.lo);
            PC = SER_TRF_INT;
        }
        else if (IF[4] && IE[4]) //Pin 10 - 13 hi to lo (Control Input)
        {
            IME = false;
            IF.reset(4);
            memoryUnit->Write(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
            Temp.data = PC;
            memoryUnit->DmaWrite(--SP, Temp.hi);
            memoryUnit->DmaWrite(--SP, Temp.lo);
            PC = CONTROL_INT;
        }
    }

    void CPU::UpdateTimers(int cycles)
    {
        std::bitset<8> TMC(memoryUnit->Read(0xFF07));
        std::bitset<8> TMA(memoryUnit->Read(0xFF06));
        std::bitset<8> TIMA(memoryUnit->Read(0xFF05));
        
        updateDivider(cycles);
        
        if(TMC.test(2)) // Check if clock is enabled
        {
            memoryUnit->TimerCounter -= cycles;
            
            if(memoryUnit->TimerCounter <= 0)
            {
                uBYTE frequency = memoryUnit->Read(0xFF07) & 0x03;
                switch(frequency)
                {
                    case 0: memoryUnit->TimerCounter = 1024; break;
                    case 1: memoryUnit->TimerCounter = 16; break;
                    case 2: memoryUnit->TimerCounter = 64; break;
                    case 3: memoryUnit->TimerCounter = 256; break;
                }
                
                // Timer Overflow
                if(memoryUnit->Read(0xFF05) == 255)
                {
                    memoryUnit->Write(0xFF05, memoryUnit->Read(0xFF06));
                    memoryUnit->RequestInterupt(2);
                }
                else
                {
                    memoryUnit->Write(0xFF05, memoryUnit->Read(0xFF05)+1);
                }
            }
        }
        
    }

    void CPU::updateDivider(int cycles)
    {
        dividerRegisterCounter += cycles;
        if(dividerRegisterCounter >= 255)
        {
            memoryUnit->DmaWrite(0xFF04, memoryUnit->DmaRead(0xFF04)+1);
            dividerRegisterCounter = 0;
        }
    }

    uBYTE CPU::adjustDAA(uBYTE reg)
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

        return reg;
    }
    
    void CPU::Halt()
    {
        std::bitset<8> IE(memoryUnit->DmaRead(INTERUPT_EN_REGISTER_ADR));
        std::bitset<8> IF(memoryUnit->DmaRead(INTERUPT_FLAG_REG));
        if (IF[0] && IE[0]) //V-Blank
        {
            Halted = false;
        }
        else if (IF[1] && IE[1]) // LCDC
        {
            Halted = false;
        }
        else if (IF[2] && IE[2]) // Timer Overflow
        {
            Halted = false;
        }
        else if (IF[3] && IE[3]) // Serial I/O Complete
        {
            Halted = false;
        }
        else if (IF[4] && IE[4]) //Pin 10 - 13 hi to lo (Control Input)
        {
            Halted = false;
        }
    }
}
