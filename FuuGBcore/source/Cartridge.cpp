//
//  Cartridge.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#include "Fuupch.h"
#include "Cartridge.h"

namespace FuuGB
{
	Cartridge::Cartridge(FILE* input)
	{
		ROM = new uBYTE[0x200000];
		memset(this->ROM, 0x0, 0x200000);
		fread(this->ROM, 0x1, 0x200000, input);

		switch (ROM[0x147])
		{
		case 0x00:
			ROMM = true;
			break;
		case 0x01:
			MBC1 = true;
			break;
		case 0x02:
			MBC1 = true;
			RAM = true;
			break;
		case 0x03:
			MBC1 = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0x05:
			MBC2 = true;
			break;
		case 0x06:
			MBC2 = true;
			BATTERY = true;
			break;
		case 0x08:
			ROMM = true;
			RAM = true;
			break;
		case 0x09:
			ROMM = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0x0B:
			//Not Supported
			exit(-7);
			break;
		case 0x0C:
			//Not Supported
			exit(-7);
			break;
		case 0x0D:
			//Not Supported
			exit(-7);
			break;
		case 0x0F:
			MBC3 = true;
			TIMER = true;
			BATTERY = true;
			break;
		case 0x10:
			MBC3 = true;
			TIMER = true;
			BATTERY = true;
			RAM = true;
			break;
		case 0x11:
			MBC3 = true;
			break;
		case 0x12:
			MBC3 = true;
			RAM = true;
			break;
		case 0x13:
			MBC3 = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0x15:
			MBC4 = true;
			break;
		case 0x16:
			MBC4 = true;
			RAM = true;
			break;
		case 0x17:
			MBC4 = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0x19:
			MBC5 = true;
			break;
		case 0x1A:
			MBC5 = true;
			RAM = true;
			break;
		case 0x1B:
			MBC5 = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0x1C:
			MBC5 = true;
			RUMBLE = true;
			break;
		case 0x1D:
			MBC5 = true;
			RUMBLE = true;
			RAM = true;
			break;
		case 0x1E:
			MBC5 = true;
			RUMBLE = true;
			RAM = true;
			BATTERY = true;
			break;
		case 0xFC:
			//Not Supported
			exit(-7);
			break;
		case 0xFD:
			//Not Supported
			exit(-7);
			break;
		case 0xFE:
			//Not Supported
			exit(-7);
			break;
		case 0xFF:
			//Not Supported
			exit(-7);
			break;
		default:
			break;
		}

		switch (ROM[0x148])
		{
		case 0x00:
			ROM_BANK_CNT = 0;
			break;
		case 0x01:
			ROM_BANK_CNT = 4;
			break;
		case 0x02:
			ROM_BANK_CNT = 8;
			break;
		case 0x03:
			ROM_BANK_CNT = 16;
			break;
		case 0x04:
			ROM_BANK_CNT = 32;
			break;
		case 0x05:
			ROM_BANK_CNT = 64;
			break;
		case 0x06:
			ROM_BANK_CNT = 128;
			break;
		case 0x07:
			ROM_BANK_CNT = 256;
			break;
		case 0x52:
			ROM_BANK_CNT = 72;
			break;
		case 0x53:
			ROM_BANK_CNT = 80;
			break;
		case 0x54:
			ROM_BANK_CNT = 96;
			break;
		default:
			//Invalid Value
			exit(-8);
			break;
		}

		switch (ROM[0x149])
		{
		case 0x00:
			RAM_SIZE = 2;
			RAM_BANK_CNT = 0;
			break;
		case 0x01:
			RAM_SIZE = 8;
			RAM_BANK_CNT = 0;
		case 0x03:
			RAM_SIZE = 32;
			RAM_BANK_CNT = 4;
		default:
			//Invalid Value
			exit(-9);
			break;
		}
		extRamEnabled = false;
		mode = false;
	}

	Cartridge::~Cartridge()
	{
		delete[] ROM;
	}
}