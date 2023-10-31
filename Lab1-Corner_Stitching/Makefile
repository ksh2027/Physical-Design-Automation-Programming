all: Lab1

CXX = g++

LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3

Lab1: main.o
	$(CXX) $(LINKFLAGS) main.o -o Lab1

main.o: main.cpp
	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c

clean:
	rm -rf *.o *.gch Lab1






