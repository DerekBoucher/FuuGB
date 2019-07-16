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

		memory = mem;

		timer_update_cnt = 0;
        divider_register = 0;
        divider_count = 0;
        
		_cpuRunning = true;
		_cpuPaused = false;
		//_cpuTHR = new std::thread(&CPU::clock, this);
		FlagBits = new std::bitset<sizeof(uBYTE)*8>(&AF.lo);
		AluBits = new std::bitset<sizeof(uBYTE)*8>(&AF.hi);
		
		FUUGB_CPU_LOG("CPU Initialized.");
	}

	CPU::~CPU()
	{
		//_cpuTHR->join();
		//delete _cpuTHR;
		FUUGB_CPU_LOG("CPU Destroyed.");
	}

	void CPU::Pause()
	{
		_cpuPaused = true;
	}

	void CPU::clock()
	{
		while (_cpuRunning)
		{
			if (_cpuPaused)
			{
				std::unique_lock<std::mutex> pauseLock(Shared::mu_GB);
				Shared::cv_GB.wait(pauseLock);
				pauseLock.unlock();
				_cpuPaused = false;
			}
			executeNextOpCode();
			checkInterupts();
		}
	}

	int CPU::executeNextOpCode()
	{
		timer_update_cnt = 0;
		uBYTE byte = memory->readMemory(PC++);
		uBYTE SP_data = 0x0;
		Register* temp = new Register();
		printf("[CPU]: Executing next OpCode @PC=%x: %x\n", PC-1,byte);
        if(PC-1 == 0x6d && memory->readMemory(0xFF50) == 0x01)
            printf("");
		if (memory->readMemory(0xFF44) == 144)
			printf("");
		switch (byte)
		{
		case NOP:
			//4 CPU Cycle
			timer_update_cnt += 4;
			break;

		case LD_16IMM_BC:
			//12 CPU Cycles
			BC.lo = memory->readMemory(PC++);
			BC.hi = memory->readMemory(PC++);
			timer_update_cnt += 12;
			break;

		case LD_A_adrBC:
			//8 CPU Cycles
			memory->writeMemory(BC.data, AF.hi);
			timer_update_cnt += 8;
			break;

		case INC_BC:
			//8 CPU Cycles
			increment16BitRegister(BC.data);
			timer_update_cnt += 8;
			break;

		case INC_B:
			//4 CPU Cycles
			increment8BitRegister(BC.hi);
			timer_update_cnt += 4;
			break;

		case DEC_B:
			//4 CPU Cycles
			decrement8BitRegister(BC.hi);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_B:
			//8 CPU Cycles
			BC.hi = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case RLC_A:
			//4 CPU Cycles
			rotateReg(true, false, AF.hi);
			timer_update_cnt += 4;
			break;

		case LD_SP_adr:
			//20 CPU cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			SP_data = memory->readMemory(SP);
			memory->writeMemory(temp->data, SP_data);
			timer_update_cnt += 20;
			break;

		case ADD_BC_HL:
			//8 CPU Cycles
			add16BitRegister(HL.data, BC.data);
			timer_update_cnt += 8;
			break;

		case LD_adrBC_A:
			//8 CPU Cycles
			AF.hi = memory->readMemory(BC.data);
			timer_update_cnt += 8;
			break;

		case DEC_BC:
			//8 CPU Cycles
			decrement16BitRegister(BC.data);
			timer_update_cnt += 8;
			break;

		case INC_C:
			//4 CPU Cycles
			increment8BitRegister(BC.lo);
			timer_update_cnt += 4;
			break;

		case DEC_C:
			//4 CPU Cycles
			decrement8BitRegister(BC.lo);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_C:
			//8 CPU Cycles
			BC.lo = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case RRC_A:
			//4 CPU Cycles
			rotateReg(false, false, AF.hi);
			timer_update_cnt += 4;
			break;

		case STOP:
			//4 Clock Cycles
			_cpuPaused = true;
			break;

		case LD_16IMM_DE:
			//12 Clock Cycles
			DE.lo = memory->readMemory(PC++);
			DE.hi = memory->readMemory(PC++);
			timer_update_cnt += 12;
			break;

		case LD_A_adrDE:
			//8 Clock Cycles
			memory->writeMemory(DE.data, AF.hi);
			timer_update_cnt += 8;
			break;

		case INC_DE:
			//8 Clock Cycles
			increment16BitRegister(DE.data);
			timer_update_cnt += 8;
			break;

		case INC_D:
			//4 Clock Cycles
			increment8BitRegister(DE.hi);
			timer_update_cnt += 4;
			break;

		case DEC_D:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_D:
			//8 Clock Cycles
			DE.hi = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case RL_A:
			//4 Clock Cycles
			rotateReg(true, true, AF.hi);
			timer_update_cnt += 4;
			break;

		case RJmp_IMM:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (TestBitInByte(byte, 7))
				PC = PC - twoComp_Byte(byte);
			else
				PC = PC + byte;
			timer_update_cnt += 8;
			break;

		case ADD_DE_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, DE.data);
			timer_update_cnt += 8;
			break;

		case LD_adrDE_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(DE.data);
			timer_update_cnt += 8;
			break;

		case DEC_DE:
			//4 Clock Cycles
			decrement16BitRegister(DE.data);
			timer_update_cnt += 4;
			break;

		case INC_E:
			//4 Clock Cycles
			increment8BitRegister(DE.lo);
			timer_update_cnt += 4;
			break;

		case DEC_E:
			//4 clock cycles
			decrement8BitRegister(DE.lo);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_E:
			//8 Clock Cycles
			DE.lo = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case RR_A:
			//4 clock cycles
			rotateReg(false, true, AF.hi);
			timer_update_cnt += 4;
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
			timer_update_cnt += 8;
			break;

		case LD_16IMM_HL:
			//12 Clock Cycles
			HL.lo = memory->readMemory(PC++);
			HL.hi = memory->readMemory(PC++);
			timer_update_cnt += 12;
			break;

		case LDI_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data++, AF.hi);
			timer_update_cnt += 8;
			break;

		case INC_HL:
			//4 Clock Cycles
			increment16BitRegister(HL.data);
			timer_update_cnt += 4;
			break;

		case INC_H:
			//4 Clock Cycles
			increment8BitRegister(HL.hi);
			timer_update_cnt += 4;
			break;

		case DEC_H:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_H:
			//8 Clock Cycles
			HL.hi = memory->readMemory(PC++);
			timer_update_cnt += 8;
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
			timer_update_cnt += 8;
			break;

		case ADD_HL_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, HL.data);
			timer_update_cnt += 8;
			break;

		case LDI_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data++);
			timer_update_cnt += 8;
			break;

		case DEC_HL:
			//4 Clock Cycles
			decrement16BitRegister(HL.data);
			timer_update_cnt += 4;
			break;

		case INC_L:
			//4 Clock Cycles
			increment8BitRegister(HL.lo);
			timer_update_cnt += 4;
			break;

		case DEC_L:
			//4 Clock Cycles
			decrement8BitRegister(HL.lo);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_L:
			//8 Clock Cycles
			HL.lo = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case NOT_A:
			//4 Clock Cycles
			AF.hi = ~AF.hi;
			timer_update_cnt += 4;
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
			timer_update_cnt += 8;
			break;

		case LD_16IM_SP:
			//12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			SP = temp->data;
			timer_update_cnt += 12;
			break;

		case LDD_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data--, AF.hi);
			timer_update_cnt += 8;
			break;

		case INC_SP:
			//8 Clock Cycles
			increment16BitRegister(SP);
			timer_update_cnt += 8;
			break;

		case INC_valHL:
			//12 Clock Cycles
			byte = memory->readMemory(HL.data);
			++byte;
			if (byte == 0x00)
				CPU_FLAG_BIT_SET(Z_FLAG);
			else
				CPU_FLAG_BIT_RESET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);

			if (checkCarryFromBit_Byte(2, byte, 0x01))
				CPU_FLAG_BIT_SET(H_FLAG);

			memory->writeMemory(HL.data, byte);
			timer_update_cnt += 12;
			break;
			
		case DEC_valHL:
			//12 Clock Cycles
			byte = memory->readMemory(HL.data);
			--byte;
			if (byte == 0x00)
				CPU_FLAG_BIT_SET(Z_FLAG);
			else
				CPU_FLAG_BIT_RESET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);

			if (!checkBorrowFromBit_Byte(3, byte, 0x01))
				CPU_FLAG_BIT_SET(H_FLAG);

			memory->writeMemory(HL.data, byte);
			timer_update_cnt += 12;
			break;

		case LD_8IMM_adrHL:
			//12 Clock Cycles
			byte = memory->readMemory(PC++);
			memory->writeMemory(HL.data, byte);
			timer_update_cnt += 12;
			break;

		case SET_CARRY_FLAG:
			//4 Clock Cycles
			CPU_FLAG_BIT_RESET(N_FLAG);
			CPU_FLAG_BIT_RESET(H_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG);
			timer_update_cnt += 4;
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
			timer_update_cnt += 8;
			break;

		case ADD_SP_HL:
			//8 Clock Cycles
			add16BitRegister(HL.data, SP);
			timer_update_cnt += 8;
			break;

		case LDD_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data--);
			timer_update_cnt += 8;
			break;

		case DEC_SP:
			//8 Clock Cycles;
			decrement16BitRegister(SP);
			timer_update_cnt += 8;
			break;

		case INC_A:
			//4 Clock Cycles
			increment8BitRegister(AF.hi);
			timer_update_cnt += 4;
			break;

		case DEC_A:
			//4 Clock Cycles
			decrement8BitRegister(AF.hi);
			timer_update_cnt += 4;
			break;

		case LD_8IMM_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(PC++);
			timer_update_cnt += 8;
			break;

		case COMP_CARRY_FLAG:
			//4 Clock Cycles
			if (CPU_FLAG_BIT_TEST(C_FLAG))
				CPU_FLAG_BIT_RESET(C_FLAG);
			else
				CPU_FLAG_BIT_SET(C_FLAG);
			
			CPU_FLAG_BIT_RESET(N_FLAG);
			CPU_FLAG_BIT_RESET(H_FLAG);
			timer_update_cnt += 4;
			break;

		case LD_B_B:
			//4 Clock Cycles
			BC.hi = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_B:
			//4 Clock Cycles
			BC.hi = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_B:
			//4 Clock Cycles
			BC.hi = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_B:
			//4 Clock Cycles
			BC.hi = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_B:
			//4 Clock Cycles
			BC.hi = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_B:
			//4 Clock Cycles
			BC.hi = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_B:
			//8 Clock Cycles
			BC.hi = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_B:
			//4 Clock Cycles
			BC.hi = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_C:
			//4 Clock Cycles
			BC.lo = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_C:
			//4 Clock Cycles
			BC.lo = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_C:
			//4 Clock Cycles
			BC.lo = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_C:
			//4 Clock Cycles
			BC.lo = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_C:
			//4 Clock Cycles
			BC.lo = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_C:
			//4 Clock Cycles
			BC.lo = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_C:
			//8 Clock Cycles
			BC.lo = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_C:
			//4 Clock Cycles
			BC.lo = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_D:
			//4 Clock Cycles
			DE.hi = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_D:
			//4 Clock Cycles
			DE.hi = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_D:
			//4 Clock Cycles
			DE.hi = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_D:
			//4 Clock Cycles
			DE.hi = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_D:
			//4 Clock Cycles
			DE.hi = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_D:
			//4 Clock Cycles
			DE.hi = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_D:
			//8 Clock Cycles
			DE.hi = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_D:
			//4 Clock Cycles
			DE.hi = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_E:
			//4 Clock Cycles
			DE.lo = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_E:
			//4 Clock Cycles
			DE.lo = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_E:
			//4 Clock Cycles
			DE.lo = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_E:
			//4 Clock Cycles
			DE.lo = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_E:
			//4 Clock Cycles
			DE.lo = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_E:
			//4 Clock Cycles
			DE.lo = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_E:
			//8 Clock Cycles
			DE.lo = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_E:
			//4 Clock Cycles
			DE.lo = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_H:
			//4 Clock Cycles
			HL.hi = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_H:
			//4 Clock Cycles
			HL.hi = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_H:
			//4 Clock Cycles
			HL.hi = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_H:
			//4 Clock Cycles
			HL.hi = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_H:
			//4 Clock Cycles
			HL.hi = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_H:
			//4 Clock Cycles
			HL.hi = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_H:
			//8 Clock Cycles
			HL.hi = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_H:
			//4 Clock Cycles
			HL.hi = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_L:
			//4 Clock Cycles
			HL.lo = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_L:
			//4 Clock Cycles
			HL.lo = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_L:
			//4 Clock Cycles
			HL.lo = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_L:
			//4 Clock Cycles
			HL.lo = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_L:
			//4 Clock Cycles
			HL.lo = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_L:
			//4 Clock Cycles
			HL.lo = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_L:
			//8 Clock Cycles
			HL.lo = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_L:
			//4 Clock Cycles
			HL.lo = AF.hi;
			timer_update_cnt += 4;
			break;

		case LD_B_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, BC.hi);
			timer_update_cnt += 8;
			break;

		case LD_C_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, BC.lo);
			timer_update_cnt += 8;
			break;

		case LD_D_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, DE.hi);
			timer_update_cnt += 8;
			break;
			
		case LD_E_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, DE.lo);
			timer_update_cnt += 8;
			break;

		case LD_H_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, HL.hi);
			timer_update_cnt += 8;
			break;

		case LD_L_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, HL.lo);
			timer_update_cnt += 8;
			break;

		case HALT:
			//4 Clock Cycles
			halt();
			timer_update_cnt += 4;
			break;

		case LD_A_adrHL:
			//8 Clock Cycles
			memory->writeMemory(HL.data, AF.hi);
			timer_update_cnt += 8;
			break;

		case LD_B_A:
			//4 Clock Cycles
			AF.hi = BC.hi;
			timer_update_cnt += 4;
			break;

		case LD_C_A:
			//4 Clock Cycles
			AF.hi = BC.lo;
			timer_update_cnt += 4;
			break;

		case LD_D_A:
			//4 Clock Cycles
			AF.hi = DE.hi;
			timer_update_cnt += 4;
			break;

		case LD_E_A:
			//4 Clock Cycles
			AF.hi = DE.lo;
			timer_update_cnt += 4;
			break;

		case LD_H_A:
			//4 Clock Cycles
			AF.hi = HL.hi;
			timer_update_cnt += 4;
			break;

		case LD_L_A:
			//4 Clock Cycles
			AF.hi = HL.lo;
			timer_update_cnt += 4;
			break;

		case LD_adrHL_A:
			//8 Clock Cycles
			AF.hi = memory->readMemory(HL.data);
			timer_update_cnt += 8;
			break;

		case LD_A_A:
			//4 Clock Cycles
			AF.hi = AF.hi;
			timer_update_cnt += 4;
			break;

		case ADD_B_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case ADD_C_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break;

		case ADD_D_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case ADD_E_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case ADD_H_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;

		case ADD_L_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case ADD_adrHL_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 8;
			break;

		case ADD_A_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case ADC_B_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_C_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_D_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_E_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_H_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_L_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case ADC_adrHL_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 8;
			break;

		case ADC_A_A:
			//4 Clock Cycles
			add8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SUB_B_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case SUB_C_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break; 

		case SUB_D_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case SUB_E_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case SUB_H_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;
			break;

		case SUB_L_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case SUB_adrHL_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 8;
			break;

		case SUB_A_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case SBC_B_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_C_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, BC.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_D_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_E_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, DE.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_H_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_L_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, HL.lo, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case SBC_adrHL_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(HL.data), CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 8;
			break;

		case SBC_A_A:
			//4 Clock Cycles
			sub8BitRegister(AF.hi, AF.hi, CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 4;
			break;

		case AND_B_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case AND_C_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break;

		case AND_D_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case AND_E_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case AND_H_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;
			break;

		case AND_L_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case AND_adrHL_A:
			//8 Clock Cycles
			and8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 8;
			break;

		case AND_A_A:
			//4 Clock Cycles
			and8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case XOR_B_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case XOR_C_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break;

		case XOR_D_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case XOR_E_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case XOR_H_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;
			break;

		case XOR_L_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case XOR_adrHL_A:
			//8 Clock Cycles
			xor8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 8;
			break;

		case XOR_A_A:
			//4 Clock Cycles
			xor8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case OR_B_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case OR_C_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break;

		case OR_D_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case OR_E_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case OR_H_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;
			break;

		case OR_L_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case OR_adrHL_A:
			//8 Clock Cycles
			or8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 4;
			break;

		case OR_A_A:
			//4 Clock Cycles
			or8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case CMP_B_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, BC.hi);
			timer_update_cnt += 4;
			break;

		case CMP_C_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, BC.lo);
			timer_update_cnt += 4;
			break;

		case CMP_D_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, DE.hi);
			timer_update_cnt += 4;
			break;

		case CMP_E_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, DE.lo);
			timer_update_cnt += 4;
			break;

		case CMP_H_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, HL.hi);
			timer_update_cnt += 4;
			break;

		case CMP_L_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, HL.lo);
			timer_update_cnt += 4;
			break;

		case CMP_adrHL_A:
			//8 Clock Cycles
			cmp8BitRegister(AF.hi, memory->readMemory(HL.data));
			timer_update_cnt += 8;
			break;

		case CMP_A_A:
			//4 Clock Cycles
			cmp8BitRegister(AF.hi, AF.hi);
			timer_update_cnt += 4;
			break;

		case RET_NOT_ZERO:
			//20/8 Clock Cycles
			
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				PC = temp->data;
				
				timer_update_cnt += 20;
			}
			else
				timer_update_cnt += 8;
			break;

		case POP_BC:
			//12 Clock Cycles
			BC.lo = memory->readMemory(SP++);
			BC.hi = memory->readMemory(SP++);
			timer_update_cnt += 12;
			break;

		case JMP_NOT_ZERO:
			//16/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				
				PC = temp->data;
				timer_update_cnt += 16;
			}
			else
				timer_update_cnt += 12;
			break;

		case JMP:
			//16 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			
			PC = temp->data;
			timer_update_cnt += 16;
			break;

		case CALL_NOT_ZERO:
			//24/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
				timer_update_cnt += 24;
			}
			else
				timer_update_cnt += 12;
			break;

		case PUSH_BC:
			//16 clock cycles
			memory->writeMemory(--SP, BC.hi);
			memory->writeMemory(--SP, BC.lo);
			
			timer_update_cnt += 16;
			break;

		case ADD_IMM_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;

		case RST_0:
			//16 Clock Cycles
			temp->data = PC;
			
			memory->writeMemory(--SP, temp->hi);
			memory->writeMemory(--SP, temp->lo);
			PC = 0x0000;
			timer_update_cnt += 16;
			break;

		case RET_ZERO:
			//8 Clock Cycles if cc false else 20 clock cycles
			
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				
				PC = temp->data;
				timer_update_cnt += 20;
			}
			else
				timer_update_cnt += 8;
			break;

		case RETURN:
			//16 Clock Cycles
			temp->lo = memory->readMemory(SP++);
			temp->hi = memory->readMemory(SP++);
			
			PC = temp->data;
			timer_update_cnt += 16;
			break;

		case JMP_ZERO:
			//16/12 Clock cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				
				PC = temp->data;
				timer_update_cnt += 16;
			}
			else
				timer_update_cnt += 12;
			break;

		case EXT_OP:
			//4 Clock Cycles, this opcode is special, it allows for 16 bit opcodes
			timer_update_cnt += 4;
			switch (memory->readMemory(PC++))
			{
			case RLC_B:
				//4 clock Cycles
				rotateReg(true, false, BC.hi);
				timer_update_cnt += 4;
				break;

			case RLC_C:
				//4 clock Cycles
				rotateReg(true, false, BC.lo);
				timer_update_cnt += 4;
				break;

			case RLC_D:
				//4 clock Cycles
				rotateReg(true, false, DE.hi);
				timer_update_cnt += 4;
				break;

			case RLC_E:
				//4 clock Cycles
				rotateReg(true, false, DE.lo);
				timer_update_cnt += 4;
				break;

			case RLC_H:
				//4 clock Cycles
				rotateReg(true, false, HL.hi);
				timer_update_cnt += 4;
				break;

			case RLC_L:
				//4 clock Cycles
				rotateReg(true, false, HL.lo);
				timer_update_cnt += 4;
				break;

			case RLC_adrHL:
				//8 clock Cycles
				rotateReg(true, false, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case eRLC_A:
				//4 clock Cycles
				rotateReg(true, false, AF.hi);
				timer_update_cnt += 4;
				break;
				
			case RRC_B:
				//4 clock Cycles
				rotateReg(false, false, BC.hi);
				timer_update_cnt += 4;
				break;

			case RRC_C:
				//4 clock Cycles
				rotateReg(false, false, BC.lo);
				timer_update_cnt += 4;
				break;

			case RRC_D:
				//4 clock Cycles
				rotateReg(false, false, DE.hi);
				timer_update_cnt += 4;
				break;

			case RRC_E:
				//4 clock Cycles
				rotateReg(false, false, DE.lo);
				timer_update_cnt += 4;
				break;

			case RRC_H:
				//4 clock Cycles
				rotateReg(false, false, HL.hi);
				timer_update_cnt += 4;
				break;

			case RRC_L:
				//4 clock Cycles
				rotateReg(false, false, HL.lo);
				timer_update_cnt += 4;
				break;

			case RRC_adrHL:
				//8 clock Cycles
				rotateReg(false, false, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case eRRC_A:
				//4 clock Cycles
				rotateReg(false, false, AF.hi);
				timer_update_cnt += 4;
				break;

			case RL_B:
				//4 clock Cycles
				rotateReg(true, true, BC.hi);
				timer_update_cnt += 4;
				break;

			case RL_C:
				//4 clock Cycles
				rotateReg(true, true, BC.lo);
				timer_update_cnt += 4;
				break;

			case RL_D:
				//4 clock Cycles
				rotateReg(true, true, DE.hi);
				timer_update_cnt += 4;
				break;

			case RL_E:
				//4 clock Cycles
				rotateReg(true, true, DE.lo);
				timer_update_cnt += 4;
				break;

			case RL_H:
				//4 clock Cycles
				rotateReg(true, true, HL.hi);
				timer_update_cnt += 4;
				break;

			case RL_L:
				//4 clock Cycles
				rotateReg(true, true, HL.lo);
				timer_update_cnt += 4;
				break;

			case RL_adrHL:
				//8 clock Cycles
				rotateReg(true, true, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case eRL_A:
				//4 clock Cycles
				rotateReg(true, true, AF.hi);
				timer_update_cnt += 4;
				break;

			case RR_B:
				//4 clock Cycles
				rotateReg(false, true, BC.hi);
				timer_update_cnt += 4;
				break;

			case RR_C:
				//4 clock Cycles
				rotateReg(false, true, BC.lo);
				timer_update_cnt += 4;
				break;

			case RR_D:
				//4 clock Cycles
				rotateReg(false, true, DE.hi);
				timer_update_cnt += 4;
				break;

			case RR_E:
				//4 clock Cycles
				rotateReg(false, true, DE.lo);
				timer_update_cnt += 4;
				break;

			case RR_H:
				//4 clock Cycles
				rotateReg(false, true, HL.hi);
				timer_update_cnt += 4;
				break;

			case RR_L:
				//4 clock Cycles
				rotateReg(false, true, HL.lo);
				timer_update_cnt += 4;
				break;

			case RR_adrHL:
				//8 clock Cycles
				rotateReg(false, true, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case eRR_A:
				//4 clock Cycles
				rotateReg(false, true, AF.hi);
				timer_update_cnt += 4;
				break;

			case SLA_B:
				//4 clock cycles
				shiftReg(true, false, BC.hi);
				timer_update_cnt += 4;
				break;

			case SLA_C:
				//4 clock cycles
				shiftReg(true, false, BC.lo);
				timer_update_cnt += 4;
				break;

			case SLA_D:
				//4 clock cycles
				shiftReg(true, false, DE.hi);
				timer_update_cnt += 4;
				break;

			case SLA_E:
				//4 clock cycles
				shiftReg(true, false, DE.lo);
				timer_update_cnt += 4;
				break;

			case SLA_H:
				//4 clock cycles
				shiftReg(true, false, HL.hi);
				timer_update_cnt += 4;
				break;

			case SLA_L:
				//4 clock cycles
				shiftReg(true, false, HL.lo);
				timer_update_cnt += 4;
				break;

			case SLA_adrHL:
				//8 Clock Cycles
				shiftReg(true, false, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SLA_A:
				//8 Clock Cycles
				shiftReg(true, false, AF.hi);
				timer_update_cnt += 4;
				break;

			case SRA_B:
				//4 clock Cycles
				shiftReg(false, true, BC.hi);
				timer_update_cnt += 4;
				break;

			case SRA_C:
				//4 clock Cycles
				shiftReg(false, true, BC.lo);
				timer_update_cnt += 4;
				break;

			case SRA_D:
				//4 clock Cycles
				shiftReg(false, true, DE.hi);
				timer_update_cnt += 4;
				break;

			case SRA_E:
				//4 clock Cycles
				shiftReg(false, true, DE.lo);
				timer_update_cnt += 4;
				break;

			case SRA_H:
				//4 clock Cycles
				shiftReg(false, true, HL.hi);
				timer_update_cnt += 4;
				break;

			case SRA_L:
				//4 clock Cycles
				shiftReg(false, true, HL.lo);
				timer_update_cnt += 4;
				break;

			case SRA_adrHL:
				//8 clock Cycles
				shiftReg(false, true, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SRA_A:
				//4 clock Cycles
				shiftReg(false, true, AF.hi);
				timer_update_cnt += 4;
				break;

			case SWAP_B:
				//4 Clock Cycles
				swapReg(BC.hi);
				timer_update_cnt += 4;
				break;

			case SWAP_C:
				//4 Clock Cycles
				swapReg(BC.lo);
				timer_update_cnt += 4;
				break;

			case SWAP_D:
				//4 Clock Cycles
				swapReg(DE.hi);
				timer_update_cnt += 4;
				break;

			case SWAP_E:
				//4 Clock Cycles
				swapReg(DE.lo);
				timer_update_cnt += 4;
				break;

			case SWAP_H:
				//4 Clock Cycles
				swapReg(HL.hi);
				timer_update_cnt += 4;
				break;

			case SWAP_L:
				//4 Clock Cycles
				swapReg(HL.lo);
				timer_update_cnt += 4;
				break;

			case SWAP_adrHL:
				//8 Clock Cycles
				swapReg(memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SWAP_A:
				//4 Clock Cycles
				swapReg(AF.hi);
				timer_update_cnt += 4;
				break;

			case SRL_B:
				//4 Clock Cycles
				shiftReg(false, false, BC.hi);
				timer_update_cnt += 4;
				break;

			case SRL_C:
				//4 Clock Cycles
				shiftReg(false, false, BC.lo);
				timer_update_cnt += 4;
				break;

			case SRL_D:
				//4 Clock Cycles
				shiftReg(false, false, DE.hi);
				timer_update_cnt += 4;
				break;

			case SRL_E:
				//4 Clock Cycles
				shiftReg(false, false, DE.lo);
				timer_update_cnt += 4;
				break;

			case SRL_H:
				//4 Clock Cycles
				shiftReg(false, false, HL.hi);
				timer_update_cnt += 4;
				break;

			case SRL_L:
				//4 Clock Cycles
				shiftReg(false, false, HL.lo);
				timer_update_cnt += 4;
				break;

			case SRL_adrHL:
				//8 Clock Cycles
				shiftReg(false, false, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SRL_A:
				//4 Clock Cycles
				shiftReg(false, false, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_1_B:
				//4 Clock Cycles
				test_bit(0, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_1_C:
				//4 Clock Cycles
				test_bit(0, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_1_D:
				//4 Clock Cycles
				test_bit(0, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_1_E:
				//4 Clock Cycles
				test_bit(0, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_1_H:
				//4 Clock Cycles
				test_bit(0, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_1_L:
				//4 Clock Cycles
				test_bit(0, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_1_adrHL:
				//8 Clock Cycles
				test_bit(0, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_1_A:
				//4 Clock Cycles
				test_bit(0, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_2_B:
				//4 Clock Cycles
				test_bit(1, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_2_C:
				//4 Clock Cycles
				test_bit(1, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_2_D:
				//4 Clock Cycles
				test_bit(1, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_2_E:
				//4 Clock Cycles
				test_bit(1, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_2_H:
				//4 Clock Cycles
				test_bit(1, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_2_L:
				//4 Clock Cycles
				test_bit(1, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_2_adrHL:
				//8 Clock Cycles
				test_bit(1, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_2_A:
				//4 Clock Cycles
				test_bit(1, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_3_B:
				//4 Clock Cycles
				test_bit(2, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_3_C:
				//4 Clock Cycles
				test_bit(2, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_3_D:
				//4 Clock Cycles
				test_bit(2, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_3_E:
				//4 Clock Cycles
				test_bit(2, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_3_H:
				//4 Clock Cycles
				test_bit(2, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_3_L:
				//4 Clock Cycles
				test_bit(2, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_3_adrHL:
				//8 Clock Cycles
				test_bit(2, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_3_A:
				//4 Clock Cycles
				test_bit(2, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_4_B:
				//4 Clock Cycles
				test_bit(3, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_4_C:
				//4 Clock Cycles
				test_bit(3, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_4_D:
				//4 Clock Cycles
				test_bit(3, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_4_E:
				//4 Clock Cycles
				test_bit(3, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_4_H:
				//4 Clock Cycles
				test_bit(3, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_4_L:
				//4 Clock Cycles
				test_bit(3, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_4_adrHL:
				//8 Clock Cycles
				test_bit(3, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_4_A:
				//4 Clock Cycles
				test_bit(3, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_5_B:
				//4 Clock Cycles
				test_bit(4, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_5_C:
				//4 Clock Cycles
				test_bit(4, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_5_D:
				//4 Clock Cycles
				test_bit(4, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_5_E:
				//4 Clock Cycles
				test_bit(4, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_5_H:
				//4 Clock Cycles
				test_bit(4, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_5_L:
				//4 Clock Cycles
				test_bit(4, HL.lo);
				timer_update_cnt += 4;
				break;
			
			case BIT_5_adrHL:
				//8 Clock Cycles
				test_bit(4, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_5_A:
				//4 Clock Cycles
				test_bit(4, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_6_B:
				//4 Clock Cycles
				test_bit(5, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_6_C:
				//4 Clock Cycles
				test_bit(5, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_6_D:
				//4 Clock Cycles
				test_bit(5, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_6_E:
				//4 Clock Cycles
				test_bit(5, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_6_H:
				//4 Clock Cycles
				test_bit(5, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_6_L:
				//4 Clock Cycles
				test_bit(5, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_6_adrHL:
				//8 Clock Cycles
				test_bit(5, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;
				
			case BIT_6_A:
				//4 Clock Cycles
				test_bit(5, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_7_B:
				//4 Clock Cycles
				test_bit(6, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_7_C:
				//4 Clock Cycles
				test_bit(6, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_7_D:
				//4 Clock Cycles
				test_bit(6, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_7_E:
				//4 Clock Cycles
				test_bit(6, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_7_H:
				//4 Clock Cycles
				test_bit(6, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_7_L:
				//4 Clock Cycles
				test_bit(6, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_7_adrHL:
				//8 Clock Cycles
				test_bit(6, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_7_A:
				//4 Clock Cycles
				test_bit(6, AF.hi);
				timer_update_cnt += 4;
				break;

			case BIT_8_B:
				//4 Clock Cycles
				test_bit(7, BC.hi);
				timer_update_cnt += 4;
				break;

			case BIT_8_C:
				//4 Clock Cycles
				test_bit(7, BC.lo);
				timer_update_cnt += 4;
				break;

			case BIT_8_D:
				//4 Clock Cycles
				test_bit(7, DE.hi);
				timer_update_cnt += 4;
				break;

			case BIT_8_E:
				//4 Clock Cycles
				test_bit(7, DE.lo);
				timer_update_cnt += 4;
				break;

			case BIT_8_H:
				//4 Clock Cycles
				test_bit(7, HL.hi);
				timer_update_cnt += 4;
				break;

			case BIT_8_L:
				//4 Clock Cycles
				test_bit(7, HL.lo);
				timer_update_cnt += 4;
				break;

			case BIT_8_adrHL:
				//8 Clock Cycles
				test_bit(7, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case BIT_8_A:
				//4 Clock Cycles
				test_bit(7, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_1_B:
				//4 Clock Cycles
				reset_bit(0, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_1_C:
				//4 Clock Cycles
				reset_bit(0, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_1_D:
				//4 Clock Cycles
				reset_bit(0, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_1_E:
				//4 Clock Cycles
				reset_bit(0, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_1_H:
				//4 Clock Cycles
				reset_bit(0, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_1_L:
				//4 Clock Cycles
				reset_bit(0, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_1_adrHL:
				//8 Clock Cycles
				reset_bit(0, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_1_A:
				//4 Clock Cycles
				reset_bit(0, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_2_B:
				//4 Clock Cycles
				reset_bit(1, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_2_C:
				//4 Clock Cycles
				reset_bit(1, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_2_D:
				//4 Clock Cycles
				reset_bit(1, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_2_E:
				//4 Clock Cycles
				reset_bit(1, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_2_H:
				//4 Clock Cycles
				reset_bit(1, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_2_L:
				//4 Clock Cycles
				reset_bit(1, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_2_adrHL:
				//8 Clock Cycles
				reset_bit(1, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_2_A:
				//4 Clock Cycles
				reset_bit(1, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_3_B:
				//4 Clock Cycles
				reset_bit(2, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_3_C:
				//4 Clock Cycles
				reset_bit(2, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_3_D:
				//4 Clock Cycles
				reset_bit(2, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_3_E:
				//4 Clock Cycles
				reset_bit(2, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_3_H:
				//4 Clock Cycles
				reset_bit(2, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_3_L:
				//4 Clock Cycles
				reset_bit(2, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_3_adrHL:
				//8 Clock Cycles
				reset_bit(2, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_3_A:
				//4 Clock Cycles
				reset_bit(2, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_4_B:
				//4 Clock Cycles
				reset_bit(3, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_4_C:
				//4 Clock Cycles
				reset_bit(3, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_4_D:
				//4 Clock Cycles
				reset_bit(3, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_4_E:
				//4 Clock Cycles
				reset_bit(3, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_4_H:
				//4 Clock Cycles
				reset_bit(3, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_4_L:
				//4 Clock Cycles
				reset_bit(3, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_4_adrHL:
				//8 Clock Cycles
				reset_bit(3, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_4_A:
				//4 Clock Cycles
				reset_bit(3, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_5_B:
				//4 Clock Cycles
				reset_bit(4, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_5_C:
				//4 Clock Cycles
				reset_bit(4, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_5_D:
				//4 Clock Cycles
				reset_bit(4, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_5_E:
				//4 Clock Cycles
				reset_bit(4, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_5_H:
				//4 Clock Cycles
				reset_bit(4, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_5_L:
				//4 Clock Cycles
				reset_bit(4, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_5_adrHL:
				//8 Clock Cycles
				reset_bit(4, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_5_A:
				//4 Clock Cycles
				reset_bit(4, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_6_B:
				//4 Clock Cycles
				reset_bit(5, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_6_C:
				//4 Clock Cycles
				reset_bit(5, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_6_D:
				//4 Clock Cycles
				reset_bit(5, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_6_E:
				//4 Clock Cycles
				reset_bit(5, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_6_H:
				//4 Clock Cycles
				reset_bit(5, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_6_L:
				//4 Clock Cycles
				reset_bit(5, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_6_adrHL:
				//8 Clock Cycles
				reset_bit(5, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_6_A:
				//4 Clock Cycles
				reset_bit(5, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_7_B:
				//4 Clock Cycles
				reset_bit(6, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_7_C:
				//4 Clock Cycles
				reset_bit(6, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_7_D:
				//4 Clock Cycles
				reset_bit(6, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_7_E:
				//4 Clock Cycles
				reset_bit(6, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_7_H:
				//4 Clock Cycles
				reset_bit(6, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_7_L:
				//4 Clock Cycles
				reset_bit(6, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_7_adrHL:
				//8 Clock Cycles
				reset_bit(6, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_7_A:
				//4 Clock Cycles
				reset_bit(6, AF.hi);
				timer_update_cnt += 4;
				break;

			case RES_8_B:
				//4 Clock Cycles
				reset_bit(7, BC.hi);
				timer_update_cnt += 4;
				break;

			case RES_8_C:
				//4 Clock Cycles
				reset_bit(7, BC.lo);
				timer_update_cnt += 4;
				break;

			case RES_8_D:
				//4 Clock Cycles
				reset_bit(7, DE.hi);
				timer_update_cnt += 4;
				break;

			case RES_8_E:
				//4 Clock Cycles
				reset_bit(7, DE.lo);
				timer_update_cnt += 4;
				break;

			case RES_8_H:
				//4 Clock Cycles
				reset_bit(7, HL.hi);
				timer_update_cnt += 4;
				break;

			case RES_8_L:
				//4 Clock Cycles
				reset_bit(7, HL.lo);
				timer_update_cnt += 4;
				break;

			case RES_8_adrHL:
				//8 Clock Cycles
				reset_bit(7, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case RES_8_A:
				//4 Clock Cycles
				reset_bit(7, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_1_B:
				//4 Clock Cycles
				set_bit(0, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_1_C:
				//4 Clock Cycles
				set_bit(0, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_1_D:
				//4 Clock Cycles
				set_bit(0, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_1_E:
				//4 Clock Cycles
				set_bit(0, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_1_H:
				//4 Clock Cycles
				set_bit(0, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_1_L:
				//4 Clock Cycles
				set_bit(0, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_1_adrHL:
				//8 Clock Cycles
				set_bit(0, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_1_A:
				//4 Clock Cycles
				set_bit(0, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_2_B:
				//4 Clock Cycles
				set_bit(1, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_2_C:
				//4 Clock Cycles
				set_bit(1, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_2_D:
				//4 Clock Cycles
				set_bit(1, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_2_E:
				//4 Clock Cycles
				set_bit(1, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_2_H:
				//4 Clock Cycles
				set_bit(1, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_2_L:
				//4 Clock Cycles
				set_bit(1, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_2_adrHL:
				//8 Clock Cycles
				set_bit(1, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_2_A:
				//4 Clock Cycles
				set_bit(1, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_3_B:
				//4 Clock Cycles
				set_bit(2, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_3_C:
				//4 Clock Cycles
				set_bit(2, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_3_D:
				//4 Clock Cycles
				set_bit(2, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_3_E:
				//4 Clock Cycles
				set_bit(2, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_3_H:
				//4 Clock Cycles
				set_bit(2, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_3_L:
				//4 Clock Cycles
				set_bit(2, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_3_adrHL:
				//8 Clock Cycles
				set_bit(2, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_3_A:
				//4 Clock Cycles
				set_bit(2, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_4_B:
				//4 Clock Cycles
				set_bit(3, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_4_C:
				//4 Clock Cycles
				set_bit(3, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_4_D:
				//4 Clock Cycles
				set_bit(3, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_4_E:
				//4 Clock Cycles
				set_bit(3, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_4_H:
				//4 Clock Cycles
				set_bit(3, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_4_L:
				//4 Clock Cycles
				set_bit(3, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_4_adrHL:
				//8 Clock Cycles
				set_bit(3, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_4_A:
				//4 Clock Cycles
				set_bit(3, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_5_B:
				//4 Clock Cycles
				set_bit(4, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_5_C:
				//4 Clock Cycles
				set_bit(4, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_5_D:
				//4 Clock Cycles
				set_bit(4, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_5_E:
				//4 Clock Cycles
				set_bit(4, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_5_H:
				//4 Clock Cycles
				set_bit(4, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_5_L:
				//4 Clock Cycles
				set_bit(4, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_5_adrHL:
				//8 Clock Cycles
				set_bit(4, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_5_A:
				//4 Clock Cycles
				set_bit(4, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_6_B:
				//4 Clock Cycles
				set_bit(5, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_6_C:
				//4 Clock Cycles
				set_bit(5, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_6_D:
				//4 Clock Cycles
				set_bit(5, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_6_E:
				//4 Clock Cycles
				set_bit(5, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_6_H:
				//4 Clock Cycles
				set_bit(5, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_6_L:
				//4 Clock Cycles
				set_bit(5, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_6_adrHL:
				//8 Clock Cycles
				set_bit(5, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_6_A:
				//4 Clock Cycles
				set_bit(5, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_7_B:
				//4 Clock Cycles
				set_bit(6, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_7_C:
				//4 Clock Cycles
				set_bit(6, BC.lo);
				timer_update_cnt += 4;
				break;

			case SET_7_D:
				//4 Clock Cycles
				set_bit(6, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_7_E:
				//4 Clock Cycles
				set_bit(6, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_7_H:
				//4 Clock Cycles
				set_bit(6, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_7_L:
				//4 Clock Cycles
				set_bit(6, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_7_adrHL:
				//8 Clock Cycles
				set_bit(6, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_7_A:
				//4 Clock Cycles
				set_bit(6, AF.hi);
				timer_update_cnt += 4;
				break;

			case SET_8_B:
				//4 Clock Cycles
				set_bit(7, BC.hi);
				timer_update_cnt += 4;
				break;

			case SET_8_C:
				//4 Clock Cycles
				set_bit(7, BC.lo);
				timer_update_cnt += 4;
				break;
				
			case SET_8_D:
				//4 Clock Cycles
				set_bit(7, DE.hi);
				timer_update_cnt += 4;
				break;

			case SET_8_E:
				//4 Clock Cycles
				set_bit(7, DE.lo);
				timer_update_cnt += 4;
				break;

			case SET_8_H:
				//4 Clock Cycles
				set_bit(7, HL.hi);
				timer_update_cnt += 4;
				break;

			case SET_8_L:
				//4 Clock Cycles
				set_bit(7, HL.lo);
				timer_update_cnt += 4;
				break;

			case SET_8_adrHL:
				//8 Clock Cycles
				set_bit(7, memory->readMemory(HL.data));
				timer_update_cnt += 8;
				break;

			case SET_8_A:
				//4 Clock Cycles
				set_bit(7, AF.hi);
				timer_update_cnt += 4;
				break;

			default:
				break;
			}
			break;

		case CALL_ZERO:
			//24/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
				timer_update_cnt += 24;
			}
			else
				timer_update_cnt += 12;
			break;

		case CALL:
			//24 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			
			Register temp2;
			temp2.data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = temp->data;
			timer_update_cnt += 24;
			break;

		case ADC_8IMM_A:
			//8 Clock Cycles
			add8BitRegister(AF.hi, memory->readMemory(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 8;
			break;

		case RST_8:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0008;
			
			timer_update_cnt += 16;
			break;

		case RET_NOCARRY:
			//20/8 Clock Cycles
			
			if (!CPU_FLAG_BIT_TEST(C_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				PC = temp->data;
				
				timer_update_cnt += 20;
			}
			else
				timer_update_cnt += 8;
			break;

		case POP_DE:
			//12 Clock Cycles
			DE.lo = memory->readMemory(SP++);
			DE.hi = memory->readMemory(SP++);
			timer_update_cnt += 12;
			break;

		case JMP_NOCARRY:
			//16/12 Clock cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(C_FLAG))
			{
				
				PC = temp->data;
				timer_update_cnt += 16;
			}
			else
				timer_update_cnt += 12;
			break;

		case CALL_NOCARRY:
			//24/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(C_FLAG))
			{
				
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
				timer_update_cnt += 24;
			}
			else
				timer_update_cnt += 12;
			break;

		case PUSH_DE:
			//16 clock cycles
			memory->writeMemory(--SP, DE.hi);
			memory->writeMemory(--SP, DE.lo);
			
			timer_update_cnt += 16;
			break;

		case SUB_8IMM_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;

		case RST_10:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0010;
			
			timer_update_cnt += 16;
			break;

		case RET_CARRY:
			//20/8 Clock Cycles
			
			if (CPU_FLAG_BIT_TEST(C_FLAG))
			{
				temp->lo = memory->readMemory(SP++);
				temp->hi = memory->readMemory(SP++);
				PC = temp->data;
				
				timer_update_cnt += 20;
			}else
				timer_update_cnt += 8;
			break;

		case RET_INT:
			//16 Clock Cycles
			temp->lo = memory->readMemory(SP++);
			temp->hi = memory->readMemory(SP++);
			PC = temp->data;
			byte = 0xFF;
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, byte);
			timer_update_cnt += 16;
			break;

		case JMP_CARRY:
			//16/12 Clock cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(C_FLAG))
			{
				
				PC = temp->data;
				timer_update_cnt += 16;
			}
			else
				timer_update_cnt += 12;
			break;

		case CALL_CARRY:
			//24/12 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			if (CPU_FLAG_BIT_TEST(C_FLAG))
			{
				
				Register temp2;
				temp2.data = PC;
				memory->writeMemory(--SP, temp2.hi);
				memory->writeMemory(--SP, temp2.lo);
				PC = temp->data;
				timer_update_cnt += 24;
			}
			else
				timer_update_cnt += 12;
			break;

		case SBC_8IMM_A:
			//8 Clock Cycles
			sub8BitRegister(AF.hi, memory->readMemory(PC++), CPU_FLAG_BIT_TEST(C_FLAG));
			timer_update_cnt += 8;
			break;

		case RST_18:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0018;
			
			timer_update_cnt += 16;
			break;

		case LDH_A_IMMadr:
			//12 Clock Cycles
			memory->writeMemory((0xFF00 + memory->readMemory(PC++)), AF.hi);
			timer_update_cnt += 12;
			break;

		case POP_HL:
			//12 Clock Cycles
			HL.lo = memory->readMemory(SP++);
			HL.hi = memory->readMemory(SP++);
			timer_update_cnt += 12;
			break;

		case LDH_A_C:
			//8 Clock Cycles
			memory->writeMemory((0xFF00 + BC.lo), AF.hi);
			timer_update_cnt += 8;
			break;

		case PUSH_HL:
			//16 clock cycles
			memory->writeMemory(--SP, HL.hi);
			memory->writeMemory(--SP, HL.lo);
			
			timer_update_cnt += 16;
			break;

		case AND_8IMM_A:
			//8 Clock Cycles
			and8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;

		case RST_20:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0020;
			
			timer_update_cnt += 16;
			break;

		case ADD_SIMM_SP:
			//16 Clock Cycles
			byte = memory->readMemory(PC++);
			if (TestBitInByte(byte, 7))
			{
				if (checkBorrowFromBit_Byte(5, SP, byte))
					CPU_FLAG_BIT_SET(H_FLAG);
				SP = SP - twoComp_Byte(byte);
			}
			else
			{
				if (checkCarryFromBit_Byte(3,SP,byte))
					CPU_FLAG_BIT_SET(H_FLAG);
				if (checkCarryFromBit_Byte(7, SP, byte))
					CPU_FLAG_BIT_SET(C_FLAG);
				else
					CPU_FLAG_BIT_RESET(C_FLAG);
				SP = SP + byte;
			}
			CPU_FLAG_BIT_RESET(Z_FLAG);
			CPU_FLAG_BIT_RESET(N_FLAG);
			
			
			
			timer_update_cnt += 16;
			break;

		case JMP_adrHL:
			//4 Clock Cycles
			PC = HL.data;
			timer_update_cnt += 4;
			break;

		case LD_A_adr:
			//16 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			memory->writeMemory(temp->data, AF.hi);
			timer_update_cnt += 16;
			break;

		case XOR_8IMM_A:
			//8 Clock Cycles
			xor8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;
			
		case RST_28:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0028;
			
			timer_update_cnt += 16;
			break;

		case LDH_IMMadr_A:
			//12 Clock Cycles
			AF.hi = memory->readMemory(0xFF00 + memory->readMemory(PC++));
			timer_update_cnt += 12;
			break;

		case POP_AF:
			//12 Clock Cycles
			AF.lo = memory->readMemory(SP++);
			AF.hi = memory->readMemory(SP++);
			timer_update_cnt += 12;
			break;

		case DISABLE_INT:
			//4 Clock Cycles
			byte = 0x00;
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, byte);
			timer_update_cnt += 4;
			break;

		case PUSH_AF:
			//16 clock cycles
			memory->writeMemory(--SP, AF.hi);
			memory->writeMemory(--SP, AF.lo);
			timer_update_cnt += 16;
			break;

		case OR_8IMM_A:
			//8 Clock Cycles
			or8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;

		case RST_30:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0030;
			timer_update_cnt += 16;
			break;

		case LDHL_S_8IMM_SP_HL:
			//12 Clock Cycles
			add16BitRegister(HL.data, (SP + memory->readMemory(PC++)));
			CPU_FLAG_BIT_RESET(Z_FLAG);
			timer_update_cnt += 12;
			break;

		case LD_HL_SP:
			//8 Clock Cycles
			SP = HL.data;
			timer_update_cnt += 8;
			break;

		case LD_16adr_A:
			//16 Clock Cycles
			temp->lo = memory->readMemory(PC++);
			temp->hi = memory->readMemory(PC++);
			AF.hi = memory->readMemory(temp->data);
			timer_update_cnt += 16;
			break;

		case ENABLE_INT:
			//4 Clock Cycles
			byte = 0xFF;
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, byte);
			timer_update_cnt += 4;
			break;

		case CMP_8IMM_A:
			//8 Clock Cycles
			cmp8BitRegister(AF.hi, memory->readMemory(PC++));
			timer_update_cnt += 8;
			break;

		case RST_38:
			//16 Clock Cycles
			temp->data = PC;
			memory->writeMemory(--SP, temp2.hi);
			memory->writeMemory(--SP, temp2.lo);
			PC = 0x0038;
			timer_update_cnt += 16;
			break;

		default:
			break;
		}
        delete temp;
		return timer_update_cnt;
	}

	void CPU::stop()
	{
		this->_cpuRunning = false;
	}

	void CPU::increment16BitRegister(uWORD& reg)
	{
		++reg;
	}

	void CPU::increment8BitRegister(uBYTE& reg)
	{
		++reg;
		if (reg == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Byte(2, reg, 0x01))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);
	}

	void CPU::decrement8BitRegister(uBYTE& reg)
	{
		--reg;
		if (reg == 0x0)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);

		if (checkBorrowFromBit_Byte(3, reg, 0x01))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);
	}

	void CPU::decrement16BitRegister(uWORD& reg)
	{
		
		--reg;
	}

	void CPU::add16BitRegister(uWORD& host, uWORD operand)
	{
		
		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Word(10, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);

		if (host > 0xFFFF - operand)
			CPU_FLAG_BIT_SET(C_FLAG);
		else
			CPU_FLAG_BIT_RESET(C_FLAG);
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

	bool CPU::checkCarryFromBit_Word(int pos, uWORD word, uWORD addedWord)
	{
		std::bitset<sizeof(uWORD) * 8> BitField(word);
		std::bitset<sizeof(uWORD) * 8> AddedBitField(addedWord);
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

	bool CPU::checkBorrowFromBit_Word(int pos, uWORD word, uWORD subtractedWord)
	{
		std::bitset<sizeof(uWORD) * 8> BitField(word);
		std::bitset<sizeof(uWORD) * 8> SubBitField(subtractedWord);
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

	uWORD CPU::twoComp_Word(uWORD word)
	{
		std::bitset<16> twoCompWord(word);

		twoCompWord.flip();

		uWORD result = twoCompWord.to_ulong() + 0x01;

		return result;
	}

	void CPU::add8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host + operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Byte(2, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);

		if (host > (0xFF - operand))
			CPU_FLAG_BIT_SET(C_FLAG);
		else
			CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::add8BitRegister(uBYTE& host, uBYTE operand, bool carry)
	{
		host = host + operand + carry;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_RESET(N_FLAG);

		if (checkCarryFromBit_Byte(2, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);

		if (host > (0xFF - carry - operand))
			CPU_FLAG_BIT_SET(C_FLAG);
		else
			CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::sub8BitRegister(uBYTE& host, uBYTE operand)
	{
		host = host - operand;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);
		
		if (!checkBorrowFromBit_Byte(3, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);

		if (host < operand)
			CPU_FLAG_BIT_SET(C_FLAG);
		else
			CPU_FLAG_BIT_RESET(C_FLAG);
	}

	void CPU::sub8BitRegister(uBYTE& host, uBYTE operand, bool carry)
	{
		host = host - operand - carry;

		if (host == 0x00)
			CPU_FLAG_BIT_SET(Z_FLAG);
		else
			CPU_FLAG_BIT_RESET(Z_FLAG);

		CPU_FLAG_BIT_SET(N_FLAG);

		if (!checkBorrowFromBit_Byte(3, host, operand))
			CPU_FLAG_BIT_SET(H_FLAG);
		else
			CPU_FLAG_BIT_RESET(H_FLAG);

		if (host < (operand + carry))
			CPU_FLAG_BIT_SET(C_FLAG);
		else
			CPU_FLAG_BIT_RESET(C_FLAG);
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

		if (!checkBorrowFromBit_Byte(3, host, operand))
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
        delete FlagBits;
        FlagBits = new std::bitset<8>(AF.lo);
		FlagBits->set(flag);
		AF.lo = (uBYTE)FlagBits->to_ulong();
	}

	void CPU::Flag_reset(int flag)
	{
        delete FlagBits;
        FlagBits = new std::bitset<8>(AF.lo);
		FlagBits->reset(flag);
		AF.lo = (uBYTE)FlagBits->to_ulong();
	}

	bool CPU::Flag_test(int flag)
	{
        delete FlagBits;
        FlagBits = new std::bitset<8>(AF.lo);
		return FlagBits->test(flag);
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

	void CPU::checkInterupts()
	{
		std::bitset<8> IME(memory->DMA_read(INTERUPT_EN_REGISTER_ADR));
		std::bitset<8> IF(memory->DMA_read(INTERUPT_FLAG_REG));
		Register Temp;

		if (IF[0] && IME[0]) //V-Blank
		{
			IME.reset();
            IF.reset(0);
            memory->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
			memory->writeMemory(INTERUPT_EN_REGISTER_ADR, (uBYTE)IME.to_ulong());
			Temp.data = PC;
			memory->DMA_write(--SP, Temp.hi);
			memory->DMA_write(--SP, Temp.lo);
			PC = VBLANK_INT;
		}
		else if (IF[1] && IME[1]) // LCDC
		{
			IME.reset();
            IF.reset(1);
            memory->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, (uBYTE)IME.to_ulong());
			Temp.data = PC;
			memory->DMA_write(--SP, Temp.hi);
			memory->DMA_write(--SP, Temp.lo);
			PC = LCDC_INT;
		}
		else if (IF[2] && IME[2]) // Timer Overflow
		{
			IME.reset();
            IF.reset(2);
            memory->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, (uBYTE)IME.to_ulong());
			Temp.data = PC;
			memory->DMA_write(--SP, Temp.hi);
			memory->DMA_write(--SP, Temp.lo);
			PC = TIMER_OVER_INT;
		}
		else if (IF[3] && IME[3]) // Serial I/O Complete
		{
			IME.reset();
            IF.reset(3);
            memory->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, (uBYTE)IME.to_ulong());
			Temp.data = PC;
			memory->DMA_write(--SP, Temp.hi);
			memory->DMA_write(--SP, Temp.lo);
			PC = SER_TRF_INT;
		}
		else if (IF[4] && IME[4]) //Pin 10 - 13 hi to lo (Control Input)
		{
			IME.reset();
            IF.reset(4);
            memory->writeMemory(INTERUPT_FLAG_REG, (uBYTE)IF.to_ulong());
			memory->DMA_write(INTERUPT_EN_REGISTER_ADR, (uBYTE)IME.to_ulong());
			Temp.data = PC;
			memory->DMA_write(--SP, Temp.hi);
			memory->DMA_write(--SP, Temp.lo);
			PC = CONTROL_INT;
		}
	}

	void CPU::updateTimers(int cycles)
	{
        std::bitset<8> TMC(memory->readMemory(0xFF07));
        std::bitset<8> TMA(memory->readMemory(0xFF06));
        std::bitset<8> TIMA(memory->readMemory(0xFF05));
        
        this->updateDivider(cycles);
        
        if(TMC.test(2)) //Check if clock is enabled
        {
            memory->timer_counter -= cycles;
            
            if(memory->timer_counter <= 0)
            {
                uBYTE frequency = memory->readMemory(0xFF07) & 0x03;
                switch(frequency)
                {
                    case 0: memory->timer_counter = 1024; break;
                    case 1: memory->timer_counter = 16; break;
                    case 2: memory->timer_counter = 64; break;
                    case 3: memory->timer_counter = 256; break;
                }
                
                //Timer Overflow
                if(memory->readMemory(0xFF05) == 255)
                {
                    memory->writeMemory(0xFF05, memory->readMemory(0xFF06));
                    memory->RequestInterupt(2);
                }
                else
                    memory->writeMemory(0xFF05, memory->readMemory(0xFF05)+1);
            }
        }
        
	}

    void CPU::updateDivider(int cycles)
    {
        divider_count += cycles;
        if(divider_count >= 255)
        {
            memory->DMA_write(0xFF04, memory->DMA_read(0xFF04)+1);
            divider_count = 0;
        }
    }
    
	void CPU::halt()
	{
		std::bitset<8> IME(memory->DMA_read(INTERUPT_EN_REGISTER_ADR));
		std::bitset<8> IF(memory->DMA_read(INTERUPT_FLAG_REG));
		bool interuptHasOccured = false;
		while (!interuptHasOccured)
		{
			if (IF[0] && IME[0]) //V-Blank
			{
				interuptHasOccured = true;
			}
			else if (IF[1] && IME[1]) // LCDC
			{
				interuptHasOccured = true;
			}
			else if (IF[2] && IME[2]) // Timer Overflow
			{
				interuptHasOccured = true;
			}
			else if (IF[3] && IME[3]) // Serial I/O Complete
			{
				interuptHasOccured = true;
			}
			else if (IF[4] && IME[4]) //Pin 10 - 13 hi to lo (Control Input)
			{
				interuptHasOccured = true;
			}
		}
	}
    
    
}
