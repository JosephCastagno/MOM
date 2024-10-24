CXX=g++
CXXFLAGS=-std=c++20 -g 
LDFLAGS=-lboost_system -lboost_thread -lboost_program_options

EXCHANGE_SRC_DIRS=. ../althea/wolfie ../althea/xml ../althea/msg 

EXCHANGE_SOURCES=$(wildcard $(foreach dir, $(EXCHANGE_SRC_DIRS), $(dir)/*.cpp))

EXCHANGE_TARGET=exchange

all: $(EXCHANGE_TARGET)

$(EXCHANGE_TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(EXCHANGE_SOURCES) -o $(EXCHANGE_TARGET)

clean:
	rm exchange 
