# Determine OS
ifeq ($(OS),Windows_NT)
	OSFLAG=WIN
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG=LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG=MACOS
	endif
endif

ROOT_DIR := $(PWD)
CORE_DIR := $(ROOT_DIR)/FuuGBcore
SAND_DIR := $(ROOT_DIR)/FuuSandBox
SDL_DIR	 := external/SDL2
OBJDIR := obj
BUILDDIR = build

INCLUDE_PATH :=
INCLUDE_PATH += -Iinclude
INCLUDE_PATH += -I$(SAND_DIR)

ifeq ($(OSFLAG),WIN)
	INCLUDE_PATH += -I$(SDL_DIR)/Windows/SDL2-2.0.10/i686-w64-mingw32/include/SDL2
endif

LIB_PATH :=
ifeq ($(OSFLAG),WIN)
	LIB_PATH += -L$(SDL_DIR)/Windows/SDL2-2.0.10/i686-w64-mingw32/lib
endif

CXX = g++

LIBS := -lcomdlg32 -lSDL2 -lpthread -lSDL2main -lole32 -loleaut32 -limm32 -lVersion -lGdi32 -lsetupapi -lhid -lWinmm
LIB = FuuGBcore

CXXFLAGS := -g
CXXFLAGS += $(INCLUDE_PATH) $(LIB_PATH)
CXXFLAGS += -Wall

ifeq ($(OSFLAG),WIN)
	CXXFLAGS += -DFUUGB_SYSTEM_WINDOWS -DFUUGB_BUILD_DLL
endif

LDFLAGS :=

export ROOT_DIR CXX CXXFLAGS LDFLAGS LIBS LIB INCLUDE_PATH LIB_PATH OSFLAG

all:
	@$(MAKE) -C FuuGBcore
	@$(MAKE) -C FuuSandBox
	@cp FuuGBcore/external/boot/DMG_ROM.bin $(BUILDDIR)/DMG_ROM.bin

.PHONY: clean
clean:
	@$(MAKE) -C FuuGBcore clean
	@$(MAKE) -C FuuSandBox clean
