.POSIX:
.SUFFIXES:

MON13DIR=..
THEFTDIR=./theft


CC=gcc
CFLAGS=-std=c99 -fPIC -Wall -Werror
#TESTCFLAGS=-fprofile-arcs -ftest-coverage


TESTLIBS=-ltheft -lmon13 -lm
TESTLFLAGS=-L$(THEFTDIR)/build/ -L$(MON13DIR) $(TESTLIBS)
TESTINC=-I./ -I$(MON13DIR) -I$(THEFTDIR)/inc

all: test

test.o: test.c $(MON13DIR)/mon13.h known.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) test.c $(TESTINC)

test: $(MON13DIR)/libmon13.so test.o
	$(CC) -o test test.o $(TESTLFLAGS)
	LD_LIBRARY_PATH=$(MON13DIR) ./test > test.log

clean:
	rm *.o test test.log