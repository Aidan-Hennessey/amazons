cc = g++
ccflags = -g -I. -x c++ -o main -Wall -O2 -std=c++14
depens = amazons.hpp amazons.cpp Board.hpp Board.cpp \
		UI.hpp UI.cpp MoveTree.hpp MoveTree.cpp
all = amazons small_amazons tiny_amazons tests

.PHONY: clean

amazons: $(depens)
	$(cc) ${ccflags} $^ -o $@

small_amazons: $(depens)
	$(cc) ${ccflags} -DSMALL $^ -o $@

tiny_amazons: $(depens)
	$(cc) ${ccflags} -DTINY $^ -o $@

tests: $(depens) tests.cpp
	$(cc) ${ccflags} -DTESTS $^ -o $@

clean:
	/bin/rm -f *.o $(all)