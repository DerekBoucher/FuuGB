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

LIBS =-Wl,-Bstatic -lcomdlg32 -lpthread -lSDL2 -lSDL2main

ifeq ($(OSFLAG),WIN)
	INCLUDE_PATH += -I$(SDL_DIR)/Windows/SDL2-2.0.10/i686-w64-mingw32/include/SDL2
	WIN_CXX_FLAGS =-Wl,-Bstatic
	CXX = g++
endif

LIBS =$(WIN_CXX_FLAGS) -lpthread -lSDL2 -lSDL2main

ifeq ($(OSFLAG),MACOS)
	INCLUDE_PATH += -I$(SDL_DIR)/MacOS/SDL2.framework/Headers
endif

LIB_PATH :=
ifeq ($(OSFLAG),WIN)
	LIB_PATH += -L$(SDL_DIR)/Windows/SDL2-2.0.10/i686-w64-mingw32/lib
	LIBS += -lsetupapi -lhid -lmingw32 -mwindows -lm -ldinput8 -lcomdlg32 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32  -loleaut32 -lshell32 -lversion -luuid
endif

LIB = FuuGBcore

CXXFLAGS :=
CXXFLAGS += -std=c++11
CXXFLAGS += $(INCLUDE_PATH) $(LIB_PATH)
CXXFLAGS += -Wall

ifeq ($(OSFLAG),WIN)
	CXXFLAGS += -DFUUGB_SYSTEM_WINDOWS
endif

ifeq ($(OSFLAG),MACOS)
	CXXFLAGS += -DFUUGB_SYSTEM_MACOS
	CXX=g++
endif

LDFLAGS :=

export ROOT_DIR CXX CXXFLAGS LDFLAGS LIBS LIB INCLUDE_PATH LIB_PATH OSFLAG

.PHONY: clean all msvc

all: #GNU
	@echo ========== making rule $@ ==========
	@$(MAKE) --no-print-directory -C FuuGBcore
	@$(MAKE) --no-print-directory -C FuuGBemu
	@cp FuuGBcore/external/boot/DMG_ROM.bin $(BUILDDIR)/DMG_ROM.bin

msvc:
	@$(MAKE) --no-print-directory -C FuuGBcore msvc
	#@$(MAKE) --no-print-directory -C FuuSandBox
	#@cp FuuGBcore/external/boot/DMG_ROM.bin $(BUILDDIR)/DMG_ROM.bin

clean:
	@echo ========== Cleaning... ==========
	@$(MAKE) --no-print-directory -C FuuGBcore clean
	@$(MAKE) --no-print-directory -C FuuGBemu clean
