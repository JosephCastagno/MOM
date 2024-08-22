CXXFLAGS = -std=c++17 -stdlib=libc++ -E -I./wolfie -I./xml
LDFLAGS = -L/opt/homebrew/lib
CC=/opt/homebrew/Cellar/gcc/14.1.0_2/bin/gcc-14
CXX=/opt/homebrew/Cellar/gcc/14.1.0_2/bin/g++-14

# Directories
SRC_DIRS = wolfie xml
BUILD_DIR = build

# Source files
CPP_FILES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJ_FILES = $(patsubst $(SRC_DIRS)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))

# Output binary
TARGET = $(BUILD_DIR)/mware

# Default rule
all: $(TARGET)

# Rule to compile the target
$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Rule to compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR)/*

# Phony targets
.PHONY: all clean

