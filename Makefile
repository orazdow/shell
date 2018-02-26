appname := myshell

CXX := g++
CXXFLAGS := -x c++ -std=c++11

srcfiles := $(shell find . -maxdepth 1 -name "*.cpp" -o -name "*.c")

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(srcfiles) -o $(appname)

clean:
	rm -f $(objects)

