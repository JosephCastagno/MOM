CXX=g++
CXXFLAGS=-std=c++20 -g
LDFLAGS=-lboost_system -lboost_thread -lboost_program_options

ORDER_PLACER_SRC_DIRS=. ../althea/wolfie ../althea/xml ../althea/msg 

ORDER_PLACER_SOURCES=$(wildcard $(foreach dir, $(ORDER_PLACER_SRC_DIRS), $(dir)/*.cpp))

ORDER_PLACER_TARGET=order_placer

all: $(ORDER_PLACER_TARGET)

$(ORDER_PLACER_TARGET):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(ORDER_PLACER_SOURCES) -o $(ORDER_PLACER_TARGET)

clean:
	rm order_placer 
