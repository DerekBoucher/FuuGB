//
//  Cartridge.h
//  GBemu
//
//  Created by Derek Boucher on 2019-02-10.
//  Copyright © 2019 Derek Boucher. All rights reserved.
//

#ifndef Cartridge_h
#define Cartridge_h

#include "Core.h"
namespace FuuGB
{
    class FUUGB_API Cartridge
    {
    public:
        Cartridge(FILE* input);
        virtual ~Cartridge();
    
        uBYTE*      ROM;
        bool        MBC1;
        bool        MBC2;
        bool        MBC3;
        bool        MBC5;
        bool        HuC1;
        bool        ROMonly;
        bool        RAM;
    };
}
#endif /* Cartridge_h */
