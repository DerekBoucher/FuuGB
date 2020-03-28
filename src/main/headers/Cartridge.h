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
    
        uBYTE*  ROM;
        bool    ROMM        = false;
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
        bool    extRamEnabled;
        bool    mode;
        int     ROM_BANK_CNT;
        int     RAM_BANK_CNT;
        int     RAM_SIZE;
        uBYTE   currentRamBank;
        uBYTE   currentRomBank;
        uWORD   RAM_Bank_Base_MBC1;
    };
}
#endif /* Cartridge_h */
