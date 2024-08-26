# Compiler command
CXX=/opt/homebrew/Cellar/gcc/14.1.0_2/bin/g++-14
CXXFLAGS=-std=c++20 -g -I/opt/homebrew/include -I./wolfie -I./xml
LDFLAGS=-L/opt/homebrew/lib -lboost_system -lboost_thread-mt -lboost_program_options

# Directories containing the source files
WOLFIE_SRC_DIRS=wolfie xml msg
ROSEBUD_SRC_DIRS=rosebud xml msg

WOLFIE_SOURCES=$(wildcard $(foreach dir, $(WOLFIE_SRC_DIRS), $(dir)/*.cpp))
ROSEBUD_SOURCES=$(wildcard $(foreach dir, $(ROSEBUD_SRC_DIRS), $(dir)/*.cpp))

# Target output
WOLFIE_TARGET=build/wolfie
ROSEBUD_TARGET=build/rosebud

# rules
all: $(WOLFIE_TARGET) $(ROSEBUD_TARGET)
wolfie: $(WOLFIE_TARGET)
rosebud: $(ROSEBUD_TARGET)

# Rule to build the target directly
$(WOLFIE_TARGET):
	$(CXX) $(CXXFLAGS) -I./wolfie -I./xml $(LDFLAGS) $(WOLFIE_SOURCES) -o $(WOLFIE_TARGET)

$(ROSEBUD_TARGET):
	$(CXX) $(CXXFLAGS) -I./rosebud -I./xml $(LDFLAGS) $(ROSEBUD_SOURCES) -o $(ROSEBUD_TARGET)
# Clean rule
clean:
	rm -rf build/* 
