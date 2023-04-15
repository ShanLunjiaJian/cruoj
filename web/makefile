CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lpthread -lmariadbcpp

local=$(wildcard *.cpp)
app = $(wildcard app/*.cpp)
app_libs = $(wildcard app/libs/*.cpp)


.PHONY: main
main: 
	g++ -g $(local) $(app) $(app_libs) -o main -std=c++17 -lpthread -lmariadbcpp -DCROW_ENABLE_DEBUG