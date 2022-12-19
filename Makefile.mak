cc = gcc
ccflags = -g -I. -x c++ -o main -Wall -O2 -std=c++11
depens = amazons.hpp Board.hpp Board.cpp \
		UI.hpp UI.cpp amazons.ccp

.PHONY clean

amazons: $(depens)
	$(cc) ${ccflags} $^ -o $@

clean:
	/bin/rm -f *.o amazons