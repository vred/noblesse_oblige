CC = gcc
CFLAGS = -Wall
 
all: wtc

wtc:
	$(CC) $(CFLAGS) -o wtc wtc.c -lrt -lpthread

clean:
	rm -f *.o
	rm wtc
