.POSIX:
.SUFFIXES:

CC=gcc
CFLAGS=-std=c99
THEFTDIR=../../theft

all: test

mon13.o: mon13.c mon13.h
	$(CC) -c $(CLFAGS) mon13.c

mon13dat.o: mon13dat.c mon13.h
	$(CC) -c $(CFLAGS) mon13dat.c

test.o: test.c mon13.h
	$(CC) -c $(CLFAGS) test.c -I./ -I$(THEFTDIR)/inc

test: mon13.o mon13dat.o test.o
	$(CC) -o test mon13.o mon13dat.o test.o -L$(THEFTDIR)/build/ -ltheft
	./test

clean:
	rm *.o test