BUILDDIR = ../build
TARGET = FuuGBemu
SRCS = $(wildcard *.cpp)
SAND_INCLUDE_PATH = -I../FuuGBcore/include
SAND_LIB_PATH =
LIBEXT=

ifeq ($(OSFLAG),WIN)
	SAND_INCLUDE_PATH += -I../FuuGBcore/external/SDL2/Windows/SDL2-2.0.10/i686-w64-mingw32/include/SDL2
	SAND_LIB_PATH = -L../FuuGBcore/external/SDL2/Windows/SDL2-2.0.10/i686-w64-mingw32/lib
	LIBEXT=dll
	L =-Wl,-Bstatic -lSDL2main -lSDL2 -lsetupapi -lhid -lpthread -lmingw32 -lm -lcomdlg32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32  -loleaut32 -lshell32 -lversion -luuid -static-libgcc
endif

ifeq ($(OSFLAG),MACOS)
	SAND_INCLUDE_PATH +=-I../FuuGBcore/external/SDL2/MacOS/SDL2.framework/Headers
	SAND_LIB_PATH = -F../FuuGBcore/external/SDL2/MacOS/SDL2.framework
	LIBEXT=dylib
endif

$(BUILDDIR)/$(TARGET): $(SRCS)
	@echo $(SAND_INCLUDE_PATH)
	@$(CXX) $^ -o $@ $(CXXFLAGS) $(L) $(SAND_INCLUDE_PATH) $(SAND_LIB_PATH) -Bdynamic $(BUILDDIR)/FuuGBcore.$(LIBEXT) -rpath ./

.PHONY: clean
clean:
	@$(RM) -f *.o
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)