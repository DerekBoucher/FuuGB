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

		FUUGB_MEM_LOG("MEMORY Initialized.");
		bootRomClosed = false;
        timer_counter = 0;
	}

	Memory::~Memory()
	{

		delete[] M_MEM;
		delete cart;
		FUUGB_MEM_LOG("MEMORY Destroyed.");
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
	}

	void Memory::stop()
	{
	}

	void Memory::writeMemory(uWORD addr, uBYTE data)
	{
		if (addr < 0x8000) //Cart area
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
		else if ((addr >= 0x8000) && (addr < 0xA000)) //Video RAM
		{
			std::bitset<2> STAT(M_MEM[0xFF41] & 0x03);
			uBYTE mode = STAT.to_ulong();

			if(mode == 0 || mode == 1 || mode == 2)
				M_MEM[addr] = data;
		}
		else if ((addr >= 0xA000) && (addr < 0xC000)) //Switchable Ram Bank
		{
			if (cart->extRamEnabled)
			{
				M_MEM[addr] = data;
			}
		}
		else if ((addr >= 0xC000) && (addr < 0xE000))//Internal RAM
		{
			M_MEM[addr] = data;
			M_MEM[addr + ECHO_RAM_OFFSET] = data;
		}
		else if ((addr >= 0xE000) && (addr < 0xFE00)) //Echo of Internal RAM
		{
			M_MEM[addr] = data;
			M_MEM[addr - ECHO_RAM_OFFSET] = data;
		}
		else if ((addr >= 0xFE00) && (addr < 0xFE9F)) //OAM RAM
		{
			std::bitset<2> STAT(M_MEM[0xFF41] & 0x03);
			uBYTE mode = STAT.to_ulong();

			if (mode == 0 || mode == 1)
				M_MEM[addr] = data;
		}
        else if(addr == 0xFF07) //Timer Controller
        {
            uBYTE currentfrequency = this->readMemory(addr) & 0x03;
            M_MEM[addr] = data;
            uBYTE newfreq = this->readMemory(addr) & 0x03;
            if(currentfrequency != newfreq)
            {
                uBYTE frequency = this->readMemory(0xFF07) & 0x03;
                switch(frequency)
                {
                    case 0: this->timer_counter = 1024; break;
                    case 1: this->timer_counter = 16; break;
                    case 2: this->timer_counter = 64; break;
                    case 3: this->timer_counter = 256; break;
                }
            }
        }
        else if(addr == 0xFF04)
        {
            M_MEM[addr] = 0;
        }
		else if (addr == 0xFF40)
		{
			M_MEM[addr] = data;
			M_MEM[addr] |= 0x80;
		}
		else if (addr == 0xFF41)
		{
			data = data & 0xF8; //first 3 LS bits are Read only
			uBYTE temp = M_MEM[addr] & 0x07;
			data |= temp;
			M_MEM[addr] = data;
		}
        else if(addr == 0xFF44)
        {
            M_MEM[addr] = 0;
        }
        else if(addr == 0xFF46)
        {
            DMA_Transfer(data);
        }
        else if(addr == 0xFF50)
        {
            M_MEM[addr] = data;
            closeBootRom();
        }
		else
			M_MEM[addr] = data;
	}

	uBYTE& Memory::readMemory(uWORD addr)
	{
		if (addr >= 0xA000 & addr < 0xC000)
		{
			if (cart->extRamEnabled)
				return M_MEM[addr];
			else
				return dummy;
		}
		else if ((addr >= 0x8000) && (addr < 0x9FFF))
		{
			std::bitset<2> STAT(M_MEM[0xFF41] & 0x03);
			uBYTE mode = STAT.to_ulong();

			if (mode == 0 || mode == 1 || mode == 2)
				return M_MEM[addr];
			else
			{
				dummy = 0xff;
				return dummy;
			}
		}
		else
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
		uBYTE original = cart->currentRomBank;
		if (cart->ROMM)
			return;
		if (cart->MBC1)
		{
			cart->currentRomBank = data & 0x1F;


			if (cart->currentRomBank == 0x00 ||
				cart->currentRomBank == 0x20 ||
				cart->currentRomBank == 0x40 ||
				cart->currentRomBank == 0x60)
				cart->currentRomBank += 0x01;
		}
		else if (cart->MBC2)
		{
			if ((addr & 0x10) == 0x10)
			{
				cart->currentRomBank = data & 0x0F;

				if (cart->currentRomBank == 0x00 ||
					cart->currentRomBank == 0x20 ||
					cart->currentRomBank == 0x40 ||
					cart->currentRomBank == 0x60)
					cart->currentRomBank += 0x01;
			}
		}
		else if (cart->MBC3 || cart->MBC5)
		{
			cart->currentRomBank = data & 0x7F;

			if (cart->currentRomBank == 0x00 ||
				cart->currentRomBank == 0x20 ||
				cart->currentRomBank == 0x40 ||
				cart->currentRomBank == 0x60)
				cart->currentRomBank += 0x01;
		}
		if(original != cart->currentRomBank)
			for (int i = 0x4000; i < 0x8000; ++i)
				M_MEM[i] = cart->ROM[i*(cart->currentRomBank)];
	}

	void Memory::toggleRAM(uWORD addr, uBYTE data)
	{
		if (cart->MBC1 || cart->MBC3 || cart->MBC5)
		{
			if ((data & 0x0F) == 0x0A)
				cart->extRamEnabled = true;
			else
				cart->extRamEnabled = false;
		}
		else if (cart->MBC2)
		{
			if ((addr & 0x10) == 0x00)
			{
				if ((data & 0x0F) == 0x0A)
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
    
    void Memory::RequestInterupt(int code)
    {
        std::bitset<8> IF(this->readMemory(0xFF0F));
        
        switch(code)
        {
            case 0: IF.set(0); this->writeMemory(0xFF0F, (uBYTE)IF.to_ulong()); break;
            case 1: IF.set(1); this->writeMemory(0xFF0F, (uBYTE)IF.to_ulong()); break;
            case 2: IF.set(2); this->writeMemory(0xFF0F, (uBYTE)IF.to_ulong()); break;
            case 3: IF.set(3); this->writeMemory(0xFF0F, (uBYTE)IF.to_ulong()); break;
            case 4: IF.set(4); this->writeMemory(0xFF0F, (uBYTE)IF.to_ulong()); break;
        }
    }
    
    void Memory::DMA_Transfer(uBYTE data)
    {
        uWORD addr = data << 8;
        for(int i = 0; i < 0xA0; i++)
            this->writeMemory(0xFE00+i, this->readMemory(addr+i));
    }
}
