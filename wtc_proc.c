#include "wtc.h" // include everything

//Processes its assigned rows, based on the passed in argument
//n, which represents the id of the process--i.e. which number child it is
//and which semaphore applies to it
int child(int n, sem_t* p2c, sem_t* c2p, int numVerts, int* M_prev, int* M_curr) {
  printf("Yo, I'm child %d\n", n);
  int i,j,k;
  for(k=0; k<numVerts; k++){
		printf("%d bout to lock sem\n", n);
		sem_wait(&c2p[n]);
		printf("%d done waiting\n",n);
		if (M_prev[i*numVerts+k] == 1) { //if mat[i][k] != 1, don't need to bother checking mat[k][j]
			for (j = 0; j < numVerts; j++) {
				if (M_prev[i*numVerts+j] == 1) {
					M_curr[i*numVerts+j] = 1;
				}
			}
		}
		sem_post(&c2p[n]);
		printf("%d done my work \n",n);
		sem_wait(&p2c[n]);
		sem_post(&p2c[n]);
		printf("%d moving on\n",n);
	}
	printf("%d exiting\n",n);
  exit(0);
}

//Manages the children
int parent(int numProcs, int numVerts, int** matrix) {
  //Create shared memory
  sem_t* p2c = NULL; //will contain array of semaphors
  sem_t* c2p = NULL;
  int* M_prev = NULL; //pointers cannot be stored in shared memory,
  int* M_curr = NULL; //thus we must use a single-dimensional array to represent our matrix
  //Necessary size for shared memory, enough for array of semaphores
  //and two NxN matrices
  int size1 = sizeof(sem_t)*numProcs; //size for semaphore array
  int size2 = sizeof(int)*numVerts*numVerts; //size for matrix
  
  //IDs for shared memory
  int fd1;
  char* p2c_name= "/shared_sems"; //semaphore for process to alert children
  //Create shared memory file and assign pointers
  fd1=shm_open(p2c_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  ftruncate(fd1,size1*2+size2*2); //make the memory file the right size
  p2c = (sem_t*)mmap(0,size1*2+size2*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  M_prev = (int*)(p2c+size1);
  M_curr = (int*)(M_prev+size2);
  c2p = (sem_t*)(M_curr+size2);	
	
	int k;
  //Initialize all the semaphores
  for(k=0; k<numProcs; k++){
	  if(sem_init(&p2c[k], 1, 1)==-1||sem_init(&c2p[k], 1, 1)==-1){
			fprintf(stderr,"sem_init failed\n");
			exit(1);
	  }
  }
  //Copy original matrix to shared memory, which has it represented
  //as a 1-dimensional array
  int i, j;
  for(i=0; i<numVerts; i++){
		for(j=0; j<numVerts; j++){
			M_prev[i*numVerts+j]=matrix[i][j];
		}
	}
	int pid;
		
	//Lock all semaphores before creating the children
	//to guarantee no child finishes before every child has been made
	for(k=0; k<numProcs; k++){
		sem_wait(&p2c[k]);
		//sem_wait(&c2p[k]);
	}

	//Create the desired number of processes
	for(k=0; k<numProcs; k++){
		pid=fork();
		if(!pid){ //i.e. I'm in the child
	  	child(k,p2c,c2p,numVerts,M_prev,M_curr);
		}
		//Else I'm still in the parent
	}
	
	int m;
	for(m=0; m<numVerts; m++){
		//Wait for all the child to parent semaphores to unlock to signal
		//that all the children have done their rows before moving to the next
		//iteration
		usleep(50000);
		for(k=0; k<numProcs; k++){
			printf("still waiting on %d\n",k);
			sem_wait(&c2p[k]);
		}
		printf("DONE WAITING!\n");
		
		//Once they've all been waited on, this means every child is done
		//and we can move onto the next iteration, so release the semaphores
		for(k=0; k<numProcs; k++){
			printf("releasing sem %d\n",k);
			sem_post(&c2p[k]);
			sem_post(&p2c[k]);
		}
	}
	
	//Destroy all semaphores
	for(k=0; k<numProcs; k++){
	  if(sem_destroy(&p2c[k])==-1||sem_destroy(&c2p[k])==-1){
			fprintf(stderr,"sem_destroy failed\n");
			exit(1);
	  }
  }
  
  //Remove the shared memory
  close(fd1);
  shm_unlink(p2c_name);
	
  exit(0);
  return 0;
}

//Warshall's algorithm, raw
int** algorithm(int n, int** mat) {
  int i,j,k;
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      if (mat[i][k] == 1) { //if mat[i][k] != 1, don't need to bother checking mat[k][j]
        for (j = 0; j < n; j++) {
          if (mat[k][j] == 1) {
            mat[i][j] = 1;
          }
        }
      }
    }
  }
  parent(6,4,mat);
  return mat;
}

/*
int wtc_proc(int** matrix, int nvertices, int nprocs) {
  int PID = fork();
  if (PID == 0)
    child();
  else
    parent();
}*/
