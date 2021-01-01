#ifndef DEFINES_H
#define DEFINES_H

typedef unsigned char uBYTE;
typedef unsigned short uWORD;
typedef char sBYTE;
typedef short sWORD;

#define SCALE_FACTOR 4

#define NATIVE_SIZE_X 160
#define NATIVE_SIZE_Y 144
#define EXT_SIZE_X 256
#define EXT_SIZE_Y 256

#define TILE_BYTE_LENGTH 16

#define LCDC_ADR 0xFF40
#define STAT_ADR 0xFF41

#define CPU_FLAG_BIT_SET(int) CPU::flagSet(int)
#define CPU_FLAG_BIT_TEST(int) CPU::flagTest(int)
#define CPU_FLAG_BIT_RESET(int) CPU::flagReset(int)

#define INTERUPT_EN_REGISTER_ADR 0xFFFF
#define INTERUPT_FLAG_REG 0xFF0F
#define TIMER_DIV_REG 0xFF04

#define VBLANK_INT 0x0040
#define LCDC_INT 0x0048
#define TIMER_OVER_INT 0x0050
#define SER_TRF_INT 0x0058
#define CONTROL_INT 0x0060

#define Z_FLAG 7
#define N_FLAG 6
#define H_FLAG 5
#define C_FLAG 4

#define NUM_ATTRIBUTES 13
#define RAM_ENABLED 0
#define ROM_RAM_MODE 1
#define ROM_ONLY 2
#define RAM  3
#define MBC1 4
#define MBC2 5
#define MBC3 6
#define MBC4 7
#define MBC5 8
#define HUC1 9
#define BATTERY   10
#define TIMER 11
#define RUMBLE 12
#define ROM_BANK_SIZE 0x4000

#define wxID_CUSTOM_DEBUGGER 500

#endif