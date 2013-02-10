#include <sys/shm.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm_com.h"

int main() {
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	char buffer[BUFSIZ];
	pid_t pid;
	int shmid;
	char *message;
	int n;
	int exit_code;
	
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

	if (shmid == -1) {
		fprintf(stderr, "shget failed \n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat (shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed \n");
		exit(EXIT_FAILURE);
		}
	printf ("memory attached at %d \n", (int)shared_memory);
	shared_stuff = (struct shared_use_st *)shared_memory;
	while(running) {
		while(shared_stuff->written_by_you == 1) {
		pid = fork();
		switch(pid) {
			case 0:
				message = "This is the child";
				n=5;
				child(n);
				exit_code = 37;
				break;
			default:
				message = "This is the parent";
				n = 3;
				exit_code = 0;
				break;
			}
			for(; n > 0; n--) {
			sleep(1);
			}
		printf("waiting for client.. \n"); 
		}
		printf("Enter some text: ");	
		fgets(buffer, BUFSIZ, stdin);
	
		strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
		shared_stuff->written_by_you = 1;	

	if (strncmp(buffer, "end", 3) == 0) {
		running = 0;
	}
}
	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed \n");
		exit(EXIT_FAILURE);
	}
return 0;
}

int child(n) {
	int running = 1;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	int shmid;
	
	srand((unsigned int)getpid());
	
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
	if (shmid == -1) {
		fprintf(stderr, "shmget failed \n");
		exit(EXIT_FAILURE);
	}
	
	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed \n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at %X \n", (int)shared_memory);
	shared_stuff = (struct shared_use_st *)shared_memory;
	shared_stuff->written_by_you=0;
	while(running) {
		if(shared_stuff->written_by_you) {
			printf("You wrote: %s", shared_stuff->some_text);
			sleep( rand() % 4); // other process waits
			shared_stuff->written_by_you = 0;
			if (strncmp(shared_stuff->some_text, "end", 3) == 0)  {
				running = 0;
			}
		}
	exit(EXIT_SUCCESS);
	} // WARNING: INVOKES ZOMBIE APOCALYPSE!
}
