appname := myshell

CXX := g++
CXXFLAGS := -x c++ -std=c++11

srcfiles := $(shell find . -maxdepth 1 -name "*.cpp" -o -name "*.c")
objects := $(filter %.o,$(patsubst %.cpp,%.o,$(srcfiles)) \
    $(patsubst %.c,%.o,$(srcfiles)))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(srcfiles) -o $(appname)

clean:
	rm -f $(objects)
