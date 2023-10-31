all: Lab4

CXX = g++

LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O0 -g

Lab4: main.o
	$(CXX) $(LINKFLAGS) main.o -o Lab4

main.o: main.cpp
	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c

clean:
	rm -rf *.o *.gch Lab4






