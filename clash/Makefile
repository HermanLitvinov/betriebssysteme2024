$CC = gcc
$CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

.PHONY: all clean

all: clash

clean:
	rm -f clash clash.o plist.o

clash: clash.o plist.o plist.h
	$(CC) $(CFLAGS) -o clash plist.o clash.o

clash.o: clash.c
	$(CC) $(CFLAGS) -c clash.c -o clash.o

plist.o: plist.c
	$(CC) $(CFLAGS) -c plist.c -o plist.o
