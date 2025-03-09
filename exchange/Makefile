CXX = g++
CXXFLAGS = -std=c++20 -g
LDFLAGS = -lboost_system -lboost_thread -lboost_program_options -lprotobuf -pthread

EXCHANGE_SRC_DIRS = . ../althea/wolfie ../althea/proto
EXCHANGE_SOURCES = $(wildcard $(foreach dir, $(EXCHANGE_SRC_DIRS), $(dir)/*.cpp))

PROTO_DIR = ../althea/proto
PROTO_SRC = $(wildcard $(PROTO_DIR)/*.proto)
PROTO_CPP = $(PROTO_SRC:.proto=.pb.cc)
PROTO_HDR = $(PROTO_SRC:.proto=.pb.h)

EXCHANGE_TARGET = build/exchange

all: $(EXCHANGE_TARGET)

%.pb.cc %.pb.h: %.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $<

$(EXCHANGE_TARGET): $(EXCHANGE_SOURCES) $(PROTO_CPP)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(EXCHANGE_SOURCES) $(PROTO_CPP) -o $@

clean:
	rm -rf build/*
	rm -f $(PROTO_DIR)/*.pb.cc $(PROTO_DIR)/*.pb.h

