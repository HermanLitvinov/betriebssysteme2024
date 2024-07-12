$CC = gcc
$CFLAGS = -pedantic -Wall -Werror -std=c11 -D_XOPEN_SOURCE=700 

.PHONY: all clean

all: test test-ref

clean: 
	rm -rf test.o test test-ref halde.o

test-ref: test.o halde-ref.o halde.h
	$(CC) $(CFLAGS) -o test-ref test.o halde-ref.o

test: test.o halde.o halde.h
	$(CC) $(CFLAGS) -o test test.o halde.o

test.o: test.c halde.h
	$(CC) $(CFLAGS) -c test.c -o test.o

halde.o: halde.c halde.h
	$(CC) $(CFLAGS) -c halde.c -o halde.o