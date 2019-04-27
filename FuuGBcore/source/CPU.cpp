//
//  CPU.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#include "Fuupch.h"
#include "CPU.h"

namespace FuuGB
{
	CPU::CPU(Memory* mem)
	{
		this->PC = 0x0000;
		this->SP = 0x0000;
		this->AF = 0x0000;
		this->BC = 0x0000;
		this->DE = 0x0000;
		this->HL = 0x0000;

		memory = mem;

		_cpuRunning = true;
		_cpuTHR = new std::thread(&CPU::clock, this);
		FlagBits = new std::bitset<sizeof(uBYTE)*8>(AF.lo);
		AluBits = new std::bitset<sizeof(uBYTE)*8>(AF.hi);
		
		FUUGB_CPU_LOG("CPU Initialized.");
	}

	CPU::~CPU()
	{
		_cpuTHR->join();
		delete _cpuTHR;
		FUUGB_CPU_LOG("CPU Destroyed.");
	}

	void CPU::clock()
	{
		while (_cpuRunning)
		{
			executeNextOpCode();
		}
	}

	void CPU::executeNextOpCode()
	{
		bool oldCarry = true;
		uBYTE byte = memory->readMemory(PC++);
		uBYTE SP_data = 0x0;
		Register* temp = new Register();
		printf("[CPU]: Executing next OpCode: %x\n", byte);
		switch (byte)
		{
		case NOP:
			//4 CPU Cycle
			break;

		case LD_16IMM_BC:
			//12 CPU Cycles
			BC.hi = memory->readMemory(PC++);
			BC.lo = memory->readMemory(PC++);
			break;

		case LD_A_adrBC:
			//8 CPU Cycles
			memory->writeMemory(BC.data, AF.hi);
			break;

		case INC_BC:
			//8 CPU Cycles
			increment16BitRegister(BC.data);
			break;

		case INC_B:
			//4 CPU Cycles
			increment8BitRegister(BC.hi);
			break;

		case DEC_B:
			//4 CPU Cycles
			decrement8BitRegister(BC.hi);
			break;

		case LD_8IMM_B:
			//8 CPU Cycles
			BC.hi = memory->readMemory(PC++);
			break;

		case RLC_A:
			//4 CPU Cycles
			if (TestBitInByte(AF.hi, 7))
				CPU_FLAG_BIT_SET(C_FLAG);
			AF.hi = AF.hi << 0x1;
			
			if (AF.hi == 0x00)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			CPU_FLAG_BIT_RESET(H_FLAG);
			break;

		case LD_SP_adr:
			//20 CPU cycles
			Register LD_addr;
			LD_addr.hi = memory->readMemory(PC++);
			LD_addr.lo = memory->readMemory(PC++);
			SP_data = memory->readMemory(SP);
			memory->writeMemory(LD_addr.data, SP_data);
			break;

		case ADD_BC_HL:
			//8 CPU Cycles
			add16BitRegister(HL.data, BC.data);
			break;

		case LD_adrBC_A:
			//8 CPU Cycles
			AF.hi = memory->readMemory(BC.data);
			break;

		case DEC_BC:
			//8 CPU Cycles
			decrement16BitRegister(BC.data);
			break;

		case INC_C:
			//4 CPU Cycles
			increment8BitRegister(BC.lo);
			break;

		case DEC_C:
			//4 CPU Cycles
			decrement8BitRegister(BC.lo);
			break;

		case LD_8IMM_C:
			//8 CPU Cycles
			BC.lo = memory->readMemory(PC++);
			break;

		case RRC_A:
			//4 CPU Cycles
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(0));
			AF.hi = AF.hi >> 1;
			if (AF.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);
			break;

		case STOP:
			/*
			
			----------------------- TO-DO ---------------------------
			
			*/
			break;

		case LD_16IMM_DE:
			//12 Clock Cycles
			DE.hi = memory->readMemory(PC++);
			DE.lo = memory->readMemory(PC++);
			break;

		case LD_A_adrDE:
			//8 Clock Cycles
			memory->writeMemory(DE.data, AF.hi);
			break;

		case INC_DE:
			//8 Clock Cycles
			increment16BitRegister(DE.data);
			break;

		case INC_D:
			//4 Clock Cycles
			increment8BitRegister(DE.hi);
			break;

		case DEC_D:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			break;

		case LD_8IMM_D:
			//8 Clock Cycles
			DE.hi = memory->readMemory(PC++);
			break;

		case RL_A:
			//4 Clock Cycles
			oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(7));
			AF.hi = AF.hi << 1;
			ALU_BIT_SET(0, oldCarry);
			break;

