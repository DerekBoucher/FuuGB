#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "Defines.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>

class Cartridge
{
public:
    explicit Cartridge(FILE* input);
    Cartridge(const Cartridge&);
    ~Cartridge();

    static const int c_NumAttributes = 13;
    static const int c_RamEnabled = 0;
    static const int c_Mode = 1;
    static const int c_RomOnly = 2;
    static const int c_Ram = 3;
    static const int c_Mbc1 = 4;
    static const int c_Mbc2 = 5;
    static const int c_Mbc3 = 6;
    static const int c_Mbc4 = 7;
    static const int c_Mbc5 = 8;
    static const int c_Huc1 = 9;
    static const int c_Battery = 10;
    static const int c_Timer = 11;
    static const int c_Rumble = 12;
    static const unsigned int c_RomBankSize = 0x4000;

    bool m_Attributes[c_NumAttributes];

    uWORD m_CurrentRamBank;
    uWORD m_CurrentRomBank;
    uWORD m_RomBankCount;
    uWORD m_RamBankCount;
    uWORD m_RamBankSize;

    uint64_t m_RomSize;
    uint64_t m_RamSize;

    uBYTE* m_Rom;

    Cartridge& operator=(const Cartridge& other)
    {
        memcpy(this->m_Attributes, other.m_Attributes, c_NumAttributes);
        this->m_CurrentRamBank = other.m_CurrentRamBank;
        this->m_CurrentRomBank = other.m_RomBankCount;
        this->m_RomBankCount = other.m_RomBankCount;
        this->m_RamBankCount = other.m_RamBankCount;
        this->m_RamBankSize = other.m_RamBankSize;
        this->m_RomSize = other.m_RomSize;
        this->m_RamSize = other.m_RamSize;
        memcpy(this->m_Rom, other.m_Rom, other.m_RomSize);
        return *this;
    }
};

#endif