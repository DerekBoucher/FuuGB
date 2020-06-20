# FuuGBemu

Cross platform C/C++ GameBoy emulator using SDL2 Library.

Algorithm for the PPU and Memory Management are taken from http://www.codeslinger.co.uk/pages/projects/gameboy.html

All credit goes to codeslinger!

Early Alpha Development

## Build Prerequisites
    GNU suite of tools (more specifically Make and g++)
    [Windows Users Only]
        You can get MinGW (Minimal GNU for Windows) which provides you with the aforementioned suite of tools
        https://osdn.net/projects/mingw/releases/
        
## How to build:

    1.  Clone Repository into arbitrary directory.
        1_1. git clone https://github.com/DerekBoucher/FuuGBemu.git
    2.  Open the command line or Terminal from the DIRECTORY of the CLONED repository.
    3.  make debug

## Emulation Accuracy Testing

	The following are tests performed on the emulator to verify its accuracy.
	All tests used are validated test roms that have been tested on real hardware.
	Blargg's test rom suite: https://github.com/retrio/gb-test-roms
	Gekkio's test rom repo: https://github.com/Gekkio/mooneye-gb/tree/master/tests

## Blargg's CPU Instruction Tests
| Test 		| Fail/Pass |
|------			|-------|
|01-special		| Fail	|
|02-interrupts		| Pass	|
|03-op sp,hl		| Fail	|
|04-op r,imm		| Fail	|
|05-op rp		| Fail	|
|06-ld r,r		| Fail	|
|07-jr,jp,call,ret,rst	| Fail	|
|08-misc instrs		| inf. loop? |
|09-op r,r		| Fail	|
|10-bit ops		| Fail	|
|11-op a,(hl)		| Fail	|