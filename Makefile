# Compilation/Linking variables
CXX = g++
COMPILE_FLAGS = -std=c++17 -Wall -pthread $(shell wx-config --cppflags) -Iinclude
LIBS = $(shell wx-config --libs) -lSDL2
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin
BIN_NAME = FuuGBemu
CPP_SRC_EXT = cpp
CPP_SOURCES = $(shell find $(SRC_PATH) -name '*.$(CPP_SRC_EXT)' | sort -k 1nr | cut -f2-)

# Platform specific flags
ifeq ($(OS),Windows_NT)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		COMPILE_FLAGS += -I /usr/include/SDL2
		COMPILE_FLAGS += -I /usr/include/gtk-3.0
		COMPILE_FLAGS += -I /usr/include/glib-2.0
		COMPILE_FLAGS += -I /usr/lib/x86_64-linux-gnu/glib-2.0/include
		COMPILE_FLAGS += -I /usr/include/pango-1.0
		COMPILE_FLAGS += -I /usr/include/harfbuzz
		COMPILE_FLAGS += -I /usr/include/cairo
		COMPILE_FLAGS += -I /usr/include/gdk-pixbuf-2.0
		COMPILE_FLAGS += -I /usr/include/atk-1.0
		COMPILE_FLAGS += -DFUUGB_SYSTEM_LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
	endif
endif

OBJECTS = $(CPP_SOURCES:$(SRC_PATH)/%.$(CPP_SRC_EXT)=$(BUILD_PATH)/%.o)

# Rules
.PHONY: debug release makeDirs clean

debug: makeDirs
	@echo "Building debug x86_64..."
	@$(eval export DEBUG_FLAGS = -g -DFUUGB_DEBUG)
	@$(MAKE) $(BIN_PATH)/$(BIN_NAME)

release: makeDirs
	@echo "Building release x86_64..."
	@$(MAKE) $(BIN_PATH)/$(BIN_NAME)

makeDirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

$(BIN_PATH)/$(BIN_NAME) : $(OBJECTS)
	@echo "Linking $^ -> $@"
	@$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(CPP_SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(COMPILE_FLAGS) $(DEBUG_FLAGS) -MP -MMD -c $< -o $@