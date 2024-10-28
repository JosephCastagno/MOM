CXX=g++
CXXFLAGS=-std=c++20 -g 
LDFLAGS=-lboost_system -lboost_thread -lboost_program_options

WOLFIE_SRC_DIRS=wolfie xml msg
ROSEBUD_SRC_DIRS=rosebud xml msg

WOLFIE_SOURCES=$(wildcard $(foreach dir, $(WOLFIE_SRC_DIRS), $(dir)/*.cpp))
ROSEBUD_SOURCES=$(wildcard $(foreach dir, $(ROSEBUD_SRC_DIRS), $(dir)/*.cpp))

ROSEBUD_TEST_SOURCES = $(filter-out rosebud/run.cpp, $(ROSEBUD_SOURCES))

WOLFIE_TARGET=build/wolfie
ROSEBUD_TARGET=build/rosebud

GTEST_LIBS=-lgtest -lgtest_main
TEST_SOURCES=$(wildcard test/*.cpp)
TEST_TARGET=build/test_suite

all: $(WOLFIE_TARGET) $(ROSEBUD_TARGET)
wolfie: $(WOLFIE_TARGET)
rosebud: $(ROSEBUD_TARGET)

$(WOLFIE_TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(WOLFIE_SOURCES) -o $(WOLFIE_TARGET)

$(ROSEBUD_TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(ROSEBUD_SOURCES) -o $(ROSEBUD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) $(ROSEBUD_TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(GTEST_LIBS) $(TEST_SOURCES) $(ROSEBUD_TEST_SOURCES) -o $(TEST_TARGET)

clean:
	rm -rf build/* 
