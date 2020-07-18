#include "Core.h"
#include "Cartridge.h"

namespace FuuGB 
{
    Cartridge::Cartridge(FILE* input) 
    {
        // Define default rom and ram bank
        CurrentRomBank = 0x01;
        CurrentRamBank = 0x01;

        fseek(input, 0x147, SEEK_SET);
        // Determine cart type
        switch (fgetc(input)) {
        case 0x00:
            ROM = true;
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
            ROM = true;
            RAM = true;
            break;
        case 0x09:
            ROM = true;
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

        // Determine amount of rom banks
        fseek(input, 0x148, SEEK_SET);
        switch (fgetc(input)) {
        case 0x00:
            RomBankCount = 0;
            RomSize = 0x8000;
            break;
        case 0x01:
            RomBankCount = 4;
            RomSize = 0x10000;
            break;
        case 0x02:
            RomBankCount = 8;
            RomSize = 0x20000;
            break;
        case 0x03:
            RomBankCount = 16;
            RomSize = 0x40000;
            break;
        case 0x04:
            RomBankCount = 32;
            RomSize = 0x80000;
            break;
        case 0x05:
            RomBankCount = 64;
            RomSize = 0x100000;
            break;
        case 0x06:
            RomBankCount = 128;
            RomSize = 0x200000;
            break;
        case 0x07:
            RomBankCount = 256;
            RomSize = 0x400000;
            break;
        case 0x08:
            RomBankCount = 512;
            RomSize = 0x800000;
            break;
        case 0x52:
            RomBankCount = 72;
            RomSize = 0x120000;
            break;
        case 0x53:
            RomBankCount = 80;
            RomSize = 0x133334;
            break;
        case 0x54:
            RomBankCount = 96;
            RomSize = 0x180000;
            break;
        default:
            exit(-8);
            break;
        }

        // Determine amount and size of the ram banks
        fseek(input, 0x149, SEEK_SET);
        switch (fgetc(input)) {
        case 0x00:
            RamBankSize = 0;
            RamBankCount = 0;
            break;
        case 0x01:
            RamBankSize = 0x800;
            RamBankCount = 1;
            break;
        case 0x02:
            RamBankSize = 0x2000;
            RamBankCount = 1;
            break;
        case 0x03:
            RamBankSize = 0x2000;
            RamBankCount = 4;
            break;
        case 0x04:
            RamBankSize = 0x2000;
            RamBankCount = 16;
            break;
        case 0x05:
            RamBankSize = 0x2000;
            RamBankCount = 8;
            break;
        default:
            exit(-9);     
            break;
        }

        RamSize = RamBankCount * RamBankSize;

        Rom = new uBYTE[RomSize];
        fseek(input, 0x0, SEEK_SET);
        fread(Rom, 0x1, RomSize, input);

        RamEnabled = false;
        Mode = false;
    }

    Cartridge::~Cartridge() {
    }
}
