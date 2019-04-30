//
//  Memory.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#include "Fuupch.h"
#include "Memory.h"

namespace FuuGB
{
	Memory::Memory(Cartridge* gameCart)
	{
		cart = gameCart;
		M_MEM = new uBYTE[0x10000];
		memset(this->M_MEM, 0x0, 0x10000);
		bootROM = fopen("DMG_ROM.bin", "rb");
		fread(M_MEM, sizeof(uBYTE), 0x100, bootROM);
		fclose(bootROM);
		for (int i = 0x100; i < 0x4000; ++i)
			M_MEM[i] = cart->ROM[i];
		_memoryRunning = true;
		//_ramTHR = new std::thread(&Memory::ramClock, this);
		FUUGB_MEM_LOG("RAM Initialized.");
	}

	Memory::~Memory()
	{
		//_ramTHR->join();
		//ramCond.notify_all();
		//delete _ramTHR;
		delete[] M_MEM;
		delete cart;
		FUUGB_MEM_LOG("RAM Destroyed.");
	}

	void Memory::ramClock()
	{
		while (_memoryRunning)
		{
			//std::this_thread::sleep_for(std::chrono::nanoseconds(RAM_CLOCK_PERIOD_NS));
			ramCond.notify_all();
		}
	}

	void Memory::stop()
	{
		this->_memoryRunning = false;
	}

	void Memory::writeMemory(uWORD addr, uBYTE data)
	{
		//std::this_thread::sleep_for(std::chrono::nanoseconds(RAM_CLOCK_PERIOD_NS));
		M_MEM[addr] = data;
	}

	uBYTE& Memory::readMemory(uWORD addr)
	{
		//std::this_thread::sleep_for(std::chrono::nanoseconds(RAM_CLOCK_PERIOD_NS));
		return M_MEM[addr];
	}
}
