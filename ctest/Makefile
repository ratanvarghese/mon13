.POSIX:
.SUFFIXES:

MON13DIR=..
THEFTDIR=./theft

MON13LIB=mon13c

CC=gcc
CFLAGS=-std=c99 -fPIC -Wall -Werror
#TESTCFLAGS=-fprofile-arcs -ftest-coverage


TESTLIBS=-ltheft -l$(MON13LIB) -lm
TESTLFLAGS=-L$(THEFTDIR)/build/ -L$(MON13DIR)/zig-out/lib/ $(TESTLIBS)
TESTINC=-I./ -I$(MON13DIR)/include -I$(THEFTDIR)/inc

all: test

test.o: test.c $(MON13DIR)/include/mon13.h known.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) test.c $(TESTINC)

test: $(MON13DIR)/zig-out/lib/lib$(MON13LIB).so test.o
	$(CC) -o test test.o $(TESTLFLAGS)
	LD_LIBRARY_PATH=$(MON13DIR)/zig-out/lib/ ./test > test.log

clean:
	rm *.o test test.log