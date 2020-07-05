#ifndef Cartridge_h
#define Cartridge_h

#include "Core.h"



namespace FuuGB
{
    class Cartridge
    {
    public:
        Cartridge(FILE* input);
        virtual ~Cartridge();
    
        uBYTE   Rom[0x200000];

        bool    RamEnabled;
        bool    Mode;

        bool    ROM         = false;
        bool    RAM         = false;
        bool    MBC1        = false;
        bool    MBC2        = false;
        bool    MBC3        = false;
        bool    MBC4        = false;
        bool    MBC5        = false;
        bool    HuC1        = false;
        bool    BATTERY     = false;
        bool    TIMER       = false;
        bool    RUMBLE      = false;

        unsigned int    CurrentRamBank;
        unsigned int    CurrentRomBank;
        unsigned int    RomBankCount;
        unsigned int    RamBankCount;
        unsigned int    RamBankSize;

        const unsigned int RomBankSize = 0x4000;
    };
}
#endif /* Cartridge_h */
