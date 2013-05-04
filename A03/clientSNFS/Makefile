CC = gcc

CFLAG = -Wall `pkg-config fuse --cflags --libs`

target : clientSNFS 

clientSNFS: clientSNFS.c
	$(CC) $(CFLAG) -o $@ $<


clean:
	rm clientSNFS
