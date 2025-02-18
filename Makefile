CXX = g++

PROGRAM = gameranger
SOURCES := $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# implementation

.SUFFIXES:      .o .cpp

.cpp.o :
	$(CXX) -c `wx-config --cxxflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):     $(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) `wx-config --libs`

clean:
	rm -f $(OBJECTS) $(PROGRAM)
