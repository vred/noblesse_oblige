CC = gcc
CFLAGS = -Wall
 
all: wtc

wtc:
	$(CC) $(CFLAGS) -o wtc wtc.c -lrm

clean:
	rm -f *.o
	rm wtc
