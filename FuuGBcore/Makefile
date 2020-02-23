BUILDDIR := ../build
OBJDIR= ../obj
OBJ=
SRCS = $(wildcard src/*.cpp)
LIBEXT=
LIB_ACTION=
OBJ_MSVC=

ifeq ($(OSFLAG),WIN)
	SRCS+=$(wildcard Windows/*.cpp)
	LIBEXT=dll
	WINDEF=-DFUUGB_BUILD_DLL
	OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst Windows/%.cpp, %.o, $(SRCS))))
	OBJS_MSVC = $(patsubst %.obj, $(OBJDIR)/%.obj, $(patsubst src/%.cpp, %.obj, $(patsubst Windows/%.cpp, %.obj, $(SRCS))))
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
	@echo ========== Linking FuuGBcore... ==========
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CXXFLAGS) $(WINDEF) $(LIB_ACTION) -o $@ -O3 $^ $(LIBS)

$(OBJS): $(SRCS)
	@echo ========== Compiling objects for FuuGBcore... ==========
	@$(CXX) $(CXXFLAGS) $(WINDEF) -c -O3 $^
	@mkdir -p $(OBJDIR)
	@cp *.o $(OBJDIR)
	@$(RM) -f *.o

.PHONY: clean msvc

msvc: $(SRCS)
	@echo $(SRCS)
	@CL /c $(SRCS) 

clean:
	@echo ========== Cleaning FuuGBcore... ==========
	@$(RM) -f *.o
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)
