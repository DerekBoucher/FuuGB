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
		FlagBits = new std::bitset<sizeof(uBYTE)*8>(&AF.lo);
		AluBits = new std::bitset<sizeof(uBYTE)*8>(&AF.hi);
		
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
			BC.lo = memory->readMemory(PC++);
			BC.hi = memory->readMemory(PC++);
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
			rotateReg(true, false, AF.hi);
			break;

		case LD_SP_adr:
			//20 CPU cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			SP_data = memory->readMemory(SP);
			memory->writeMemory(temp->data, SP_data);
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
			rotateReg(false, false, AF.hi);
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
			rotateReg(true, true, AF.hi);
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
			rotateReg(false, true, AF.hi);
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
			HL.lo = memory->readMemory(PC++);
			HL.hi = memory->readMemory(PC++);
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
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
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
			//16/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				CPU_SLEEP_FOR_MACHINE_CYCLE();
				PC = temp->data;
			}
			break;

		case JMP:
			//16 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			CPU_SLEEP_FOR_MACHINE_CYCLE();
			PC = temp->data;
			break;

		case CALL_NOT_ZERO:
			//24/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				CPU_SLEEP_FOR_MACHINE_CYCLE();
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
			}
			break;

		case PUSH_BC:
			//16 clock cycles
			memory->writeMemory(--SP, BC.hi);
			memory->writeMemory(--SP, BC.lo);
			CPU_SLEEP_FOR_MACHINE_CYCLE();
			break;

		case ADD_IMM_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(PC++));
			break;

		case RST_0:
			//16 Clock Cycles
			temp->data = PC;
			CPU_SLEEP_FOR_MACHINE_CYCLE();
			memory->writeMemory(--SP, temp->hi);
			memory->writeMemory(--SP, temp->lo);
			PC = 0x0000;
			break;

		case RET_ZERO:
			//8 Clock Cycles if cc false else 20 clock cycles
			CPU_SLEEP_FOR_MACHINE_CYCLE();
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				CPU_SLEEP_FOR_MACHINE_CYCLE();
				PC = temp->data;
			}
			break;

		case RETURN:
			//16 Clock Cycles
			temp->lo = memory->readMemory(SP++);
			temp->hi = memory->readMemory(SP++);
			CPU_SLEEP_FOR_MACHINE_CYCLE();
			PC = temp->data;

		case JMP_ZERO:
			//16/12 Clock cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				CPU_SLEEP_FOR_MACHINE_CYCLE();
				PC = temp->data;
			}
			break;

		case EXT_OP:
			//4 Clock Cycles, this opcode is special, it allows for 16 bit opcodes
			switch (memory->readMemory(PC++))
			{
			case RLC_B:
				//4 clock Cycles
				rotateReg(true, false, BC.hi);
				break;

			case RLC_C:
				//4 clock Cycles
				rotateReg(true, false, BC.lo);
				break;

			case RLC_D:
				//4 clock Cycles
				rotateReg(true, false, DE.hi);
				break;

			case RLC_E:
				//4 clock Cycles
				rotateReg(true, false, DE.lo);
				break;

			case RLC_H:
				//4 clock Cycles
				rotateReg(true, false, HL.hi);
				break;

			case RLC_L:
				//4 clock Cycles
				rotateReg(true, false, HL.lo);
				break;

			case RLC_adrHL:
				//8 clock Cycles
				rotateReg(true, false, memory->readMemory(HL.data));
				break;

			case eRLC_A:
				//4 clock Cycles
				rotateReg(true, false, AF.hi);
				break;
				
			case RRC_B:
				//4 clock Cycles
				rotateReg(false, false, BC.hi);
				break;

			case RRC_C:
				//4 clock Cycles
				rotateReg(false, false, BC.lo);
				break;

			case RRC_D:
				//4 clock Cycles
				rotateReg(false, false, DE.hi);
				break;

			case RRC_E:
				//4 clock Cycles
				rotateReg(false, false, DE.lo);
				break;

			case RRC_H:
				//4 clock Cycles
				rotateReg(false, false, HL.hi);
				break;

			case RRC_L:
				//4 clock Cycles
				rotateReg(false, false, HL.lo);
				break;

			case RRC_adrHL:
				//8 clock Cycles
				rotateReg(false, false, memory->readMemory(HL.data));
				break;

			case eRRC_A:
				//4 clock Cycles
				rotateReg(false, false, AF.hi);
				break;

			case RL_B:
				//4 clock Cycles
				rotateReg(true, true, BC.hi);
				break;

			case RL_C:
				//4 clock Cycles
				rotateReg(true, true, BC.lo);
				break;

			case RL_D:
				//4 clock Cycles
				rotateReg(true, true, DE.hi);
				break;

			case RL_E:
				//4 clock Cycles
				rotateReg(true, true, DE.lo);
				break;

			case RL_H:
				//4 clock Cycles
				rotateReg(true, true, HL.hi);
				break;

			case RL_L:
				//4 clock Cycles
				rotateReg(true, true, HL.lo);
				break;

			case RL_adrHL:
				//8 clock Cycles
				rotateReg(true, true, memory->readMemory(HL.data));
				break;

			case eRL_A:
				//4 clock Cycles
				rotateReg(true, true, AF.hi);
				break;

			case RR_B:
				//4 clock Cycles
				rotateReg(false, true, BC.hi);
				break;

			case RR_C:
				//4 clock Cycles
				rotateReg(false, true, BC.lo);
				break;

			case RR_D:
				//4 clock Cycles
				rotateReg(false, true, DE.hi);
				break;

			case RR_E:
				//4 clock Cycles
				rotateReg(false, true, DE.lo);
				break;

			case RR_H:
				//4 clock Cycles
				rotateReg(false, true, HL.hi);
				break;

			case RR_L:
				//4 clock Cycles
				rotateReg(false, true, HL.lo);
				break;

			case RR_adrHL:
				//8 clock Cycles
				rotateReg(false, true, memory->readMemory(HL.data));
				break;

			case eRR_A:
				//4 clock Cycles
				rotateReg(false, true, AF.hi);
				break;

			case SLA_B:
				//4 clock cycles
				shiftReg(true, false, BC.hi);
				break;

			case SLA_C:
				//4 clock cycles
				shiftReg(true, false, BC.lo);
				break;

			case SLA_D:
				//4 clock cycles
				shiftReg(true, false, DE.hi);
				break;

			case SLA_E:
				//4 clock cycles
				shiftReg(true, false, DE.lo);
				break;

			case SLA_H:
				//4 clock cycles
				shiftReg(true, false, HL.hi);
				break;

			case SLA_L:
				//4 clock cycles
				shiftReg(true, false, HL.lo);
				break;

			case SLA_adrHL:
				//8 Clock Cycles
				shiftReg(true, false, memory->readMemory(HL.data));
				break;

			case SLA_A:
				//8 Clock Cycles
				shiftReg(true, false, AF.hi);
				break;

			case SRA_B:
				//4 clock Cycles
				shiftReg(false, true, BC.hi);
				break;

			case SRA_C:
				//4 clock Cycles
				shiftReg(false, true, BC.lo);
				break;

			case SRA_D:
				//4 clock Cycles
				shiftReg(false, true, DE.hi);
				break;

			case SRA_E:
				//4 clock Cycles
				shiftReg(false, true, DE.lo);
				break;

			case SRA_H:
				//4 clock Cycles
				shiftReg(false, true, HL.hi);
				break;

			case SRA_L:
				//4 clock Cycles
				shiftReg(false, true, HL.lo);
				break;

			case SRA_adrHL:
				//8 clock Cycles
				shiftReg(false, true, memory->readMemory(HL.data));
				break;

			case SRA_A:
				//4 clock Cycles
				shiftReg(false, true, AF.hi);
				break;

			case SWAP_B:
				//4 Clock Cycles
				swapReg(BC.hi);
				break;

			case SWAP_C:
				//4 Clock Cycles
				swapReg(BC.lo);
				break;

			case SWAP_D:
				//4 Clock Cycles
				swapReg(DE.hi);
				break;

			case SWAP_E:
				//4 Clock Cycles
				swapReg(DE.lo);
				break;

			case SWAP_H:
				//4 Clock Cycles
				swapReg(HL.hi);
				break;

			case SWAP_L:
				//4 Clock Cycles
				swapReg(HL.lo);
				break;

			case SWAP_adrHL:
				//8 Clock Cycles
				swapReg(memory->readMemory(HL.data));
				break;

			case SWAP_A:
				//4 Clock Cycles
				swapReg(AF.hi);
				break;

			case SRL_B:
				//4 Clock Cycles
				shiftReg(false, false, BC.hi);
				break;

			case SRL_C:
				//4 Clock Cycles
				shiftReg(false, false, BC.lo);
				break;

			case SRL_D:
				//4 Clock Cycles
				shiftReg(false, false, DE.hi);
				break;

			case SRL_E:
				//4 Clock Cycles
				shiftReg(false, false, DE.lo);
				break;

			case SRL_H:
				//4 Clock Cycles
				shiftReg(false, false, HL.hi);
				break;

			case SRL_L:
				//4 Clock Cycles
				shiftReg(false, false, HL.lo);
				break;

			case SRL_adrHL:
				//8 Clock Cycles
				shiftReg(false, false, memory->readMemory(HL.data));
				break;

			case SRL_A:
				//4 Clock Cycles
				shiftReg(false, false, AF.hi);
				break;

			case BIT_1_B:
				//4 Clock Cycles
				test_bit(0, BC.hi);
				break;

			case BIT_1_C:
				//4 Clock Cycles
				test_bit(0, BC.lo);
				break;

			case BIT_1_D:
				//4 Clock Cycles
				test_bit(0, DE.hi);
				break;

			case BIT_1_E:
				//4 Clock Cycles
				test_bit(0, DE.lo);
				break;

			case BIT_1_H:
				//4 Clock Cycles
				test_bit(0, HL.hi);
				break;

			case BIT_1_L:
				//4 Clock Cycles
				test_bit(0, HL.lo);
				break;

			case BIT_1_adrHL:
				//8 Clock Cycles
				test_bit(0, memory->readMemory(HL.data));
				break;

			case BIT_1_A:
				//4 Clock Cycles
				test_bit(0, AF.hi);
				break;

			case BIT_2_B:
				//4 Clock Cycles
				test_bit(1, BC.hi);
				break;

			case BIT_2_C:
				//4 Clock Cycles
				test_bit(1, BC.lo);
				break;

			case BIT_2_D:
				//4 Clock Cycles
				test_bit(1, DE.hi);
				break;

			case BIT_2_E:
				//4 Clock Cycles
				test_bit(1, DE.lo);
				break;

			case BIT_2_H:
				//4 Clock Cycles
				test_bit(1, HL.hi);
				break;

			case BIT_2_L:
				//4 Clock Cycles
				test_bit(1, HL.lo);
				break;

			case BIT_2_adrHL:
				//8 Clock Cycles
				test_bit(1, memory->readMemory(HL.data));
				break;

			case BIT_2_A:
				//4 Clock Cycles
				test_bit(1, AF.hi);
				break;

			case BIT_3_B:
				//4 Clock Cycles
				test_bit(2, BC.hi);
				break;

			case BIT_3_C:
				//4 Clock Cycles
				test_bit(2, BC.lo);
				break;

			case BIT_3_D:
				//4 Clock Cycles
				test_bit(2, DE.hi);
				break;

			case BIT_3_E:
				//4 Clock Cycles
				test_bit(2, DE.lo);
				break;

			case BIT_3_H:
				//4 Clock Cycles
				test_bit(2, HL.hi);
				break;

			case BIT_3_L:
				//4 Clock Cycles
				test_bit(2, HL.lo);
				break;

			case BIT_3_adrHL:
				//8 Clock Cycles
				test_bit(2, memory->readMemory(HL.data));
				break;

			case BIT_3_A:
				//4 Clock Cycles
				test_bit(2, AF.hi);
				break;

			case BIT_4_B:
				//4 Clock Cycles
				test_bit(3, BC.hi);
				break;

			case BIT_4_C:
				//4 Clock Cycles
				test_bit(3, BC.lo);
				break;

			case BIT_4_D:
				//4 Clock Cycles
				test_bit(3, DE.hi);
				break;

			case BIT_4_E:
				//4 Clock Cycles
				test_bit(3, DE.lo);
				break;

			case BIT_4_H:
				//4 Clock Cycles
				test_bit(3, HL.hi);
				break;

			case BIT_4_L:
				//4 Clock Cycles
				test_bit(3, HL.lo);
				break;

			case BIT_4_adrHL:
				//8 Clock Cycles
				test_bit(3, memory->readMemory(HL.data));
				break;

			case BIT_4_A:
				//4 Clock Cycles
				test_bit(3, AF.hi);
				break;

			case BIT_5_B:
				//4 Clock Cycles
				test_bit(4, BC.hi);
				break;

			case BIT_5_C:
				//4 Clock Cycles
				test_bit(4, BC.lo);
				break;

			case BIT_5_D:
				//4 Clock Cycles
				test_bit(4, DE.hi);
				break;

			case BIT_5_E:
				//4 Clock Cycles
				test_bit(4, DE.lo);
				break;

			case BIT_5_H:
				//4 Clock Cycles
				test_bit(4, HL.hi);
				break;

			case BIT_5_L:
				//4 Clock Cycles
				test_bit(4, HL.lo);
				break;
			
			case BIT_5_adrHL:
				//8 Clock Cycles
				test_bit(4, memory->readMemory(HL.data));
				break;

			case BIT_5_A:
				//4 Clock Cycles
				test_bit(4, AF.hi);
				break;

			case BIT_6_B:
				//4 Clock Cycles
				test_bit(5, BC.hi);
				break;

			case BIT_6_C:
				//4 Clock Cycles
				test_bit(5, BC.lo);
				break;

			case BIT_6_D:
				//4 Clock Cycles
				test_bit(5, DE.hi);
				break;

			case BIT_6_E:
				//4 Clock Cycles
				test_bit(5, DE.lo);
				break;

			case BIT_6_H:
				//4 Clock Cycles
				test_bit(5, HL.hi);
				break;

			case BIT_6_L:
				//4 Clock Cycles
				test_bit(5, HL.lo);
				break;

			case BIT_6_adrHL:
				//8 Clock Cycles
				test_bit(5, memory->readMemory(HL.data));
				break;
				
			case BIT_6_A:
				//4 Clock Cycles
				test_bit(5, AF.hi);
				break;

			case BIT_7_B:
				//4 Clock Cycles
				test_bit(6, BC.hi);
				break;

			case BIT_7_C:
				//4 Clock Cycles
				test_bit(6, BC.lo);
				break;

			case BIT_7_D:
				//4 Clock Cycles
				test_bit(6, DE.hi);
				break;

			case BIT_7_E:
				//4 Clock Cycles
				test_bit(6, DE.lo);
				break;

			case BIT_7_H:
				//4 Clock Cycles
				test_bit(6, HL.hi);
				break;

			case BIT_7_L:
				//4 Clock Cycles
				test_bit(6, HL.lo);
				break;

			case BIT_7_adrHL:
				//8 Clock Cycles
				test_bit(6, memory->readMemory(HL.data));
				break;

			case BIT_7_A:
				//4 Clock Cycles
				test_bit(6, AF.hi);
				break;

			case BIT_8_B:
				//4 Clock Cycles
				test_bit(7, BC.hi);
				break;

			case BIT_8_C:
				//4 Clock Cycles
				test_bit(7, BC.lo);
				break;

			case BIT_8_D:
				//4 Clock Cycles
				test_bit(7, DE.hi);
				break;

			case BIT_8_E:
				//4 Clock Cycles
				test_bit(7, DE.lo);
				break;

			case BIT_8_H:
				//4 Clock Cycles
				test_bit(7, HL.hi);
				break;

			case BIT_8_L:
				//4 Clock Cycles
				test_bit(7, HL.lo);
				break;

			case BIT_8_adrHL:
				//8 Clock Cycles
				test_bit(7, memory->readMemory(HL.data));
				break;

			case BIT_8_A:
				//4 Clock Cycles
				test_bit(7, AF.hi);
				break;

			case RES_1_B:
				//4 Clock Cycles
				reset_bit(0, BC.hi);
				break;

			case RES_1_C:
				//4 Clock Cycles
				reset_bit(0, BC.lo);
				break;

			case RES_1_D:
				//4 Clock Cycles
				reset_bit(0, DE.hi);
				break;

			case RES_1_E:
				//4 Clock Cycles
				reset_bit(0, DE.lo);
				break;

			case RES_1_H:
				//4 Clock Cycles
				reset_bit(0, HL.hi);
				break;

			case RES_1_L:
				//4 Clock Cycles
				reset_bit(0, HL.lo);
				break;

			case RES_1_adrHL:
				//8 Clock Cycles
				reset_bit(0, memory->readMemory(HL.data));
				break;

			case RES_1_A:
				//4 Clock Cycles
				reset_bit(0, AF.hi);
				break;

			case RES_2_B:
				//4 Clock Cycles
				reset_bit(1, BC.hi);
				break;

			case RES_2_C:
				//4 Clock Cycles
				reset_bit(1, BC.lo);
				break;

			case RES_2_D:
				//4 Clock Cycles
				reset_bit(1, DE.hi);
				break;

			case RES_2_E:
				//4 Clock Cycles
				reset_bit(1, DE.lo);
				break;

			case RES_2_H:
				//4 Clock Cycles
				reset_bit(1, HL.hi);
				break;

			case RES_2_L:
				//4 Clock Cycles
				reset_bit(1, HL.lo);
				break;

			case RES_2_adrHL:
				//8 Clock Cycles
				reset_bit(1, memory->readMemory(HL.data));
				break;

			case RES_2_A:
				//4 Clock Cycles
				reset_bit(1, AF.hi);
				break;

			case RES_3_B:
				//4 Clock Cycles
				reset_bit(2, BC.hi);
				break;

			case RES_3_C:
				//4 Clock Cycles
				reset_bit(2, BC.lo);
				break;

			case RES_3_D:
				//4 Clock Cycles
				reset_bit(2, DE.hi);
				break;

			case RES_3_E:
				//4 Clock Cycles
				reset_bit(2, DE.lo);
				break;

			case RES_3_H:
				//4 Clock Cycles
				reset_bit(2, HL.hi);
				break;

			case RES_3_L:
				//4 Clock Cycles
				reset_bit(2, HL.lo);
				break;

			case RES_3_adrHL:
				//8 Clock Cycles
				reset_bit(2, memory->readMemory(HL.data));
				break;

			case RES_3_A:
				//4 Clock Cycles
				reset_bit(2, AF.hi);
				break;

			case RES_4_B:
				//4 Clock Cycles
				reset_bit(3, BC.hi);
				break;

			case RES_4_C:
				//4 Clock Cycles
				reset_bit(3, BC.lo);
				break;

			case RES_4_D:
				//4 Clock Cycles
				reset_bit(3, DE.hi);
				break;

			case RES_4_E:
				//4 Clock Cycles
				reset_bit(3, DE.lo);
				break;

			case RES_4_H:
				//4 Clock Cycles
				reset_bit(3, HL.hi);
				break;

			case RES_4_L:
				//4 Clock Cycles
				reset_bit(3, HL.lo);
				break;

			case RES_4_adrHL:
				//8 Clock Cycles
				reset_bit(3, memory->readMemory(HL.data));
				break;

			case RES_4_A:
				//4 Clock Cycles
				reset_bit(3, AF.hi);
				break;

			case RES_5_B:
				//4 Clock Cycles
				reset_bit(4, BC.hi);
				break;

			case RES_5_C:
				//4 Clock Cycles
				reset_bit(4, BC.lo);
				break;

			case RES_5_D:
				//4 Clock Cycles
				reset_bit(4, DE.hi);
				break;

			case RES_5_E:
				//4 Clock Cycles
				reset_bit(4, DE.lo);
				break;

			case RES_5_H:
				//4 Clock Cycles
				reset_bit(4, HL.hi);
				break;

			case RES_5_L:
				//4 Clock Cycles
				reset_bit(4, HL.lo);
				break;

			case RES_5_adrHL:
				//8 Clock Cycles
				reset_bit(4, memory->readMemory(HL.data));
				break;

			case RES_5_A:
				//4 Clock Cycles
				reset_bit(4, AF.hi);
				break;

			case RES_6_B:
				//4 Clock Cycles
				reset_bit(5, BC.hi);
				break;

			case RES_6_C:
				//4 Clock Cycles
				reset_bit(5, BC.lo);
				break;

			case RES_6_D:
				//4 Clock Cycles
				reset_bit(5, DE.hi);
				break;

			case RES_6_E:
				//4 Clock Cycles
				reset_bit(5, DE.lo);
				break;

			case RES_6_H:
				//4 Clock Cycles
				reset_bit(5, HL.hi);
				break;

			case RES_6_L:
				//4 Clock Cycles
				reset_bit(5, HL.lo);
				break;

			case RES_6_adrHL:
				//8 Clock Cycles
				reset_bit(5, memory->readMemory(HL.data));
				break;

			case RES_6_A:
				//4 Clock Cycles
				reset_bit(5, AF.hi);
				break;

			case RES_7_B:
				//4 Clock Cycles
				reset_bit(6, BC.hi);
				break;

			case RES_7_C:
				//4 Clock Cycles
				reset_bit(6, BC.lo);
				break;

			case RES_7_D:
				//4 Clock Cycles
				reset_bit(6, DE.hi);
				break;

			case RES_7_E:
				//4 Clock Cycles
				reset_bit(6, DE.lo);
				break;

			case RES_7_H:
				//4 Clock Cycles
				reset_bit(6, HL.hi);
				break;

			case RES_7_L:
				//4 Clock Cycles
				reset_bit(6, HL.lo);
				break;

			case RES_7_adrHL:
				//8 Clock Cycles
				reset_bit(6, memory->readMemory(HL.data));
				break;

			case RES_7_A:
				//4 Clock Cycles
				reset_bit(6, AF.hi);
				break;

			case RES_8_B:
				//4 Clock Cycles
				reset_bit(7, BC.hi);
				break;

			case RES_8_C:
				//4 Clock Cycles
				reset_bit(7, BC.lo);
				break;

			case RES_8_D:
				//4 Clock Cycles
				reset_bit(7, DE.hi);
				break;

			case RES_8_E:
				//4 Clock Cycles
				reset_bit(7, DE.lo);
				break;

			case RES_8_H:
				//4 Clock Cycles
				reset_bit(7, HL.hi);
				break;

			case RES_8_L:
				//4 Clock Cycles
				reset_bit(7, HL.lo);
				break;

			case RES_8_adrHL:
				//8 Clock Cycles
				reset_bit(7, memory->readMemory(HL.data));
				break;

			case RES_8_A:
				//4 Clock Cycles
				reset_bit(7, AF.hi);
				break;

			case SET_1_B:
				//4 Clock Cycles
				set_bit(0, BC.hi);
				break;

			case SET_1_C:
				//4 Clock Cycles
				set_bit(0, BC.lo);
				break;

			case SET_1_D:
				//4 Clock Cycles
				set_bit(0, DE.hi);
				break;

			case SET_1_E:
				//4 Clock Cycles
				set_bit(0, DE.lo);
				break;

			case SET_1_H:
				//4 Clock Cycles
				set_bit(0, HL.hi);
				break;

			case SET_1_L:
				//4 Clock Cycles
				set_bit(0, HL.lo);
				break;

			case SET_1_adrHL:
				//8 Clock Cycles
				set_bit(0, memory->readMemory(HL.data));
				break;

			case SET_1_A:
				//4 Clock Cycles
				set_bit(0, AF.hi);
				break;

			case SET_2_B:
				//4 Clock Cycles
				set_bit(1, BC.hi);
				break;

			case SET_2_C:
				//4 Clock Cycles
				set_bit(1, BC.lo);
				break;

			case SET_2_D:
				//4 Clock Cycles
				set_bit(1, DE.hi);
				break;

			case SET_2_E:
				//4 Clock Cycles
				set_bit(1, DE.lo);
				break;

			case SET_2_H:
				//4 Clock Cycles
				set_bit(1, HL.hi);
				break;

			case SET_2_L:
				//4 Clock Cycles
				set_bit(1, HL.lo);
				break;

			case SET_2_adrHL:
				//8 Clock Cycles
				set_bit(1, memory->readMemory(HL.data));
				break;

			case SET_2_A:
				//4 Clock Cycles
				set_bit(1, AF.hi);
				break;

			case SET_3_B:
				//4 Clock Cycles
				set_bit(2, BC.hi);
				break;

			case SET_3_C:
				//4 Clock Cycles
				set_bit(2, BC.lo);
				break;

			case SET_3_D:
				//4 Clock Cycles
				set_bit(2, DE.hi);
				break;

			case SET_3_E:
				//4 Clock Cycles
				set_bit(2, DE.lo);
				break;

			case SET_3_H:
				//4 Clock Cycles
				set_bit(2, HL.hi);
				break;

			case SET_3_L:
				//4 Clock Cycles
				set_bit(2, HL.lo);
				break;

			case SET_3_adrHL:
				//8 Clock Cycles
				set_bit(2, memory->readMemory(HL.data));
				break;

			case SET_3_A:
				//4 Clock Cycles
				set_bit(2, AF.hi);
				break;

			case SET_4_B:
				//4 Clock Cycles
				set_bit(3, BC.hi);
				break;

			case SET_4_C:
				//4 Clock Cycles
				set_bit(3, BC.lo);
				break;

			case SET_4_D:
				//4 Clock Cycles
				set_bit(3, DE.hi);
				break;

			case SET_4_E:
				//4 Clock Cycles
				set_bit(3, DE.lo);
				break;

			case SET_4_H:
				//4 Clock Cycles
				set_bit(3, HL.hi);
				break;

			case SET_4_L:
				//4 Clock Cycles
				set_bit(3, HL.lo);
				break;

			case SET_4_adrHL:
				//8 Clock Cycles
				set_bit(3, memory->readMemory(HL.data));
				break;

			case SET_4_A:
				//4 Clock Cycles
				set_bit(3, AF.hi);
				break;

			case SET_5_B:
				//4 Clock Cycles
				set_bit(4, BC.hi);
				break;

			case SET_5_C:
				//4 Clock Cycles
				set_bit(4, BC.lo);
				break;

			case SET_5_D:
				//4 Clock Cycles
				set_bit(4, DE.hi);
				break;

			case SET_5_E:
				//4 Clock Cycles
				set_bit(4, DE.lo);
				break;

			case SET_5_H:
				//4 Clock Cycles
				set_bit(4, HL.hi);
				break;

			case SET_5_L:
				//4 Clock Cycles
				set_bit(4, HL.lo);
				break;

			case SET_5_adrHL:
				//8 Clock Cycles
				set_bit(4, memory->readMemory(HL.data));
				break;

			case SET_5_A:
				//4 Clock Cycles
				set_bit(4, AF.hi);
				break;

			case SET_6_B:
				//4 Clock Cycles
				set_bit(5, BC.hi);
				break;

			case SET_6_C:
				//4 Clock Cycles
				set_bit(5, BC.lo);
				break;

			case SET_6_D:
				//4 Clock Cycles
				set_bit(5, DE.hi);
				break;

			case SET_6_E:
				//4 Clock Cycles
				set_bit(5, DE.lo);
				break;

			case SET_6_H:
				//4 Clock Cycles
				set_bit(5, HL.hi);
				break;

			case SET_6_L:
				//4 Clock Cycles
				set_bit(5, HL.lo);
				break;

			case SET_6_adrHL:
				//8 Clock Cycles
				set_bit(5, memory->readMemory(HL.data));
				break;

			case SET_6_A:
				//4 Clock Cycles
				set_bit(5, AF.hi);
				break;

			case SET_7_B:
				//4 Clock Cycles
				set_bit(6, BC.hi);
				break;

			case SET_7_C:
				//4 Clock Cycles
				set_bit(6, BC.lo);
				break;

			case SET_7_D:
				//4 Clock Cycles
				set_bit(6, DE.hi);
				break;

			case SET_7_E:
				//4 Clock Cycles
				set_bit(6, DE.lo);
				break;

			case SET_7_H:
				//4 Clock Cycles
				set_bit(6, HL.hi);
				break;

			case SET_7_L:
				//4 Clock Cycles
				set_bit(6, HL.lo);
				break;

			case SET_7_adrHL:
				//8 Clock Cycles
				set_bit(6, memory->readMemory(HL.data));
				break;

			case SET_7_A:
				//4 Clock Cycles
				set_bit(6, AF.hi);
				break;

			case SET_8_B:
				//4 Clock Cycles
				set_bit(7, BC.hi);
				break;

			case SET_8_C:
				//4 Clock Cycles
				set_bit(7, BC.lo);
				break;
				
			case SET_8_D:
				//4 Clock Cycles
				set_bit(7, DE.hi);
				break;

			case SET_8_E:
				//4 Clock Cycles
				set_bit(7, DE.lo);
				break;

			case SET_8_H:
				//4 Clock Cycles
				set_bit(7, HL.hi);
				break;

			case SET_8_L:
				//4 Clock Cycles
				set_bit(7, HL.lo);
				break;

			case SET_8_adrHL:
				//8 Clock Cycles
				set_bit(7, memory->readMemory(HL.data));
				break;

			case SET_8_A:
				//4 Clock Cycles
				set_bit(7, AF.hi);
				break;

			default:
				break;
			}
			break;

		case CALL_ZERO:
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
		CPU_SLEEP_FOR_MACHINE_CYCLE();
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
		CPU_SLEEP_FOR_MACHINE_CYCLE();
		--reg;
	}

	void CPU::add16BitRegister(uWORD& host, uWORD operand)
	{
		CPU_SLEEP_FOR_MACHINE_CYCLE();
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

			for (int i = 7; i > 1; --i)
			{
				BitField.set(i, BitField[i - 1]);
			}

			if (withCarry)
				BitField.set(0, oldCarry);
			else
				BitField.set(0, MSB);
		}
		else //Right
		{
			bool oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
			bool LSB = BitField[0];

			if (LSB)
				CPU_FLAG_BIT_SET(C_FLAG);
			else
				CPU_FLAG_BIT_RESET(C_FLAG);

			for (int i = 0; i < BitField.size() - 1; ++i)
			{
				BitField.set(i, BitField[i + 1]);
			}

			if (withCarry)
				BitField.set(7, oldCarry);
			else
				BitField.set(7, LSB);
		}

		reg = BitField.to_ulong();

		CPU_FLAG_BIT_RESET(N_FLAG);
		CPU_FLAG_BIT_RESET(H_FLAG);

		if (reg == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
	}

	void CPU::shiftReg(bool direction, bool keepMSB, uBYTE& reg)
	{
		std::bitset<8> BitField(reg);
		if (direction) //left
		{
			if (BitField[7])
				CPU_FLAG_BIT_SET(C_FLAG);
			else
				CPU_FLAG_BIT_RESET(C_FLAG);

			for (int i = 7; i > 1; --i)
			{
				BitField.set(i, BitField[i - 1]);
			}

			BitField[0] = false;
		}
		else //Right
		{
			if (BitField[0])
				CPU_FLAG_BIT_SET(C_FLAG);
			else
				CPU_FLAG_BIT_RESET(C_FLAG);

			for (int i = 1; i < BitField.size() - 1; ++i)
			{
				BitField.set(i, BitField[i + 1]);
			}

			if (!keepMSB)
				BitField[7] = false;
		}

		reg = BitField.to_ulong();

		if (reg == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);

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

		CPU_FLAG_BIT_RESET(N_FLAG);
		CPU_FLAG_BIT_RESET(H_FLAG);
		CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::Flag_set(int flag)
	{
		FlagBits->set(flag);
		AF.lo = FlagBits->to_ulong();
	}

	void CPU::Flag_reset(int flag)
	{
		FlagBits->reset(flag);
		AF.lo = FlagBits->to_ulong();
	}

	bool CPU::Flag_test(int flag)
	{
		return FlagBits->test(flag);
	}

	void CPU::test_bit(int pos, uBYTE reg)
	{
		std::bitset<8> BitField(reg);
		if (!BitField.test(pos))
			CPU_FLAG_BIT_SET(Z_FLAG);

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
}
