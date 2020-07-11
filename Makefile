# Define compiler and linker
CXX = g++
LIBS =
LIBTOCP =

# Determine target platform
OS_FLAG :=
ifeq ($(OS),Windows_NT)
	OSFLAG += WINDOWS
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += OSX
	endif
endif

# Define source, build and output paths
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# Output name
BIN_NAME = FuuGBemu

# File extensions to scan for
CPP_SRC_EXT = cpp
OBJ_C_SRC_EXT = m
OBJ_CPP_SRC_EXT = mm

# Find all source files in the source directory, sorted by
# most recently modified
CPP_SOURCES = $(shell find $(SRC_PATH) -name '*.$(CPP_SRC_EXT)' | sort -k 1nr | cut -f2-)
OBJ_C_SOUCRES =
OBJ_CPP_SOURCES = 
ifeq ($(OSFLAG), OSX)
	OBJ_C_SOUCRES += $(shell find $(SRC_PATH)/osx -name '*.$(OBJ_C_SRC_EXT)' | sort -k 1nr | cut -f2-)
	OBJ_CPP_SOURCES += $(shell find $(SRC_PATH)/osx -name '*.$(OBJ_CPP_SRC_EXT)' | sort -k 1nr | cut -f2-)
endif

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(CPP_SOURCES:$(SRC_PATH)/%.$(CPP_SRC_EXT)=$(BUILD_PATH)/%.o)
ifeq ($(OSFLAG), OSX)
	OBJECTS += $(OBJ_C_SOUCRES:$(SRC_PATH)/osx/%.$(OBJ_C_SRC_EXT)=$(BUILD_PATH)/%.o)
	OBJECTS += $(OBJ_CPP_SOURCES:$(SRC_PATH)/osx/%.$(OBJ_CPP_SRC_EXT)=$(BUILD_PATH)/%.o)
endif

# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# flags #
COMPILE_FLAGS = -std=c++11 -Wall -Wextra -pthread
ifeq ($(OSFLAG), LINUX)
	COMPILE_FLAGS += -DFUUGB_SYSTEM_LINUX
endif
ifeq ($(OSFLAG), OSX)
	COMPILE_FLAGS += -DFUUGB_SYSTEM_MACOS -x objective-c++
	LIBS += -framework Cocoa -Llib/sdl2/osx
	LIBTOCP = lib/sdl2/osx/libSDL2.dylib
endif
ifeq ($(OSFLAG), WINDOWS)
	COMPILE_FLAGS += -DFUUGB_SYSTEM_WINDOWS -DUNICODE -D_UNICODE
	LIBS += -Llib/sdl2/windows -lcomdlg32
	LIBTOCP = lib/sdl2/windows/SDL2.dll
endif

INCLUDES = -I include/ -I /usr/local/include -I lib/sdl2/include
# Space-separated pkg-config libraries used by this project
LIBS += -lSDL2 -lpthread

.PHONY: default_target
default_target: debug

.PHONY: debug
debug: export COMPILE_FLAGS := $(COMPILE_FLAGS) -DFUUGB_DEBUG -g3
debug: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
debug: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)
	@cp $(LIBTOCP) $(BIN_PATH)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) -o $@ ${LIBS}

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(CPP_SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

$(BUILD_PATH)/%.o: $(SRC_PATH)/osx/%.$(OBJ_C_SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

$(BUILD_PATH)/%.o: $(SRC_PATH)/osx/%.$(OBJ_CPP_SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: target
target:
	@echo $(OSFLAG)
	@echo $(LIBS)