#include "Core.h"
#include "Memory.h"

namespace FuuGB
{
    Memory::Memory(Cartridge* c)
    {
        cart  = c;
        mainMemory  = new uBYTE[0x10000];
        memset(mainMemory, 0x0, 0x10000);

        for (int i = 0x0; i < 0x100; i++) {
            mainMemory[i] = bootRom[i];
        }
        for (int i = 0x100; i < 0x4000; i++) {
            mainMemory[i] = cart->Rom[i];
        }

        dmaTransferInProgress   = false;
        bootRomClosed           = false;
        TimerCounter            = 1024;
        dmaCyclesCompleted      = 0;
    }

    Memory::~Memory()
    {
        delete[] mainMemory;
        delete cart;
    }

    void Memory::closeBootRom()
    {
        if (!bootRomClosed)
        {
            for (int i = 0x0000; i < 0x0100; ++i) {
                mainMemory[i] = cart->Rom[i];
            }
            bootRomClosed = true;
        }
    }

    void Memory::Write(uWORD addr, uBYTE data)
    {
        if ((addr < 0x8000) && !dmaTransferInProgress) // Cart ROM
        {
            if (addr < 0x2000) 
            {
                toggleRam(addr, data);
            }
            else if ((addr >= 0x2000) && (addr < 0x4000))
            {
                changeRomBank(addr, data);
            }
            else if ((addr >= 0x4000) && (addr < 0x6000))
            {
                changeRamBank(data);
            }
            else if ((addr >= 0x6000) && (addr < 0x8000))
            {
                changeMode(data);
            }
        }
        else if ((addr >= 0x8000) && (addr < 0xA000) && !dmaTransferInProgress) // Video RAM
        {
            uBYTE mode = getStatMode();

            if (mode == 0 || mode == 1 || mode == 2) 
            {
                mainMemory[addr] = data;
            }
        }
        else if ((addr >= 0xA000) && (addr < 0xC000) && !dmaTransferInProgress) // External RAM
        {
            if (cart->RamEnabled)
            {
                cart->Rom[addr*cart->CurrentRamBank] = data;
            }
        }
        else if ((addr >= 0xC000) && (addr < 0xD000) && !dmaTransferInProgress) // Work RAM 0
        {
            mainMemory[addr] = data;
        }
        else if ((addr >= 0xD000) && (addr < 0xE000) && !dmaTransferInProgress) // Work RAM 1
        {
            mainMemory[addr] = data;
        }
        else if ((addr >= 0xE000) && (addr < 0xFE00) && !dmaTransferInProgress) //Echo of Work RAM, typically not used
        {
            mainMemory[addr] = data;
            mainMemory[addr - ECHO_RAM_OFFSET] = data;
        }
        else if ((addr >= 0xFE00) && (addr < 0xFEA0) && !dmaTransferInProgress) //OAM RAM
        {
            uBYTE mode = getStatMode();

            if (mode == 0 || mode == 1) 
            {
                mainMemory[addr] = data;
            }
        }
        else if ((addr >= 0xFEA0) && (addr < 0xFF00) && !dmaTransferInProgress) // Not Usable
        {
            return;
        }
        else if ((addr >= 0xFF00) && (addr < 0xFF80) && !dmaTransferInProgress) // I/O Registers
        {
            if (addr == 0xFF00) // Joypad register
            {
                data = (data & 0xF0) | (mainMemory[addr] & 0x0F);
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF01) // Serial Transfer Data
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF02) // Serial Transfer Control Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF04) // Divider Register
            {
                mainMemory[addr] = 0x00;
            }
            else if (addr == 0xFF05) // Timer Counter Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF06) // Timer Modulo Register
            {
                mainMemory[addr] = data;
            }
            else if(addr == 0xFF07) // Timer Controller Register
            {
                mainMemory[addr] = data;
                switch(mainMemory[addr] & 0x03)
                {
                    case 0: this->TimerCounter = 1024; break;
                    case 1: this->TimerCounter = 16; break;
                    case 2: this->TimerCounter = 64; break;
                    case 3: this->TimerCounter = 256; break;
                }
            }
            else if (addr == 0xFF0F) // Interrupt Flag Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF10) // Channel 1 Sweep Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF11) // Channel 1 Sound length/wave pattern duty Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF12) // Channel 1 Volume Envelope Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF13) // Channel 1 Frequency lo Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF14) // Channel 1 Freqency hi Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF16) // Channel 2 Sound length/wave pattern duty Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF17) // Channel 2 Volume Envelope Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF18) // Channel 2 Frequency lo Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF19) // Channel 2 Freqency hi Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF1A) // Channel 3 Sound On/Off Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF1B) // Channel 3 Sound Length Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF1C) // Channel 3 Select Output Level Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF1D) // Channel 3 Frequency lo Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF1E) // Channel 3 Frequency hi Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF20) // Channel 4 Sound length/wave pattern duty Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF21) // Channel 4 Volume Envelope Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF22) // Channel 4 Polynomial Counter Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF23) // Channel 4 Counter/Consecutive Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF24) // Channel Control Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF25) // Selection of Sound Output Terminal
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF26) // Sound On/Off
            {
                data = (data & 0x80) | (mainMemory[addr] & 0x7F); // Only bit 7 is writable
                mainMemory[addr] = data;
            }
            else if ((addr >= 0xFF30) && (addr < 0xFF40)) // Wave Pattern RAM
            {
                if (mainMemory[0xFF1A] & 0x80) // Only accessible if CH3 bit 7 is reset
                {
                    mainMemory[addr] = data;
                }
            }
            else if (addr == 0xFF40) // LCDC Register
            {
                uBYTE mode = getStatMode();
                if(!(data & 0x80))
                {
                    if(mode != 1) 
                    {
                        data |= 0x80;
                    }   
                }
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF41) // STAT Register
            {
                uBYTE temp = mainMemory[addr] & 0x07;
                data |= 0x80;
                data = data & 0xF8;
                data |= temp;
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF42) // Scroll Y Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF43) // Scroll X Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF44) // LY Register
            {
                mainMemory[addr] = 0;
            }
            else if (addr == 0xFF45) // LY Compare Register
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF46) // Request for dma transfer
            {
                dmaTransfer(data);
            }
            else if (addr == 0xFF47) // BG Palette Data
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF48) // Object Palette 0 Data
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF49) // Object Palette 1 Data
            {
                mainMemory[addr] = data;
            }
            else if (addr == 0xFF50)
            {
                mainMemory[addr] = data;
                closeBootRom();
            }
            else if (addr == 0xFF51) // New DMA source, high
            {
                // Not Used in DMG
            }
            else if (addr == 0xFF52) // New DMA source, low
            {
                // Not Used in DMG
            }
            else if (addr == 0xFF53) // New DMA dest, high
            {
                // Not Used in DMG
            }
            else if (addr == 0xFF54) // New DMA dest, lo
            {
                // Not used in DMG
            }
            else if (addr == 0xFF55) // New DMA length/Mode/Start
            {
                // Not used in DMG
            }
            else if (addr == 0xFF56) // Infrared Communications Port
            {
                // Not used in DMG
            }
        }
        else if ((addr >= 0xFF80) && (addr < 0xFFFE)) // HRAM
        {
            mainMemory[addr] = data;
        }
        else if ((addr == 0xFFFF) && !dmaTransferInProgress) // Interrupt Enable Register
        {
            mainMemory[addr] = data;
        }
    }

    uBYTE Memory::Read(uWORD addr)
    {
        if ((addr < 0x4000) && !dmaTransferInProgress) // Cart ROM Bank 0
        { 
            return mainMemory[addr];
        }
        else if ((addr >= 0x4000) && (addr < 0x8000) && !dmaTransferInProgress) // Cart ROM Bank n
        { 
            return cart->Rom[addr*cart->CurrentRomBank];
        }
        else if((addr >= 0x8000) && (addr < 0xA000) && !dmaTransferInProgress) // Video RAM
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xA000) && (addr < 0xC000) && !dmaTransferInProgress) // External RAM
        {
            if (cart->RamEnabled)
            {
                return cart->Rom[addr*cart->CurrentRamBank];
            }
            else 
            {
                return 0x00;
            }
        }
        else if ((addr >= 0xC000) && (addr < 0xD000) && !dmaTransferInProgress) // Work RAM 0
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xD000) && (addr < 0xE000) && !dmaTransferInProgress) // Work RAM 1
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xE000) && (addr < 0xFE00) && !dmaTransferInProgress) // Echo of Work RAM
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xFE00) && (addr < 0xFEA0) && !dmaTransferInProgress) //OAM RAM
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xFEA0) && (addr < 0xFF00) && !dmaTransferInProgress) // Not Usable
        {
            return 0xFF;
        }
        else if ((addr >= 0xFF00) && (addr < 0xFF80) && !dmaTransferInProgress) // I/O Registers
        {
            return mainMemory[addr];
        }
        else if ((addr >= 0xFF80) && (addr < 0xFFFE)) // HRAM
        {
            return mainMemory[addr];
        }
        else // Interrupt Enable Register
        {
            return mainMemory[addr];
        }
    }

    uBYTE Memory::DmaRead(uWORD addr)
    {
        return mainMemory[addr];
    }

    void Memory::DmaWrite(uWORD addr, uBYTE data)
    {
        if (addr == 0xFF41) {
            data |= 0x80;
        }
        mainMemory[addr] = data;
    }

    void Memory::changeRomBank(uWORD addr, uBYTE data)
    {
        if (cart->ROM)
            return;
        if (cart->MBC1)
        {
            cart->CurrentRomBank = data & 0x1F;


            if (cart->CurrentRomBank == 0x00 ||
                cart->CurrentRomBank == 0x20 ||
                cart->CurrentRomBank == 0x40 ||
                cart->CurrentRomBank == 0x60)
                cart->CurrentRomBank += 0x01;
        }
        else if (cart->MBC2)
        {
            if ((addr & 0x10) == 0x10)
            {
                cart->CurrentRomBank = data & 0x0F;

                if (cart->CurrentRomBank == 0x00 ||
                    cart->CurrentRomBank == 0x20 ||
                    cart->CurrentRomBank == 0x40 ||
                    cart->CurrentRomBank == 0x60)
                    cart->CurrentRomBank += 0x01;
            }
        }
        else if (cart->MBC3 || cart->MBC5)
        {
            cart->CurrentRomBank = data & 0x7F;

            if (cart->CurrentRomBank == 0x00 ||
                cart->CurrentRomBank == 0x20 ||
                cart->CurrentRomBank == 0x40 ||
                cart->CurrentRomBank == 0x60)
                cart->CurrentRomBank += 0x01;
        }
    }

    void Memory::toggleRam(uWORD addr, uBYTE data)
    {
        if (cart->MBC1 || cart->MBC3 || cart->MBC5)
        {
            if ((data & 0x0F) == 0x0A)
                cart->RamEnabled = true;
            else
                cart->RamEnabled = false;
        }
        else if (cart->MBC2)
        {
            if ((addr & 0x10) == 0x00)
            {
                if ((data & 0x0F) == 0x0A)
                    cart->RamEnabled = true;
                else
                    cart->RamEnabled = false;
            }
        }
    }

    void Memory::changeMode(uBYTE data)
    {
        if (data & 0x01)
            cart->Mode = true;
        else
            cart->Mode = false;
    }

    void Memory::changeRamBank(uBYTE data)
    {
        if (cart->MBC1)
        {
            if (cart->Mode)
            {
                cart->CurrentRamBank = data & 0x03;

                for (int i = 0xA000; i < 0xC000; ++i)
                    mainMemory[i] = cart->Rom[i*(cart->CurrentRamBank)];
            }
            else
            {
                cart->CurrentRomBank |= ((data & 0x03) << 3);
                for (int i = 0x4000; i < 0x8000; ++i)
                    mainMemory[i] = cart->Rom[i*(cart->CurrentRomBank)];
            }
        }
    }
    
    void Memory::RequestInterupt(int code)
    {
        uBYTE IF = mainMemory[0xFF0F];
        
        switch(code)
        {
            case 0: IF |= 0x01; mainMemory[0xFF0F] = IF; break;
            case 1: IF |= 0x02; mainMemory[0xFF0F] = IF; break;
            case 2: IF |= 0x04; mainMemory[0xFF0F] = IF; break;
            case 3: IF |= 0x08; mainMemory[0xFF0F] = IF; break;
            case 4: IF |= 0x10; mainMemory[0xFF0F] = IF; break;
        }
    }

    void Memory::UpdateDmaCycles(int cyclesToAdd)
    {
        if (dmaTransferInProgress) {
            dmaCyclesCompleted += cyclesToAdd;
            if (dmaCyclesCompleted >= 160) {
                dmaTransferInProgress = false;
            }
        } else
        {
            dmaCyclesCompleted = 0;
        }
    }
    
    void Memory::dmaTransfer(uBYTE data)
    {
        dmaTransferInProgress = true;

        // Divide start address by 100h
        uWORD addr = data >> 8;

        // Check if source prefix is outside of allowed source addresses
        if (addr > 0x00F1) 
        {
            addr = 0x00F1;
        }

        // Shift back addr
        addr = addr << 8;

        // Begin Transfer
        for(uBYTE i = 0; i < 0xA0; i++)
        {
            mainMemory[0xFE00+i] = mainMemory[addr & i];
        }
    }
    
    uBYTE Memory::getStatMode() {
        return mainMemory[0xFF41] & 0x03;
    }
}
