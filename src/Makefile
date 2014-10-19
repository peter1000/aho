.PHONY: clean

CC=gcc
CFLAGS=-c -std=c11 -Wall -DDEBUG
LDFLAGS=-Wall
PROG=aho
OBJS=aho.o main.o strbuf.o

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o ${PROG} $^

%.o: %.c %.h
	${CC} ${CFLAGS} -o $@ $<

clean:
	rm -f ${PROG} *.o