		case RJmp_IMM:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (TestBitInByte(byte, 7))
				PC = PC - (byte & 0x7F);
			else
				PC = PC + (byte & 0x7F);
			break;

		case ADD_DE_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, DE.data);
			break;

		case LD_adrDE_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(DE.data);
			break;

		case DEC_DE:
			//4 Clock Cycles
			decrement16BitRegister(DE.data);
			break;

		case INC_E:
			//4 Clock Cycles
			increment8BitRegister(DE.lo);
			break;

		case DEC_E:
			//4 clock cycles
			decrement8BitRegister(DE.lo);
			break;

		case LD_8IMM_E:
			//8 Clock Cycles
			DE.lo = memory->readMemory(PC++);
			break;

		case RR_A:
			//4 clock cycles
			oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(0));
			AF.hi = AF.hi >> 1;
			ALU_BIT_SET(7, oldCarry);
			break;

		case RJmp_NOTZERO:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				if (TestBitInByte(byte, 7))
					PC = PC - twoComp_Byte(byte);
				else
					PC = PC + byte;
			}
			break;

		case LD_16IMM_HL:
			//12 Clock Cycles
			HL.hi = memory->readMemory(PC++);
			HL.lo = memory->readMemory(PC++);
			break;

		case LDI_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data++, AF.hi);
			break;

		case INC_HL:
			//4 Clock Cycles
			increment16BitRegister(HL.data);
			break;

		case INC_H:
			//4 Clock Cycles
			increment8BitRegister(HL.hi);
			break;

		case DEC_H:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			break;

		case LD_8IMM_H:
			//8 Clock Cycles
			HL.hi = memory->readMemory(PC++);
			break;

		case DAA:
			//4 Clock Cycles
			/*
			
			---------------------------------- TO-DO --------------------------------
			
			*/
			break;

		case RJmp_ZERO:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				if (TestBitInByte(byte, 7))
					PC = PC - twoComp_Byte(byte);
				else
					PC = PC + byte;
			}
			break;

		case ADD_HL_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, HL.data);
			break;

		case LDI_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data++);
			break;

		case DEC_HL:
			//4 Clock Cycles
			decrement16BitRegister(HL.data);
			break;

		case INC_L:
			//4 Clock Cycles
			increment8BitRegister(HL.lo);
			break;

		case DEC_L:
			//4 Clock Cycles
			decrement8BitRegister(HL.lo);
			break;

		case LD_8IMM_L:
			//8 Clock Cycles
			HL.lo = memory->readMemory(PC++);
			break;

		case NOT_A:
			//4 Clock Cycles
			AF.hi = ~AF.hi;
			break;

		case RJmp_NOCARRY:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(C_FLAG))
			{
				if (TestBitInByte(byte, 7))
					PC = PC - twoComp_Byte(byte);
				else
					PC = PC + byte;
			}
			break;

		case LD_16IM_SP:
			//12 Clock Cycles
			temp->hi = memory->readMemory(PC++);
			temp->lo = memory->readMemory(PC++);
			SP = temp->data;
			break;

		case LDD_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data--, AF.hi);
			break;

		case INC_SP:
			//8 Clock Cycles
			increment16BitRegister(SP);
			break;

		case INC_valHL:
			//12 Clock Cycles
			byte = memory->readMemory(HL.data);
			++byte;
			if (byte == 0x00)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);

			if (checkCarryFromBit_Byte(2, byte, 0x01))
				CPU_FLAG_BIT_SET(H_FLAG);

			memory->writeMemory(HL.data, byte);
			break;
			
		case DEC_valHL:
			//12 Clock Cycles
			byte = memory->readMemory(HL.data);
			--byte;
			if (byte == 0x00)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);

			if (!checkBorrowFromBit_Byte(3, byte, 0x01))
				CPU_FLAG_BIT_SET(H_FLAG);

			memory->writeMemory(HL.data, byte);
			break;

		case LD_8IMM_adrHL:
			//12 Clock Cycles
			byte = memory->readMemory(PC++);
			memory->writeMemory(HL.data, byte);
			break;

		case SET_CARRY_FLAG:
			//4 Clock Cycles
			CPU_FLAG_BIT_RESET(N_FLAG);
			CPU_FLAG_BIT_RESET(H_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG);
			break;

		case RJmp_CARRY:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(C_FLAG))
			{
				if (TestBitInByte(byte, 7))
					PC = PC - twoComp_Byte(byte);
				else
					PC = PC + byte;
			}
			break;

		case ADD_SP_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, SP);
			break;

		case LDD_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data--);
			break;

		case DEC_SP:
			//8 Clock Cycles;
			decrement16BitRegister(SP);
			break;

		case INC_A:
			//4 Clock Cycles
			increment8BitRegister(AF.hi);
			break;

		case DEC_A:
			//4 Clock Cycles
			decrement8BitRegister(AF.hi);
			break;

		case LD_8IMM_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(PC++);
			break;

		case COMP_CARRY_FLAG:
			//4 Clock Cycles
			if (CPU_FLAG_BIT_TEST(C_FLAG))
				CPU_FLAG_BIT_RESET(C_FLAG);
			else
				CPU_FLAG_BIT_SET(C_FLAG);
			
			CPU_FLAG_BIT_RESET(N_FLAG);
			CPU_FLAG_BIT_RESET(H_FLAG);
			break;

		case LD_B_B:
			//4 Clock Cycles
			BC.hi = BC.hi;
			break;

		case LD_C_B:
			//4 Clock Cycles
			BC.hi = BC.lo;
			break;

		case LD_D_B:
			//4 Clock Cycles
			BC.hi = DE.hi;
			break;

		case LD_E_B:
			//4 Clock Cycles
			BC.hi = DE.lo;
			break;

		case LD_H_B:
			//4 Clock Cycles
			BC.hi = HL.hi;
			break;

		case LD_L_B:
			//4 Clock Cycles
			BC.hi = HL.lo;
			break;

		case LD_adrHL_B:
			//8 Clock Cycles
			BC.hi = memory->readMemory(HL.data);
			break;

		case LD_A_B:
			//4 Clock Cycles
			BC.hi = AF.hi;
			break;

		case LD_B_C:
			//4 Clock Cycles
			BC.lo = BC.hi;
			break;

		case LD_C_C:
			//4 Clock Cycles
			BC.lo = BC.lo;
			break;

		case LD_D_C:
			//4 Clock Cycles
			BC.lo = DE.hi;
			break;

		case LD_E_C:
			//4 Clock Cycles
			BC.lo = DE.lo;
			break;

		case LD_H_C:
			//4 Clock Cycles
			BC.lo = HL.hi;
			break;

		case LD_L_C:
			//4 Clock Cycles
			BC.lo = HL.lo;
			break;

		case LD_adrHL_C:
			//8 Clock Cycles
			BC.lo = memory->readMemory(HL.data);
			break;

		case LD_A_C:
			//4 Clock Cycles
			BC.lo = AF.hi;
			break;

		case LD_B_D:
			//4 Clock Cycles
			DE.hi = BC.hi;
			break;

		case LD_C_D:
			//4 Clock Cycles
			DE.hi = BC.lo;
			break;

		case LD_D_D:
			//4 Clock Cycles
			DE.hi = DE.hi;
			break;

		case LD_E_D:
			//4 Clock Cycles
			DE.hi = DE.lo;
			break;

		case LD_H_D:
			//4 Clock Cycles
			DE.hi = HL.hi;
			break;

		case LD_L_D:
			//4 Clock Cycles
			DE.hi = HL.lo;
			break;

		case LD_adrHL_D:
			//8 Clock Cycles
			DE.hi = memory->readMemory(HL.data);
			break;

		case LD_A_D:
			//4 Clock Cycles
			DE.hi = AF.hi;
			break;

		case LD_B_E:
			//4 Clock Cycles
			DE.lo = BC.hi;
			break;

		case LD_C_E:
			//4 Clock Cycles
			DE.lo = BC.lo;
			break;

		case LD_D_E:
			//4 Clock Cycles
			DE.lo = DE.hi;
			break;

		case LD_E_E:
			//4 Clock Cycles
			DE.lo = DE.lo;
			break;

		case LD_H_E:
			//4 Clock Cycles
			DE.lo = HL.hi;
			break;

		case LD_L_E:
			//4 Clock Cycles
			DE.lo = HL.lo;
			break;

		case LD_adrHL_E:
			//8 Clock Cycles
			DE.lo = memory->readMemory(HL.data);
			break;

		case LD_A_E:
			//4 Clock Cycles
			DE.lo = AF.hi;
			break;

		case LD_B_H:
			//4 Clock Cycles
			HL.hi = BC.hi;
			break;

		case LD_C_H:
			//4 Clock Cycles
			HL.hi = BC.lo;
			break;

		case LD_D_H:
			//4 Clock Cycles
			HL.hi = DE.hi;
			break;

		case LD_E_H:
			//4 Clock Cycles
			HL.hi = DE.lo;
			break;

		case LD_H_H:
			//4 Clock Cycles
			HL.hi = HL.hi;
			break;

		case LD_L_H:
			//4 Clock Cycles
			HL.hi = HL.lo;
			break;

		case LD_adrHL_H:
			//8 Clock Cycles
			HL.hi = memory->readMemory(HL.data);
			break;

		case LD_A_H:
			//4 Clock Cycles
			HL.hi = AF.hi;
			break;

		case LD_B_L:
			//4 Clock Cycles
			HL.lo = BC.hi;
			break;

		case LD_C_L:
			//4 Clock Cycles
			HL.lo = BC.lo;
			break;

		case LD_D_L:
			//4 Clock Cycles
			HL.lo = DE.hi;
			break;

		case LD_E_L:
			//4 Clock Cycles
			HL.lo = DE.lo;
			break;

		case LD_H_L:
			//4 Clock Cycles
			HL.lo = HL.hi;
			break;

		case LD_L_L:
			//4 Clock Cycles
			HL.lo = HL.lo;
			break;

		case LD_adrHL_L:
			//8 Clock Cycles
			HL.lo = memory->readMemory(HL.data);
			break;

		case LD_A_L:
			//4 Clock Cycles
			HL.lo = AF.hi;
			break;

		case LD_B_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, BC.hi);
			break;

		case LD_C_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, BC.lo);
			break;

		case LD_D_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, DE.hi);
			break;
			
		case LD_E_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, DE.lo);
			break;

		case LD_H_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, HL.hi);
			break;

		case LD_L_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, HL.lo);
			break;

		case HALT:
			//4 Clock Cycles
			/*
			
			------------------------------ TO-DO ------------------------------
			Possible implementations:
				Put CPU thread to sleep on a Cond Var, next interupt signals that cond Var
			*/
			break;

		case LD_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, AF.hi);
			break;

		case LD_B_A:
			//4 Clock Cycles
			AF.hi = BC.hi;
			break;

		case LD_C_A:
			//4 Clock Cycles
			AF.hi = BC.lo;
			break;

		case LD_D_A:
			//4 Clock Cycles
			AF.hi = DE.hi;
			break;

		case LD_E_A:
			//4 Clock Cycles
			AF.hi = DE.lo;
			break;

		case LD_H_A:
			//4 Clock Cycles
			AF.hi = HL.hi;
			break;

		case LD_L_A:
			//4 Clock Cycles
			AF.hi = HL.lo;
			break;

		case LD_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data);
			break;

		case LD_A_A:
			//4 Clock Cycles
			AF.hi = AF.hi;
			break;

		case ADD_B_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.hi);
			break;

		case ADD_C_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.lo);
			break;

		case ADD_D_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.hi);
			break;

		case ADD_E_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.lo);
			break;

		case ADD_H_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.hi);

		case ADD_L_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.lo);
			break;

		case ADD_adrHL_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case ADD_A_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, AF.hi);
			break;

		case ADC_B_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_C_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_D_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_E_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_H_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_L_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_adrHL_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case ADC_A_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SUB_B_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.hi);
			break;

		case SUB_C_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.lo);
			break; 

		case SUB_D_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.hi);
			break;

		case SUB_E_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.lo);
			break;

		case SUB_H_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.hi);
			break;

		case SUB_L_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.lo);
			break;

		case SUB_adrHL_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case SUB_A_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, AF.hi);
			break;

		case SBC_B_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_C_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_D_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_E_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_H_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_L_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_adrHL_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case SBC_A_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			break;

		case AND_B_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, BC.hi);
			break;

		case AND_C_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, BC.lo);
			break;

		case AND_D_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, DE.hi);
			break;

		case AND_E_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, DE.lo);
			break;

		case AND_H_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, HL.hi);
			break;

		case AND_L_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, HL.lo);
			break;

		case AND_adrHL_A:
			//8 Clock Cycles
			and8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case AND_A_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, AF.hi);
			break;

		case XOR_B_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, BC.hi);
			break;

		case XOR_C_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, BC.lo);
			break;

		case XOR_D_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, DE.hi);
			break;

		case XOR_E_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, DE.lo);
			break;

		case XOR_H_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, HL.hi);
			break;

		case XOR_L_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, HL.lo);
			break;

		case XOR_adrHL_A:
			//8 Clock Cycles
			xor8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case XOR_A_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, AF.hi);
			break;

		case OR_B_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, BC.hi);
			break;

		case OR_C_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, BC.lo);
			break;

		case OR_D_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, DE.hi);
			break;

		case OR_E_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, DE.lo);
			break;

		case OR_H_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, HL.hi);
			break;

		case OR_L_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, HL.lo);
			break;

		case OR_adrHL_A:
			//8 Clock Cycles
			or8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case OR_A_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, AF.hi);
			break;

		case CMP_B_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, BC.hi);
			break;

		case CMP_C_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, BC.lo);
			break;

		case CMP_D_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, DE.hi);
			break;

		case CMP_E_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, DE.lo);
			break;

		case CMP_H_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, HL.hi);
			break;

		case CMP_L_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, HL.lo);
			break;

		case CMP_adrHL_A:
			//8 Clock Cycles
			cmp8BitRegister(AF.hi, memory->readMemory(HL.data));
			break;

		case CMP_A_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, AF.hi);
			break;

		case RET_NOT_ZERO:
		/*
		to do
		*/
			break;

		case POP_BC:
			//12 Clock Cycles
			BC.hi = memory->readMemory(SP++);
			BC.lo = memory->readMemory(SP++);
			break;

		case JMP_NOT_ZERO:
			//12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				PC = temp->data;
			}
			break;

		case JMP:
			//12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			PC = temp->data;
			break;

		case CALL_NOT_ZERO:
			//24 Clock Cycles (if cc= true)
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
			}
			break;

		case PUSH_BC:
			//12 clock cycles
			memory->writeMemory(--SP, BC.hi);
			memory->writeMemory(--SP, BC.lo);
			break;

		case ADD_IMM_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(PC++));
			break;

		case RST_0:
			//16 Clock Cycles
			temp->data = PC;
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
			memory->writeMemory(--SP, temp->hi);
			memory->writeMemory(--SP, temp->lo);
			PC = 0x0000;
			break;

		case RET_ZERO:
			//8 Clock Cycles if cc false else 20 clock cycles
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				PC = temp->data;
				std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
			}
			break;

		case RETURN:
			//16 Clock Cycles
			temp->lo = memory->readMemory(SP++);
			temp->hi = memory->readMemory(SP++);
			PC = temp->data;
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));

		case JMP_ZERO:
			//12 Clock cycles(false) or 16 clock cycles(true)
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
				PC = temp->data;
			}
			break;

		default:
			break;
		}
	}

	void CPU::stop()
	{
		this->_cpuRunning = false;
	}

	void CPU::increment16BitRegister(uWORD& reg)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		++reg;
	}

	void CPU::increment8BitRegister(uBYTE& reg)
	{
		++reg;
		if (reg == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Byte(2, reg, 0x01))
			CPU_FLAG_BIT_SET(H_FLAG);
	}

	void CPU::decrement8BitRegister(uBYTE& reg)
	{
		--reg;
		if (reg == 0x0)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);

		if (checkBorrowFromBit_Byte(3, reg, 0x01))
			CPU_FLAG_BIT_SET(H_FLAG);
	}

	void CPU::decrement16BitRegister(uWORD& reg)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		--reg;
	}

	void CPU::add16BitRegister(uWORD& host, uWORD operand)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Byte(10, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		if (checkCarryFromBit_Byte(14, host, operand))
			CPU_FLAG_BIT_SET(C_FLAG);
	}

	bool CPU::TestBitInByte(uBYTE byte, int pos)
	{
		std::bitset<sizeof(uBYTE)*8> BitField(byte);
		return BitField.test(pos);
	}

	bool CPU::TestBitInWord(uWORD word, int pos)
	{
		std::bitset<sizeof(uWORD)*8> BitField(word);
		return BitField.test(pos);
	}

	bool CPU::checkCarryFromBit_Byte(int pos, uBYTE byte, uBYTE addedByte)
	{
		std::bitset<sizeof(uBYTE) * 8> BitField(byte);
		std::bitset<sizeof(uBYTE) * 8> AddedBitField(addedByte);
		bool carry = false;
		std::bitset<sizeof(bool)> bitSum(carry);

		for (int i = 0; i < pos; ++i)
		{
			bitSum[0] = BitField[i] + AddedBitField[i] + carry;
			if (!bitSum[0])
				carry = true;
			else
				carry = false;
		}
		return carry;
	}

	bool CPU::checkBorrowFromBit_Byte(int pos, uBYTE byte, uBYTE subtractedByte)
	{
		std::bitset<sizeof(uBYTE) * 8> BitField(byte);
		std::bitset<sizeof(uBYTE) * 8> SubBitField(subtractedByte);
		bool borrow = false;

		for (int i = 0; i < pos; ++i)
		{
			if (!BitField[i] & SubBitField[i])
			{
				int j = i;
				while (!BitField[++j])
				{
					if (j == pos)
						return false;
				}
				BitField[j] = false;

				while (--j != i)
				{
					BitField[j] = true;
				}
				borrow = true;
			}
			else
				borrow = false;
		}

		return borrow;
	}

	uBYTE CPU::twoComp_Byte(uBYTE byte)
	{
		std::bitset<8> twoCompByte(byte);

		twoCompByte.flip();
		
		uBYTE result = twoCompByte.to_ulong() + 0x01;

		return result;
	}

	void CPU::add8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host + operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);
		if (checkCarryFromBit_Byte(2, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		if (checkCarryFromBit_Byte(6, host, operand))
			CPU_FLAG_BIT_SET(C_FLAG);
	}

	void CPU::add8BitRegister(uBYTE& host, uBYTE operand, bool carry)
	{
		host = host + operand + carry;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);
		if (checkCarryFromBit_Byte(2, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		if (checkCarryFromBit_Byte(6, host, operand))
			CPU_FLAG_BIT_SET(C_FLAG);
	}

	void CPU::sub8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host - operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);
		
		if (!checkBorrowFromBit_Byte(3, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);

		if (!checkBorrowFromBit_Byte(7, host, operand))
			CPU_FLAG_BIT_SET(C_FLAG);
	}

	void CPU::sub8BitRegister(uBYTE& host, uBYTE operand, bool carry)
	{
		host = host - operand - carry;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);

		if (!checkBorrowFromBit_Byte(3, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);

		if (!checkBorrowFromBit_Byte(7, host, operand))
			CPU_FLAG_BIT_SET(C_FLAG);
	}

	void CPU::and8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host & operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);
		CPU_FLAG_BIT_SET(H_FLAG);
		CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::xor8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host ^ operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);
		CPU_FLAG_BIT_RESET(H_FLAG);
		CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::or8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host | operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);
		CPU_FLAG_BIT_RESET(H_FLAG);
		CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::cmp8BitRegister(uBYTE host, uBYTE operand)
	{
		if (host == operand)
			CPU_FLAG_BIT_SET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);

		if (!checkBorrowFromBit_Byte(3, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);

		if (host < operand)
			CPU_FLAG_BIT_SET(C_FLAG);
	}
}
