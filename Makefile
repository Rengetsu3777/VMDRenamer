all: test

clean:
	rm -f *.o main

test: main.cpp vmd.cpp vmd.h
	g++ -c -g -o main.o main.cpp
	g++ -c -g -o vmd.o vmd.cpp
	g++ -o main main.o vmd.o -g