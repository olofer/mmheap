CC = gcc
CPP = g++

all : cmmheap-test mmheap-test

cmmheap-test : cmmheap-test.c
	$(CC) -O2 -Wall -Wno-unused-function -o cmmheap-test cmmheap-test.c

mmheap-test : mmheap-test.cpp
	$(CPP) -O2 -Wall -o mmheap-test mmheap-test.cpp

clean :
	rm -f mmheap-test
	rm -f cmmheap-test
