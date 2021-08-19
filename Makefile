.POSIX:
.SUFFIXES:


CC=gcc

CFLAGS=-std=c99 -fPIC
#CFLAGS=-std=c99 -ggdb
#TESTCFLAGS=-fprofile-arcs -ftest-coverage

THEFTDIR=./theft
#TESTLDLIBS=-ltheft -lgcov -lmon13
TESTLDLIBS=-ltheft -lmon13
#TESTLFLAGS=-L$(THEFTDIR)/build/ -L./ --coverage $(TESTLDLIBS)
TESTLFLAGS=-L$(THEFTDIR)/build/ -L. $(TESTLDLIBS)

all: libmon13.so libmon13.a test

cal.o: cal.c mon13.h cal.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) cal.c

mon13.o: cal.o mon13.c mon13.h cal.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) mon13.c

libmon13.a: cal.o mon13.o
	ar rcs libmon13.a cal.o mon13.o

libmon13.so: cal.o mon13.o
	$(CC) -shared cal.o mon13.o -o libmon13.so

test.o: test.c mon13.h known.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) test.c -I./ -I$(THEFTDIR)/inc

test: libmon13.so test.o
	$(CC) -o test test.o $(TESTLFLAGS)
	LD_LIBRARY_PATH=. ./test > test.log
#	gcov mon13.c >> test.log

clean:
#	rm *.o test test.log *.gcov *.gcda *.gcno *.so *.a
	rm *.o test test.log *.so *.a