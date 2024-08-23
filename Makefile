# Compiler command
CXX=/opt/homebrew/Cellar/gcc/14.1.0_2/bin/g++-14
CXXFLAGS=-std=c++17 -I/opt/homebrew/include -I./wolfie -I./xml
LDFLAGS=-L/opt/homebrew/lib -lboost_system -lboost_thread-mt

# Directories containing the source files
SRC_DIRS=wolfie xml msg

# Find all .cpp files in the source directories
SOURCES=$(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/*.cpp))

# Target output
TARGET=build/mware

# Default rule
all: $(TARGET)

# Rule to build the target directly
$(TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) -o $(TARGET)

# Clean rule
clean:
	rm -rf $(TARGET)
