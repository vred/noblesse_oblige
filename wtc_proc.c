#include "wtc.h" // include everything

typedef struct sharedData{
  sem_t* sems;
  //sem_t* c2p;
  int** M_prev; //k-1 version of Matrix
  int** M_curr; //k version of Matrix;
  } sharedData;

int child(int n) {
  int i, j;
  for (j = 1; j < n; j++) {

  }
}

//Manages the children
int parent(int numProcs, int numVerts, int** matrix) {
  //Create shared memory
  void* shared_memory=(void*)0;
  sharedData* shared = NULL;
  
  //Necessary size for shared memory, enough for array of semaphores
  //and two NxN matrices
  int size = sizeof(sem_t)*numProcs+2*sizeof(int)*numVerts*numVerts;
  
  //ID for shared memory
  int fd;
  char* shm_name= "shared_memory";
  
  //Create shared memory file
  fd=shm_open(shm_name, O_RDWR | O_CREAT, 0666);
  if(fd == -1){
		fprintf(stderr, "shm_open failure\n");
		exit(1);
	}
	//Set shared memory size
	if(ftruncate(fd,size) == -1){
		fprintf(stderr, "ftruncate failure\n");
		exit(1);
	}
	//Map shared memory
	shared_memory = mmap(0,size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Cast shared memory to appropriate data structure
	shared = (sharedData*)shared_memory;

  int k;
  //Initialize all the semaphores
  for(k=0; k++; k<numProcs){
	  if(sem_init(&shared->sems[k], 1, 1)==-1){
			fprintf(stderr,"sem_init failed\n");
			exit(1);
	  }
  }
	int pid = 0;
  if(!pid){ //i.e. I'm in the parent
	  //semop(semid, &sem_lock, 1);
	  //semop(semid, &sem_unlock, 1);
	  
  }
  else{ //i.e. I'm in the child
	  //semop(semid, &sem_lock, 1);
	  //semop(semid, &sem_unlock, 1);
  }
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
  parent(5,5,mat);
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
