CXX=g++
CXXFLAGS=-std=c++20 -g 
LDFLAGS=-lboost_system -lboost_thread -lboost_program_options -lprotobuf -pthread

WOLFIE_SRC_DIRS=wolfie
ROSEBUD_SRC_DIRS=rosebud

WOLFIE_SOURCES=$(wildcard $(foreach dir, $(WOLFIE_SRC_DIRS), $(dir)/*.cpp))
ROSEBUD_SOURCES=$(wildcard $(foreach dir, $(ROSEBUD_SRC_DIRS), $(dir)/*.cpp))

ROSEBUD_TEST_SOURCES = $(filter-out rosebud/run.cpp, $(ROSEBUD_SOURCES))

WOLFIE_TARGET=build/wolfie
ROSEBUD_TARGET=build/rosebud

PROTO_DIR=proto
PROTO_SRC=$(wildcard $(PROTO_DIR)/*.proto)
PROTO_CPP=$(PROTO_SRC:.proto=.pb.cc)
PROTO_HDR=$(PROTO_SRC:.proto=.pb.h)


GTEST_LIBS=-lgtest -lgtest_main
TEST_SOURCES=$(wildcard test/*.cpp)
TEST_TARGET=build/test_suite

all: $(WOLFIE_TARGET) $(ROSEBUD_TARGET)
wolfie: $(WOLFIE_TARGET)
rosebud: $(ROSEBUD_TARGET)

%.pb.cc %.pb.h: %.proto
	protoc -I=$(PROTO_DIR) --cpp_out=$(PROTO_DIR) $<

$(WOLFIE_TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(WOLFIE_SOURCES) -o $(WOLFIE_TARGET)

$(ROSEBUD_TARGET): $(ROSEBUD_SOURCES) $(PROTO_CPP)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(ROSEBUD_SOURCES) $(PROTO_CPP) -o $(ROSEBUD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) $(ROSEBUD_TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(GTEST_LIBS) $(TEST_SOURCES) $(ROSEBUD_TEST_SOURCES) -o $(TEST_TARGET)

clean:
	rm -rf build/* $(PROTO_DIR)/*.pb.cc $(PROTO_DIR)/*.pb.h
