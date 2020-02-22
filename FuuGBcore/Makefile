BUILDDIR := ../build
OBJDIR= ../obj
OBJ=
SRCS = $(wildcard src/*.cpp)
LIBEXT=
LIB_ACTION=

ifeq ($(OSFLAG),WIN)
	SRCS+=$(wildcard Windows/*.cpp)
	LIBEXT=dll
	CXXFLAGS += -DFUUGB_BUILD_DLL
	OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst Windows/%.cpp, %.o, $(SRCS))))
	LIB_ACTION=-shared
endif

ifeq ($(OSFLAG),MACOS)
	SRCS+=$(wildcard MacOS/*.cpp)
	LIBEXT=dylib
	FRAMEWORKS=-framework Foundation -F/Library/Frameworks
	OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst MacOS/%.cpp, %.o, $(SRCS))))
	LIB_ACTION=-dynamiclib
endif

$(BUILDDIR)/$(LIB).$(LIBEXT): $(OBJS)
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) $(LIB_ACTION) -install_name ./FuuGBcore.dylib -o $@ $^ $(LIBS)

$(OBJS): $(SRCS)
	@$(CXX) $(CXXFLAGS) -c $^
	@mkdir -p $(OBJDIR)
	@cp *.o $(OBJDIR)
	@$(RM) -f *.o

.PHONY: clean
clean:
	@echo $(OBJS)
	@$(RM) -f *.o
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)
