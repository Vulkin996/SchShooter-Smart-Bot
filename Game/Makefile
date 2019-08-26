appname := SchShooter.out

CXX := g++
CXXFLAGS := -Wall -ggdb3 -std=c++17 -lBox2D -lGL -lglut -lGLU -lopenal -lalut -lm -lfann

srcfiles := $(shell find source -maxdepth 1 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

all: $(appname)

$(appname): $(objects)
	$(CXX) $(LDFLAGS) -o $(appname) $(objects) $(CXXFLAGS)


clean:
	rm -f $(objects) $(appname)

dist-clean: clean
	rm -f *~ .depend
