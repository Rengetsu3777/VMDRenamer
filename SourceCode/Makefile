all: test

clean:
	rm -f *.o main

test: mainVmdRenamer.cpp mainVmdRenamer.h vmd.cpp vmd.h logger.cpp logger.h stringFormat.h
	g++ -std=c++17 -c -g -o logger.o logger.cpp
	g++ -std=c++17 -c -g -o mainVmdRenamer.o mainVmdRenamer.cpp
	g++ -std=c++17 -c -g -o vmd.o vmd.cpp
	g++ -std=c++17 -o mainVmdRenamer mainVmdRenamer.o vmd.o logger.o -g