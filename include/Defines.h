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

#define ID_DEBUGGER 500

#endif