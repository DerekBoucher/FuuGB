#include "Memory.h"

namespace FuuGB
{
    Memory::Memory(Cartridge *c)
    {
        m_Cart = c;
        m_Mem = new uBYTE[0x10000];
        memset(m_Mem, 0x0, 0x10000);

        m_DmaTransferInProgress = false;
        m_BootRomClosed = false;
        m_TimerCounter = 1024;
        m_DmaCyclesCompleted = 0;
        m_TranslatedAddr = 0x0000;
        m_DividerRegisterCounter = 0;
    }

    Memory::Memory(const Memory &other)
        : m_TimerCounter(other.m_TimerCounter), m_DmaCyclesCompleted(other.m_DmaCyclesCompleted), m_BootRomClosed(other.m_BootRomClosed),
          m_DmaTransferInProgress(other.m_DmaTransferInProgress),
          m_TranslatedAddr(other.m_TranslatedAddr), m_Cart(other.m_Cart), m_DividerRegisterCounter(other.m_DividerRegisterCounter)
    {
        m_Mem = new uBYTE[0x10000];
        memcpy(m_Mem, other.m_Mem, 0x10000);
    }

    Memory::~Memory()
    {
        delete[] m_Mem;
        delete m_Cart;
    }

    void Memory::closeBootRom()
    {
        if (!m_BootRomClosed)
        {
            m_BootRomClosed = true;
        }
    }

