# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++20 -g
LDFLAGS = -lboost_system -lboost_thread -lboost_program_options -lprotobuf -pthread

# Directories
ORDER_PLACER_SRC_DIRS = . ../althea/wolfie ../althea/proto

# Source Files
ORDER_PLACER_SOURCES = $(wildcard $(foreach dir, $(ORDER_PLACER_SRC_DIRS), $(dir)/*.cpp))

# Protobuf Configuration
PROTO_DIR = ../althea/proto
PROTO_SRC = $(wildcard $(PROTO_DIR)/*.proto)
PROTO_CPP = $(PROTO_SRC:.proto=.pb.cc)
PROTO_HDR = $(PROTO_SRC:.proto=.pb.h)

# Target
ORDER_PLACER_TARGET = build/order_placer

# Default Target
all: $(ORDER_PLACER_TARGET)

# Generate Protobuf Files
%.pb.cc %.pb.h: %.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $<

# Build Order Placer Executable
$(ORDER_PLACER_TARGET): $(ORDER_PLACER_SOURCES) $(PROTO_CPP)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(ORDER_PLACER_SOURCES) $(PROTO_CPP) -o $@

# Clean Build Artifacts	
clean:
	rm -rf build/*
	rm -f $(PROTO_DIR)/*.pb.cc $(PROTO_DIR)/*.pb.h

