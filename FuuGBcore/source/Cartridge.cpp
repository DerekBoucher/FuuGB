//
//  Cartridge.cpp
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

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
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x01:
			ROMonly = true;
			RAM = false;
			MBC1 = true;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x02:
			ROMonly = true;
			RAM = true;
			MBC1 = true;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x03:
			ROMonly = true;
			RAM = true;
			MBC1 = true;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x05:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = true;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x06:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = true;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x08:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x09:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x0B:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x0C:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x0D:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0x12:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = true;
			MBC5 = false;
			break;
		case 0x13:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = true;
			MBC5 = false;
			break;
		case 0x19:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1A:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1B:
			ROMonly = true;
			RAM = true;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1C:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1D:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1E:
			ROMonly = true;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = true;
			break;
		case 0x1F:
			ROMonly = false;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0xFD:
			ROMonly = false;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		case 0xFE:
			ROMonly = false;
			RAM = false;
			MBC1 = false;
			MBC2 = false;
			MBC3 = false;
			MBC5 = false;
			break;
		default:
			break;
		}
	}

	Cartridge::~Cartridge()
	{
		delete[] ROM;
	}
}