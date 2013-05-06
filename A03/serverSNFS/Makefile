CC = gcc

CFLAG = -Wall -lpthread

target : serverSNFS 

serverSNFS: serverSNFS.c
	$(CC) $(CFLAG) -o $@ $<


clean:
	rm serverSNFS
