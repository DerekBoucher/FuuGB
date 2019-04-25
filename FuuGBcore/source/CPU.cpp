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
		uBYTE byte = 0x0;
		uBYTE SP_data = 0x0;
		Register* temp = new Register();
		switch (memory->readMemory(PC++))
		{
		case NOP:
			//1 CPU Cycle
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS));
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
			if (BC.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_B:
			//4 CPU Cycles
			decrement8BitRegister(BC.hi);
			if (BC.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
			break;

		case LD_8IMM_B:
			//8 CPU Cycles
			BC.hi = memory->readMemory(PC++);
			break;

		case RLC_A:
			//4 CPU Cycles
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
			AF.lo = AF.lo | (AF.hi & 0x10);
			AF.lo = AF.lo & 0x10;
			AF.hi = AF.hi << 0x1;
			if (AF.hi == 0)
				AF.lo = AF.lo | 0x8;
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
			if (BC.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_C:
			//4 CPU Cycles
			decrement8BitRegister(BC.lo);
			if (BC.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
			break;

		case LD_8IMM_C:
			//8 CPU Cycles
			BC.lo = memory->readMemory(PC++);
			break;

		case RRC_A:
			//4 CPU Cycles
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(0));
			AF.hi >> 1;
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
			if (DE.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_D:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			if (DE.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
			break;

		case LD_8IMM_D:
			//8 Clock Cycles
			DE.hi = memory->readMemory(PC++);
			break;

		case RL_A:
			//4 Clock Cycles
			oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(7));
			AF.hi << 1;
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
			if (DE.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_E:
			//4 clock cycles
			decrement8BitRegister(DE.lo);
			if (DE.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
			break;

		case LD_8IMM_E:
			//8 Clock Cycles
			DE.lo = memory->readMemory(PC++);
			break;

		case RR_A:
			//4 clock cycles
			oldCarry = CPU_FLAG_BIT_TEST(C_FLAG);
			CPU_FLAG_BIT_SET(C_FLAG, ALU_BIT_TEST(0));
			AF.hi >> 1;
			ALU_BIT_SET(7, oldCarry);
			break;

		case RJmp_NOTZERO:
			//8 Clock Cycles
			byte = memory->readMemory(PC++);
			if (!CPU_FLAG_BIT_TEST(Z_FLAG))
			{
				if (TestBitInByte(byte, 7))
					PC = PC - (byte & 0x7F);
				else
					PC = PC + (byte & 0x7F);
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
			if (HL.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_H:
			//4 Clock Cycles
			decrement8BitRegister(DE.hi);
			if (HL.hi == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
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
					PC = PC - (byte & 0x7F);
				else
					PC = PC + (byte & 0x7F);
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
			if (HL.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_RESET(N_FLAG);
			break;

		case DEC_L:
			//4 Clock Cycles
			decrement8BitRegister(HL.lo);
			if (HL.lo == 0x0)
				CPU_FLAG_BIT_SET(Z_FLAG);

			CPU_FLAG_BIT_SET(N_FLAG);
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
					PC = PC - (byte & 0x7F);
				else
					PC = PC + (byte & 0x7F);
			}
			break;

		case LD_16IM_SP:
			//12 Clock Cycles
			temp->hi = memory->readMemory(PC++);
			temp->lo = memory->readMemory(PC++);
			SP = temp->data;
			break;

		default:
			break;
		}
	}

	void CPU::stop()
	{
		this->_cpuRunning = false;
	}

	void CPU::increment16BitRegister(uWORD reg)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		++reg;
	}

	void CPU::increment8BitRegister(uBYTE reg)
	{
		++reg;
	}

	void CPU::decrement8BitRegister(uBYTE reg)
	{
		--reg;
	}

	void CPU::decrement16BitRegister(uWORD reg)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		--reg;
	}

	void CPU::add16BitRegister(uWORD host, uWORD operand)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 4));
		CPU_FLAG_BIT_RESET(N_FLAG);
		host = host + operand;
		if (operand > UCHAR_MAX - host)
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
}