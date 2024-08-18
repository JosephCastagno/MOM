CXXFLAGS = -std=c++17 -I/opt/homebrew/include -I./wolfie -Ibuild -Wc++11-extensions
LDFLAGS = -L/opt/homebrew/lib -lprotobuf

# Directories
SRC_DIR = wolfie
BUILD_DIR = build

# Protobuf files
PROTO_FILES = $(wildcard $(SRC_DIR)/*.proto)
PROTO_CPP_FILES = $(patsubst $(SRC_DIR)/%.proto, $(BUILD_DIR)/%.pb.cc, $(PROTO_FILES))
PROTO_H_FILES = $(patsubst $(SRC_DIR)/%.proto, $(BUILD_DIR)/%.pb.h, $(PROTO_FILES))

# Source files
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))

# Output binary
TARGET = $(BUILD_DIR)/mware

# Default rule
all: $(TARGET)

# Rule to compile the target
$(TARGET): $(OBJ_FILES) $(PROTO_CPP_FILES)
	$(CXX) $(OBJ_FILES) $(PROTO_CPP_FILES) -I/opt/homebrew/include -Ibuild -o $@ $(LDFLAGS)

# Rule to compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(PROTO_H_FILES)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile .proto files to .pb.cc and .pb.h
$(BUILD_DIR)/%.pb.cc $(BUILD_DIR)/%.pb.h: $(SRC_DIR)/%.proto
	protoc --cpp_out=$(BUILD_DIR) --proto_path=$(SRC_DIR) $<

# Clean rule
clean:
	rm -rf $(BUILD_DIR)/*

# Phony targets
.PHONY: all clean

