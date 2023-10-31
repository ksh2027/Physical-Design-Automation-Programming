all: Lab2

CXX = g++

LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3

Lab2: main.o
	$(CXX) $(LINKFLAGS) main.o -o Lab2

main.o: main.cpp
	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c

clean:
	rm -rf *.o *.gch Lab2






