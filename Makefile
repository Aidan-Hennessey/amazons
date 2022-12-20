cc = g++
ccflags = -g -I. -x c++ -o main -Wall -O2 -std=c++11
depens = amazons.hpp amazons.cpp Board.hpp Board.cpp \
		UI.hpp UI.cpp 

.PHONY: clean

amazons: $(depens)
	$(cc) ${ccflags} $^ -o $@

ai: ai.cpp
	$(cc) ${ccflags} $< -o $@

tests: $(depens) tests.cpp
	$(cc) ${ccflags} -TESTS $^ -o $@

clean:
	/bin/rm -f *.o amazons