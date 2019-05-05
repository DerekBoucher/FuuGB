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

		for (int i = 0x100; i < 0x8000; ++i)
			M_MEM[i] = cart->ROM[i];

		_memoryRunning = true;
		_ramTHR = new std::thread(&Memory::ramClock, this);
		FUUGB_MEM_LOG("RAM Initialized.");
		bootRomClosed = false;
	}

	Memory::~Memory()
	{
		_ramTHR->join();
		ramCond.notify_all();
		delete _ramTHR;
		delete[] M_MEM;
		delete cart;
		FUUGB_MEM_LOG("RAM Destroyed.");
	}

	void Memory::closeBootRom()
	{
		if (!bootRomClosed)
		{
			for (int i = 0x0000; i < 0x0100; ++i)
				M_MEM[i] = cart->ROM[i];
			bootRomClosed = true;
		}
	}

	void Memory::ramClock()
	{
		while (_memoryRunning)
		{
			if (M_MEM[0xFF50] == 0x01)
				closeBootRom();
		}
	}

	void Memory::stop()
	{
		this->_memoryRunning = false;
	}

	void Memory::writeMemory(uWORD addr, uBYTE data)
	{
		if (addr < 0x8000)
		{
			if (addr >= 0x0000 && addr < 0x2000)
				toggleRAM(addr, data);
			else if (addr >= 0x2000 && addr < 0x4000)
				changeROMBank(addr, data);
			else if (addr >= 0x4000 && addr < 0x6000)
				changeRAMBank(addr, data);
			else if (addr >= 0x6000 && addr < 0x8000)
				changeMode(data);
		}
		else if ((addr >= 0xA000) && (addr < 0xC000))
		{
			if (cart->extRamEnabled)
			{
				M_MEM[addr] = data;
			}
		}
		else if ((addr >= 0xC000) && (addr < 0xE000))
		{

		}
		else if ((addr >= 0xE000) && (addr < 0xFE00))
		{

		}
		else if ((addr >= 0xFEA0) && (addr < 0xFEFF))
		{

		}
		else
			M_MEM[addr] = data;
	}

	uBYTE& Memory::readMemory(uWORD addr)
	{
		return M_MEM[addr];
	}

	uBYTE& Memory::DMA_read(uWORD addr) //Mainly used for PPU and Interupts
	{
		return M_MEM[addr];
	}

	void Memory::DMA_write(uWORD addr, uBYTE data)
	{
		M_MEM[addr] = data;
	}

	void Memory::changeROMBank(uWORD addr, uBYTE data)
	{
		if (cart->MBC1)
		{
			cart->currentRomBank = data & 0x1F;

			if (cart->currentRomBank == 0x0 || 0x20 || 0x40 || 0x60)
				cart->currentRomBank += 0x01;
		}
		else if (cart->MBC2)
		{
			if (addr & 0x10 == 0x10)
			{
				cart->currentRomBank = data & 0x0F;

				if (cart->currentRomBank == 0x0 || 0x20 || 0x40 || 0x60)
					cart->currentRomBank += 0x01;
			}
		}
		else if (cart->MBC3)
		{
			cart->currentRomBank = data & 0x7F;

			if (cart->currentRomBank == 0x0 || 0x20 || 0x40 || 0x60)
				cart->currentRomBank += 0x01;
		}
		for (int i = 0x4000; i < 0x8000; ++i)
			M_MEM[i] = cart->ROM[i*(cart->currentRomBank)];
	}

	void Memory::toggleRAM(uWORD addr, uBYTE data)
	{
		if (cart->MBC1 || cart->MBC3)
		{
			if (data & 0x0F == 0x0A)
				cart->extRamEnabled = true;
			else
				cart->extRamEnabled = false;
		}
		else if (cart->MBC2)
		{
			if ((addr & 0x10) == 0x00)
			{
				if (data & 0x0F == 0x0A)
					cart->extRamEnabled = true;
				else
					cart->extRamEnabled = false;
			}
		}
	}

	void Memory::changeMode(uBYTE data)
	{
		if (data & 0x01)
			cart->mode = true;
		else
			cart->mode = false;
	}

	void Memory::changeRAMBank(uWORD addr, uBYTE data)
	{
		if (cart->MBC1)
		{
			if (cart->mode)
			{
				cart->currentRamBank = data & 0x03;

				for (int i = 0xA000; i < 0xC000; ++i)
					M_MEM[i] = cart->ROM[i*(cart->currentRamBank)];
			}
			else
			{
				cart->currentRomBank |= ((data & 0x03) << 3);
				for (int i = 0x4000; i < 0x8000; ++i)
					M_MEM[i] = cart->ROM[i*(cart->currentRomBank)];
			}
		}
	}
}
