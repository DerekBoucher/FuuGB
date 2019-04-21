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
	}

	CPU::~CPU()
	{
		_cpuTHR->join();
		delete _cpuTHR;
	}

	void CPU::clock()
	{
		while (_cpuRunning)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS));
			executeNextOpCode();
		}
	}

	void CPU::executeNextOpCode()
	{
		switch (memory->readMemory(PC++))
		{
		case NOP:
			//1 CPU Cycle
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
			increment16BitRegister(INC_BC);
			break;

		case INC_B:
			//4 CPU Cycles
			increment8BitRegister(INC_B);
			break;

		case DEC_B:
			//4 CPU Cycles
			decrement8BitRegister(DEC_B);
			break;

		case LD_8IMM_B:
			//8 CPU Cycles
			BC.hi = memory->readMemory(PC++);
			break;

		case RLC_A:
			//4 CPU Cycles
			std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 3));
			AF.lo = AF.lo | (AF.hi & 0x10);
			AF.lo = AF.lo & 0x10;
			AF.hi = AF.hi << 0x1;
			if (AF.hi == 0)
				AF.lo = AF.lo | 0x8;
			break;

		default:
			break;
		}
	}

	void CPU::stop()
	{
		this->_cpuRunning = false;
	}

	void CPU::increment16BitRegister(opCode op)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 7));
		switch (op)
		{
		case INC_BC:
			++BC.data;
			break;

		default:
			break;
		}
	}

	void CPU::increment8BitRegister(opCode op)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 3));
		switch (op)
		{
		case INC_B:
			++BC.hi;
			break;

		default:
			break;
		}
	}

	void CPU::decrement8BitRegister(opCode op)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(CPU_CLOCK_PERIOD_NS * 3));
		switch (op)
		{
		case DEC_B:
			--BC.hi;
			break;

		default:
			break;
		}
	}
}