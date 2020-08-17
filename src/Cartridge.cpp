#include "Core.h"
#include "Cartridge.h"

namespace FuuGB
{
    Cartridge::Cartridge() {}

    Cartridge::Cartridge(FILE *input)
    {
        // Define default rom and ram bank
        m_CurrentRomBank = 0x01;
        m_CurrentRamBank = 0x01;

        // Set all attributes to false by default
        for (int i = 0; i < c_NumAttributes; i++)
        {
            m_Attributes[i] = false;
        }

        fseek(input, 0x147, SEEK_SET);
        // Determine cart type
        switch (fgetc(input))
        {
        case 0x00:
            m_Attributes[c_RomOnly] = true;
            break;
        case 0x01:
            m_Attributes[c_Mbc1] = true;
            break;
        case 0x02:
            m_Attributes[c_Mbc1] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x03:
            m_Attributes[c_Mbc1] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x05:
            m_Attributes[c_Mbc2] = true;
            break;
        case 0x06:
            m_Attributes[c_Mbc2] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x08:
            m_Attributes[c_RomOnly] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x09:
            m_Attributes[c_RomOnly] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
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
            m_Attributes[c_Mbc3] = true;
            m_Attributes[c_Timer] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x10:
            m_Attributes[c_Mbc3] = true;
            m_Attributes[c_Timer] = true;
            m_Attributes[c_Battery] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x11:
            m_Attributes[c_Mbc3] = true;
            break;
        case 0x12:
            m_Attributes[c_Mbc3] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x13:
            m_Attributes[c_Mbc3] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x15:
            m_Attributes[c_Mbc4] = true;
            break;
        case 0x16:
            m_Attributes[c_Mbc4] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x17:
            m_Attributes[c_Mbc4] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x19:
            m_Attributes[c_Mbc5] = true;
            break;
        case 0x1A:
            m_Attributes[c_Mbc5] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x1B:
            m_Attributes[c_Mbc5] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
            break;
        case 0x1C:
            m_Attributes[c_Mbc5] = true;
            m_Attributes[c_Rumble] = true;
            break;
        case 0x1D:
            m_Attributes[c_Mbc5] = true;
            m_Attributes[c_Rumble] = true;
            m_Attributes[c_Ram] = true;
            break;
        case 0x1E:
            m_Attributes[c_Mbc5] = true;
            m_Attributes[c_Rumble] = true;
            m_Attributes[c_Ram] = true;
            m_Attributes[c_Battery] = true;
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
        switch (fgetc(input))
        {
        case 0x00:
            m_RomBankCount = 1;
            m_RomSize = 0x8000;
            break;
        case 0x01:
            m_RomBankCount = 4;
            m_RomSize = 0x10000;
            break;
        case 0x02:
            m_RomBankCount = 8;
            m_RomSize = 0x20000;
            break;
        case 0x03:
            m_RomBankCount = 16;
            m_RomSize = 0x40000;
            break;
        case 0x04:
            m_RomBankCount = 32;
            m_RomSize = 0x80000;
            break;
        case 0x05:
            m_RomBankCount = 64;
            m_RomSize = 0x100000;
            break;
        case 0x06:
            m_RomBankCount = 128;
            m_RomSize = 0x200000;
            break;
        case 0x07:
            m_RomBankCount = 256;
            m_RomSize = 0x400000;
            break;
        case 0x08:
            m_RomBankCount = 512;
            m_RomSize = 0x800000;
            break;
        case 0x52:
            m_RomBankCount = 72;
            m_RomSize = 0x120000;
            break;
        case 0x53:
            m_RomBankCount = 80;
            m_RomSize = 0x133334;
            break;
        case 0x54:
            m_RomBankCount = 96;
            m_RomSize = 0x180000;
            break;
        default:
            exit(-8);
            break;
        }

        // Determine amount and size of the ram banks
        fseek(input, 0x149, SEEK_SET);
        switch (fgetc(input))
        {
        case 0x00:
            m_RamBankSize = 0x800;
            m_RamBankCount = 1;
            break;
        case 0x01:
            m_RamBankSize = 0x800;
            m_RamBankCount = 1;
            break;
        case 0x02:
            m_RamBankSize = 0x2000;
            m_RamBankCount = 1;
            break;
        case 0x03:
            m_RamBankSize = 0x2000;
            m_RamBankCount = 4;
            break;
        case 0x04:
            m_RamBankSize = 0x2000;
            m_RamBankCount = 16;
            break;
        case 0x05:
            m_RamBankSize = 0x2000;
            m_RamBankCount = 8;
            break;
        default:
            exit(-9);
            break;
        }

        m_RamSize = m_RamBankCount * m_RamBankSize;

        m_Rom = new uBYTE[m_RomSize];
        fseek(input, 0x0, SEEK_SET);
        fread(m_Rom, 0x1, m_RomSize, input);
    }

    Cartridge::Cartridge(const Cartridge &other)
        : m_CurrentRamBank(other.m_CurrentRamBank),
          m_CurrentRomBank(other.m_CurrentRomBank),
          m_RomBankCount(other.m_RomBankCount),
          m_RamBankCount(other.m_RamBankCount),
          m_RamBankSize(other.m_RamBankSize),
          m_RomSize(other.m_RomSize),
          m_RamSize(other.m_RamSize)
    {
        m_Rom = new uBYTE[m_RomSize];
        memcpy(m_Rom, other.m_Rom, m_RomSize);
        memcpy(m_Attributes, other.m_Attributes, c_NumAttributes);
    }

    Cartridge::~Cartridge()
    {
        delete[] m_Rom;
    }
} // namespace FuuGB
