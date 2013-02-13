CC = gcc
CFLAGS = -Wall
 
all: wtc

wtc:
	$(CC) $(CFLAGS) -o wtc wtc.c -lrt

clean:
	rm -f *.o
	rm wtc
