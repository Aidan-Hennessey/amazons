cc = g++
ccflags = -g -I. -x c++ -o main -Wall -O2 -std=c++14 -pthread
depens = amazons.hpp amazons.cpp Board.hpp Board.cpp \
		UI.hpp UI.cpp ai.cpp ai.hpp MoveTree.hpp MoveTree.cpp
all = amazons tests

.PHONY: clean

amazons: $(depens)
	$(cc) ${ccflags} $^ -o $@

tests: $(depens) tests.cpp
	$(cc) ${ccflags} -DTESTS $^ -o $@

clean:
	/bin/rm -f *.o $(all)