    void Memory::Write(uWORD addr, uBYTE data)
    {
        // Writing to memory takes 4 cycles
        UpdateTimers(4);

        if ((addr < 0x8000) && !m_DmaTransferInProgress) // Cart ROM
        {
            if (addr < 0x2000)
            {
                toggleRam(addr, data);
            }
            else if (addr < 0x4000)
            {
                changeRomBank(addr, data);
            }
            else if (addr < 0x6000)
            {
                changeRamBank(data);
            }
            else
            {
                changeMode(data);
            }
        }
        else if ((addr >= 0x8000) && (addr < 0xA000) && !m_DmaTransferInProgress) // Video RAM
        {
            uBYTE mode = getStatMode();

            if (mode == 0 || mode == 1 || mode == 2)
            {
                m_Mem[addr] = data;
            }
        }
        else if ((addr >= 0xA000) && (addr < 0xC000) && !m_DmaTransferInProgress) // External RAM
        {
            m_TranslatedAddr = addr - 0xA000;
            if (m_Cart->m_Attributes[Cartridge::c_RamEnabled])
            {
                if (m_Cart->m_Attributes[Cartridge::c_RomOnly])
                {
                    m_Cart->m_Rom[addr] = data;
                }
                else if (m_Cart->m_Attributes[Cartridge::c_Mbc1])
                {
                    if (m_Cart->m_Attributes[Cartridge::c_Mode])
                    {
                        m_Cart->m_Rom[m_TranslatedAddr + (0xA000 * m_Cart->m_CurrentRamBank)] = data;
                    }
                    else
                    {
                        m_Cart->m_Rom[addr] = data;
                    }
                }
            }
        }
        else if ((addr >= 0xC000) && (addr < 0xD000) && !m_DmaTransferInProgress) // Work RAM 0
        {
            m_Mem[addr] = data;
        }
        else if ((addr >= 0xD000) && (addr < 0xE000) && !m_DmaTransferInProgress) // Work RAM 1
        {
            m_Mem[addr] = data;
        }
        else if ((addr >= 0xE000) && (addr < 0xFE00) && !m_DmaTransferInProgress) //Echo of Work RAM, typically not used
        {
            m_Mem[addr] = data;
            m_Mem[addr - 0x2000] = data;
        }
        else if ((addr >= 0xFE00) && (addr < 0xFEA0) && !m_DmaTransferInProgress) //OAM RAM
        {
            uBYTE mode = getStatMode();

            if (mode == 0 || mode == 1)
            {
                m_Mem[addr] = data;
            }
        }
        else if ((addr >= 0xFEA0) && (addr < 0xFF00) && !m_DmaTransferInProgress) // Not Usable
        {
            return;
        }
        else if ((addr >= 0xFF00) && (addr < 0xFF80) && !m_DmaTransferInProgress) // I/O Registers
        {
            if (addr == 0xFF00) // Joypad register
            {
                data = (data & 0xF0) | (m_Mem[addr] & 0x0F);
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF01) // Serial Transfer Data
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF02) // Serial Transfer Control Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF04) // Divider Register
            {
                m_Mem[addr] = 0x00;
            }
            else if (addr == 0xFF05) // Timer Counter Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF06) // Timer Modulo Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF07) // Timer Controller Register
            {
                m_Mem[addr] = data;
                switch (m_Mem[addr] & 0x03)
                {
                case 0:
                    this->m_TimerCounter = 1024;
                    break;
                case 1:
                    this->m_TimerCounter = 16;
                    break;
                case 2:
                    this->m_TimerCounter = 64;
                    break;
                case 3:
                    this->m_TimerCounter = 256;
                    break;
                }
            }
            else if (addr == 0xFF0F) // Interrupt Flag Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF10) // Channel 1 Sweep Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF11) // Channel 1 Sound length/wave pattern duty Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF12) // Channel 1 Volume Envelope Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF13) // Channel 1 Frequency lo Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF14) // Channel 1 Freqency hi Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF16) // Channel 2 Sound length/wave pattern duty Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF17) // Channel 2 Volume Envelope Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF18) // Channel 2 Frequency lo Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF19) // Channel 2 Freqency hi Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF1A) // Channel 3 Sound On/Off Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF1B) // Channel 3 Sound Length Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF1C) // Channel 3 Select Output Level Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF1D) // Channel 3 Frequency lo Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF1E) // Channel 3 Frequency hi Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF20) // Channel 4 Sound length/wave pattern duty Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF21) // Channel 4 Volume Envelope Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF22) // Channel 4 Polynomial Counter Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF23) // Channel 4 Counter/Consecutive Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF24) // Channel Control Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF25) // Selection of Sound Output Terminal
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF26) // Sound On/Off
            {
                data = (data & 0x80) | (m_Mem[addr] & 0x7F); // Only bit 7 is writable
                m_Mem[addr] = data;
            }
            else if ((addr >= 0xFF30) && (addr < 0xFF40)) // Wave Pattern RAM
            {
                if (m_Mem[0xFF1A] & 0x80) // Only accessible if CH3 bit 7 is reset
                {
                    m_Mem[addr] = data;
                }
            }
            else if (addr == 0xFF40) // LCDC Register
            {
                uBYTE mode = getStatMode();
                if (!(data & 0x80))
                {
                    if (mode != 1)
                    {
                        data |= 0x80;
                    }
                }
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF41) // STAT Register
            {
                uBYTE temp = m_Mem[addr] & 0x07;
                data |= 0x80;
                data = data & 0xF8;
                data |= temp;
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF42) // Scroll Y Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF43) // Scroll X Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF44) // LY Register
            {
                m_Mem[addr] = 0;
            }
            else if (addr == 0xFF45) // LY Compare Register
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF46) // Request for dma transfer
            {
                dmaTransfer(data);
            }
            else if (addr == 0xFF47) // BG Palette Data
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF48) // Object Palette 0 Data
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF49) // Object Palette 1 Data
            {
                m_Mem[addr] = data;
            }
            else if (addr == 0xFF50)
            {
                m_Mem[addr] = data;
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
            m_Mem[addr] = data;
        }
        else if ((addr == 0xFFFF) && !m_DmaTransferInProgress) // Interrupt Enable Register
        {
            m_Mem[addr] = data;
        }
    }

    uBYTE Memory::Read(uWORD addr)
    {
        // Reading from memory takes 4 cycles
        UpdateTimers(4);

        if ((addr < 0x4000) && !m_DmaTransferInProgress) // Cart ROM Bank 0
        {
            if (!m_BootRomClosed && (addr < 0x100))
            {
                return m_BootRom[addr];
            }
            else
            {
                if (m_Cart->m_Attributes[Cartridge::c_Mode])
                {
                    if (m_Cart->m_RomSize > 0x100000)
                    {
                        switch (m_Cart->m_CurrentRamBank)
                        {
                        case 0x00:
                            return m_Cart->m_Rom[addr];
                            break;
                        case 0x01:
                            return m_Cart->m_Rom[addr * 0x20];
                            break;
                        case 0x02:
                            return m_Cart->m_Rom[addr * 0x40];
                            break;
                        case 0x03:
                            return m_Cart->m_Rom[addr * 0x60];
                            break;
                        default:
                            return m_Cart->m_Rom[addr];
                            break;
                        }
                    }
                    else
                    {
                        return m_Cart->m_Rom[addr];
                    }
                }
                else
                {
                    return m_Cart->m_Rom[addr];
                }
            }
        }
        else if ((addr >= 0x4000) && (addr < 0x8000) && !m_DmaTransferInProgress) // Cart ROM Bank n
        {
            m_TranslatedAddr = addr - 0x4000;
            return m_Cart->m_Rom[m_TranslatedAddr + (0x4000 * m_Cart->m_CurrentRomBank)];
        }
        else if ((addr >= 0x8000) && (addr < 0xA000) && !m_DmaTransferInProgress) // Video RAM
        {
            return m_Mem[addr];
        }
        else if ((addr >= 0xA000) && (addr < 0xC000) && !m_DmaTransferInProgress) // External RAM
        {
            m_TranslatedAddr = addr - 0xA000;
            if (m_Cart->m_Attributes[Cartridge::c_RamEnabled])
            {
                if (m_Cart->m_Attributes[Cartridge::c_RomOnly])
                {
                    return m_Cart->m_Rom[addr];
                }
                if (m_Cart->m_Attributes[Cartridge::c_Mbc1])
                {
                    if (m_Cart->m_Attributes[Cartridge::c_Mode])
                    {
                        return m_Cart->m_Rom[m_TranslatedAddr + (0xA000 * m_Cart->m_CurrentRamBank)];
                    }
                    else
                    {
                        return m_Cart->m_Rom[addr];
                    }
                }
            }
            else
            {
                return 0xFF;
            }
        }
        else if ((addr >= 0xC000) && (addr < 0xD000) && !m_DmaTransferInProgress) // Work RAM 0
        {
            return m_Mem[addr];
        }
        else if ((addr >= 0xD000) && (addr < 0xE000) && !m_DmaTransferInProgress) // Work RAM 1
        {
            return m_Mem[addr];
        }
        else if ((addr >= 0xE000) && (addr < 0xFE00) && !m_DmaTransferInProgress) // Echo of Work RAM
        {
            return m_Mem[addr];
        }
        else if ((addr >= 0xFE00) && (addr < 0xFEA0) && !m_DmaTransferInProgress) //OAM RAM
        {
            uBYTE mode = getStatMode();

            if (mode == 0 || mode == 1)
                return m_Mem[addr];
            else
                return 0xFF;
        }
        else if ((addr >= 0xFEA0) && (addr < 0xFF00) && !m_DmaTransferInProgress) // Not Usable
        {
            return 0xFF;
        }
        else if ((addr >= 0xFF00) && (addr < 0xFF80) && !m_DmaTransferInProgress) // I/O Registers
        {
            return m_Mem[addr];
        }
        else if ((addr >= 0xFF80) && (addr < 0xFFFE)) // HRAM
        {
            return m_Mem[addr];
        }
        // Interrupt Enable Register 0xFFFF
        return m_Mem[addr];
    }

    uBYTE Memory::DmaRead(uWORD addr)
    {
        return m_Mem[addr];
    }

    void Memory::DmaWrite(uWORD addr, uBYTE data)
    {
        if (addr == 0xFF41)
        {
            data |= 0x80;
        }
        m_Mem[addr] = data;
    }

    void Memory::UpdateTimers(int cycles)
    {
        uBYTE TAC = m_Mem[0xFF07];

        // Update the divider register
        m_DividerRegisterCounter += cycles;
        if (m_DividerRegisterCounter >= 256)
        {
            m_Mem[0xFF04]++;
            m_DividerRegisterCounter -= 256;
        }

        if (TAC & (1 << 2)) // Check if clock is enabled
        {
            m_TimerCounter -= cycles;

            while (m_TimerCounter <= 0)
            {
                int remainder = m_TimerCounter;

                uBYTE frequency = (TAC & 0x03);
                switch (frequency)
                {
                case 0:
                    m_TimerCounter = 1024;
                    break;
                case 1:
                    m_TimerCounter = 16;
                    break;
                case 2:
                    m_TimerCounter = 64;
                    break;
                case 3:
                    m_TimerCounter = 256;
                    break;
                }

                m_TimerCounter += remainder;

                // Timer Overflow
                if (m_Mem[0xFF05] == 0xFF)
                {
                    m_Mem[0xFF05] = m_Mem[0xFF06];
                    RequestInterupt(2);
                }
                else
                {
                    m_Mem[0xFF05]++;
                }
            }
        }
    }

    void Memory::changeRomBank(uWORD addr, uBYTE data)
    {
        if (m_Cart->m_Attributes[Cartridge::c_RomOnly])
        {
            m_Cart->m_CurrentRomBank = 0x01;
            return;
        }

        if (m_Cart->m_Attributes[Cartridge::c_Mbc1])
        {
            m_Cart->m_CurrentRomBank = (data & 0x1F);

            if (m_Cart->m_CurrentRomBank > m_Cart->m_RomBankCount)
                m_Cart->m_CurrentRomBank &= (m_Cart->m_RomBankCount - 1);

            if (m_Cart->m_CurrentRomBank == 0x00 ||
                m_Cart->m_CurrentRomBank == 0x20 ||
                m_Cart->m_CurrentRomBank == 0x40 ||
                m_Cart->m_CurrentRomBank == 0x60)
                m_Cart->m_CurrentRomBank += 0x01;
        }
        else if (m_Cart->m_Attributes[Cartridge::c_Mbc2])
        {
            if ((addr & 0x10) == 0x10)
            {
                m_Cart->m_CurrentRomBank = data & 0x0F;

                if (m_Cart->m_CurrentRomBank == 0x00 ||
                    m_Cart->m_CurrentRomBank == 0x20 ||
                    m_Cart->m_CurrentRomBank == 0x40 ||
                    m_Cart->m_CurrentRomBank == 0x60)
                    m_Cart->m_CurrentRomBank += 0x01;
            }
        }
        else if (m_Cart->m_Attributes[Cartridge::c_Mbc3] || m_Cart->m_Attributes[Cartridge::c_Mbc5])
        {
            m_Cart->m_CurrentRomBank = data & 0x7F;

            if (m_Cart->m_CurrentRomBank == 0x00 ||
                m_Cart->m_CurrentRomBank == 0x20 ||
                m_Cart->m_CurrentRomBank == 0x40 ||
                m_Cart->m_CurrentRomBank == 0x60)
                m_Cart->m_CurrentRomBank += 0x01;
        }
    }

    void Memory::toggleRam(uWORD addr, uBYTE data)
    {
        if (m_Cart->m_Attributes[Cartridge::c_Mbc1] ||
            m_Cart->m_Attributes[Cartridge::c_Mbc3] ||
            m_Cart->m_Attributes[Cartridge::c_Mbc5] ||
            m_Cart->m_Attributes[Cartridge::c_RomOnly])
        {
            if ((data & 0x0F) == 0x0A)
                m_Cart->m_Attributes[Cartridge::c_RamEnabled] = true;
            else
                m_Cart->m_Attributes[Cartridge::c_RamEnabled] = false;
        }
        else if (m_Cart->m_Attributes[Cartridge::c_Mbc2])
        {
            if ((addr & 0x10) == 0x00)
            {
                if ((data & 0x0F) == 0x0A)
                    m_Cart->m_Attributes[Cartridge::c_RamEnabled] = true;
                else
                    m_Cart->m_Attributes[Cartridge::c_RamEnabled] = false;
            }
        }
    }

    void Memory::changeMode(uBYTE data)
    {
        if (m_Cart->m_Attributes[Cartridge::c_Mbc1])
        {
            if (data & 0x01)
            {
                m_Cart->m_Attributes[Cartridge::c_Mode] = true;
            }
            else
            {
                m_Cart->m_Attributes[Cartridge::c_Mode] = false;
            }
        }
    }

    void Memory::changeRamBank(uBYTE data)
    {
        if (m_Cart->m_Attributes[Cartridge::c_Mbc1])
        {
            if (m_Cart->m_Attributes[Cartridge::c_Mode])
            {
                m_Cart->m_CurrentRamBank = data & 0x03;
            }
            else
            {
                m_Cart->m_CurrentRamBank = 0x01;
                m_Cart->m_CurrentRomBank |= ((data & 0x03) << 5);
                if (m_Cart->m_CurrentRomBank > m_Cart->m_RomBankCount)
                    m_Cart->m_CurrentRomBank &= (m_Cart->m_RomBankCount - 1);
            }
        }
    }

    void Memory::RequestInterupt(int code)
    {
        uBYTE IF = m_Mem[0xFF0F];

        switch (code)
        {
        case 0:
            IF |= 0x01;
            m_Mem[0xFF0F] = IF;
            break;
        case 1:
            IF |= 0x02;
            m_Mem[0xFF0F] = IF;
            break;
        case 2:
            IF |= 0x04;
            m_Mem[0xFF0F] = IF;
            break;
        case 3:
            IF |= 0x08;
            m_Mem[0xFF0F] = IF;
            break;
        case 4:
            IF |= 0x10;
            m_Mem[0xFF0F] = IF;
            break;
        }
    }

    void Memory::UpdateDmaCycles(int cyclesToAdd)
    {
        if (m_DmaTransferInProgress)
        {
            m_DmaCyclesCompleted += cyclesToAdd;
            if (m_DmaCyclesCompleted >= 160)
            {
                m_DmaTransferInProgress = false;
            }
        }
        else
        {
            m_DmaCyclesCompleted = 0;
        }
    }

    void Memory::dmaTransfer(uBYTE data)
    {
        m_DmaTransferInProgress = true;

        // Check if source prefix is outside of allowed source addresses
        if (data > 0xF1)
        {
            data = 0xF1;
        }

        // Begin Transfer
        for (uBYTE i = 0; i < 0xA0; i++)
        {
            m_Mem[0xFE00 + i] = m_Mem[(data << 8) | i];
        }
    }

    uBYTE Memory::getStatMode()
    {
        return m_Mem[0xFF41] & 0x03;
    }
} // namespace FuuGB
