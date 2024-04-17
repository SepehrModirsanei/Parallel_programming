CC=g++

CFLAGS=-c

INCDIR=-I/opt/intel/ipp/include

LIBDIR=-L/opt/intel/ipp/lib

CXXFLAG=-c -msse4

all: main

main: main.o 
	$(CC) main.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) $(INCDIR) $(CXXFLAG) main.cpp

clean:
	rm -f *o main