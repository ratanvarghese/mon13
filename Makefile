.POSIX:
.SUFFIXES:

CC=gcc
CFLAGS=-std=c99
THEFTDIR=./theft

all: test

mon13.o: mon13.c mon13.h cal.h
	$(CC) -c $(CFLAGS) mon13.c

cal.o: cal.c mon13.h cal.h
	$(CC) -c $(CFLAGS) cal.c

test.o: test.c mon13.h
	$(CC) -c $(CFLAGS) test.c -I./ -I$(THEFTDIR)/inc

test: mon13.o cal.o test.o
	$(CC) -o test mon13.o cal.o test.o -L$(THEFTDIR)/build/ -ltheft
	./test > test.log

clean:
	rm *.o test