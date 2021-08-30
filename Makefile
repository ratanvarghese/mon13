.POSIX:
.SUFFIXES:


CC=gcc

CFLAGS=-std=c99 -fPIC -Wall -Werror
#CFLAGS=-std=c99 -ggdb
#TESTCFLAGS=-fprofile-arcs -ftest-coverage

#TESTLDLIBS=-ltheft -lgcov -lmon13
#TESTLFLAGS=-L$(THEFTDIR)/build/ -L./ --coverage $(TESTLDLIBS)

all: libmon13.so libmon13.a

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

clean:
	rm *.o *.so *.a