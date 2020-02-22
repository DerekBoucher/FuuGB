BUILDDIR := ../build
OBJDIR= ../obj
OBJ=
SRCS = $(wildcard src/*.cpp)
LIBEXT=

ifeq ($(OSFLAG),WIN)
	SRCS+=$(wildcard Windows/*.cpp)
	LIBEXT=dll
	CXXFLAGS += -DFUUGB_BUILD_DLL
	OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst Windows/%.cpp, %.o, $(SRCS))))
endif

ifeq ($(OSFLAG),MACOS)
	SRCS+=$(wildcard MacOS/*.mm)
	LIBEXT=dylib
	FRAMEWORKS=-framework Foundation -F/Library/Frameworks
	CXXFLAGS+=
	OBJS = $(patsubst %.o, $(OBJDIR)/%.o, $(patsubst src/%.cpp, %.o, $(patsubst MacOS/%.m, %.o, $(patsubst MacOS/%.mm, %.o, $(SRCS)))))
endif

$(BUILDDIR)/$(LIB).$(LIBEXT): $(OBJS)
	@mkdir -p $(BUILDDIR)
	@$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJS): $(SRCS)
	@$(CXX) $(CXXFLAGS) -c $^
	@mkdir -p $(OBJDIR)
	@cp *.o $(OBJDIR)
	@$(RM) -f *.o

.PHONY: clean makeMacCompatible
clean:
	@$(RM) -f *.o
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)
