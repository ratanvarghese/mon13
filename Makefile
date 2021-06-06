.POSIX:
.SUFFIXES:


CC=gcc

CFLAGS=-std=c99
TESTCFLAGS=-fprofile-arcs -ftest-coverage

THEFTDIR=./theft
TESTLDLIBS=-ltheft -lgcov
TESTLFLAGS=-L$(THEFTDIR)/build/ --coverage $(TESTLDLIBS)

all: test

mon13.o: mon13.c mon13.h cal.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) mon13.c

cal.o: cal.c mon13.h cal.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) cal.c

test.o: test.c mon13.h known.h
	$(CC) -c $(CFLAGS) $(TESTCFLAGS) test.c -I./ -I$(THEFTDIR)/inc

test: mon13.o cal.o test.o
	$(CC) -o test mon13.o cal.o test.o $(TESTLFLAGS)
	./test > test.log
	gcov mon13.c >> test.log

clean:
	rm *.o test test.log *.gcov *.gcda *.gcno