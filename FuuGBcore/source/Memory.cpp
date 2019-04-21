//
//  Memory.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#include "Memory.h"

namespace FuuGB
{
	Memory::Memory(Cartridge* gameCart)
	{
		cart = gameCart;
		M_MEM = new uBYTE[0x10000];
		memset(this->M_MEM, 0x0, 0x10000);
		bootROM = fopen("../../../../../../../../Desktop/Xcode Projects/GBemu/Common/DMG_ROM.bin", "rb");
		fread(M_MEM, sizeof(uBYTE), 0x100, bootROM);
		fclose(bootROM);
		for (int i = 0x100;i < 0x4000; ++i)
			M_MEM[i] = cart->ROM[i - 0x100];
		_memoryRunning = true;
		_memTHR = new std::thread(&Memory::clock, this);
	}

	Memory::~Memory()
	{
		_memTHR->join();
		memCond.notify_all();
		delete _memTHR;
		delete[] M_MEM;
		delete cart;
	}

	void Memory::clock()
	{
		while (_memoryRunning)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(MEMORY_CLOCK_PERIOD_NS));
			memCond.notify_all();
		}
	}

	void Memory::stop()
	{
		this->_memoryRunning = false;
	}

	void Memory::writeMemory(uWORD addr, uBYTE data)
	{
		std::unique_lock<decltype(key)> lock(key);
		memCond.wait(lock);
		M_MEM[addr] = data;
		lock.unlock();
	}

	uBYTE& Memory::readMemory(uWORD addr)
	{
		std::unique_lock<decltype(key)> lock(key);
		memCond.wait(lock);
		lock.unlock();
		return M_MEM[addr];
	}
}