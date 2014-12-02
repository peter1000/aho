.PHONY: clean

CC=gcc
CFLAGS=-c -std=c11 -Wall
LDFLAGS=-Wall
PROG=run-tests
OBJS=aho.o aho.test.o common.o main.o primes.o strbuf.o strbuf.test.o test.o generic/hashtable.test.o

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} $^

%.o: %.c %.h
	${CC} ${CFLAGS} -o $@ $<

%.test.o: %.test.c %.h
	${CC} ${CFLAGS} -o $@ $<

clean:
	rm -rf ${PROG} *.o generic/*.o
