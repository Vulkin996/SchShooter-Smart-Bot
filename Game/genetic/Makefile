PROGRAM=bot
CXX       = g++
CXXFLAGS  = -g -std=c++17 -Wall

$(PROGRAM): bot.o bot.h
	$(CXX) -o $(PROGRAM) bot.o bot.h

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)
