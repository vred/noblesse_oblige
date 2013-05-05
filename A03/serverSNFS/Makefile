CC = gcc

CFLAG = -Wall -lpthread

target : serverSNFS 

clientSNFS: serverSNFS.c
	$(CC) $(CFLAG) -o $@ $<


clean:
	rm serverSNFS
