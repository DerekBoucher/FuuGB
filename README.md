# FuuGBemu
![Windows 64-Bit Build](https://github.com/DerekBoucher/FuuGBemu/workflows/Windows%2064-Bit%20Build/badge.svg?event=push)    ![MacOS 64-Bit Build](https://github.com/DerekBoucher/FuuGBemu/workflows/MacOS%2064-Bit%20Build/badge.svg?event=push)    ![languages](https://img.shields.io/github/languages/top/DerekBoucher/FuuGBemu)

Cross platform C/C++ GameBoy emulator using SDL2 framework.

Algorithm for the PPU and Memory Management are taken from http://www.codeslinger.co.uk/pages/projects/gameboy.html

All credit goes to codeslinger!

Early Alpha Development

Build Script System provided by Gradle! https://github.com/gradle/gradle

## Build Prerequisites
    1.  Gradle build system must be installed on your machine
    2.  To install gradle:

    MAC OS X
        3_1. Brew package manager:
            3_1_1. brew install gradle

            OR

        3_2. Visit https://gradle.org/releases/ and download the binary or complete package.
    
    WINDOWS
        3_3. Visit https://gradle.org/install/ and follow the directives for "Microsoft Windows users"
    
    4.  Supported toolchains: Visual C++, Gcc, Clang.


## How to build:

    1.  Clone Repository into arbitrary directory.
    2.  Open the command line or Terminal from the DIRECTORY of the CLONED repository.
    3.  Type in the following command:
        3_1.    gradle build

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