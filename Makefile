.POSIX:
.SUFFIXES:


CC=gcc

CFLAGS=-std=c99 -fPIC -Wall -Werror
#CFLAGS=-std=c99 -ggdb
#TESTCFLAGS=-fprofile-arcs -ftest-coverage

THEFTDIR=./theft
#TESTLDLIBS=-ltheft -lgcov -lmon13
TESTLDLIBS=-ltheft -lmon13 -lm
#TESTLFLAGS=-L$(THEFTDIR)/build/ -L./ --coverage $(TESTLDLIBS)
TESTLFLAGS=-L$(THEFTDIR)/build/ -L. $(TESTLDLIBS)

all: libmon13.so libmon13.a test

cal.o: cal.c mon13.h cal.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) cal.c

calc.o: calc.c mon13.h cal.h calc.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) calc.c

format.o: format.c mon13.h cal.h calc.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) format.c

libmon13.a: cal.o calc.o format.o
	ar rcs libmon13.a cal.o calc.o format.o

libmon13.so: cal.o calc.o format.o
	$(CC) -shared cal.o calc.o format.o -o libmon13.so

test.o: test.c mon13.h known.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) test.c -I./ -I$(THEFTDIR)/inc

test: libmon13.so test.o
	$(CC) -o test test.o $(TESTLFLAGS)
	LD_LIBRARY_PATH=. ./test > test.log
#	gcov calc.c >> test.log

clean:
#	rm *.o test test.log *.gcov *.gcda *.gcno *.so *.a
	rm *.o test test.log *.so *.a