CC = gcc
CPP = g++

cmmheap-test : cmmheap-test.c
	$(CC) -O2 -Wall -Wno-unused-function -o cmmheap-test cmmheap-test.c

clean :
	rm -f cmmheap-test

