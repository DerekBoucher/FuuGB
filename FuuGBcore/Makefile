BUILDDIR := ../build
OBJDIR= ../obj
SRCS = $(wildcard src/*.cpp)
LIBEXT=
L=

ifeq ($(OSFLAG),WIN)
	SRCS+=$(wildcard Windows/*.cpp)
	LIBEXT=dll
	L =-Wl,-Bstatic -lSDL2main -lSDL2 -lsetupapi -lhid -lpthread -lmingw32 -mwindows -lm -lcomdlg32 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32  -loleaut32 -lshell32 -lversion -luuid -static-libgcc
endif

OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst Windows/%.cpp, %.o, $(SRCS))))

$(BUILDDIR)/$(LIB).$(LIBEXT): $(OBJS)
	@mkdir -p $(BUILDDIR)
	@$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(L)
$(OBJS): $(SRCS)
	@$(CXX) $(CXXFLAGS) -c $^
	@mkdir -p $(OBJDIR)
	@cp *.o $(OBJDIR)
	@$(RM) -f *.o

.PHONY: clean
clean:
	@$(RM) -f *.o
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)