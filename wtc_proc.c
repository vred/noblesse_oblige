#include "wtc.h" // include everything

/*int child(int n) {
  int i, j;
  for (j = 1; j < n; j++) {

  }
}*/

//Manages the children
int parent(int numProcs, int numVerts, int** matrix) {
  //Create shared memory
  sem_t* sems = NULL; //will contain array of semaphors
  int* M_prev = NULL; //pointers cannot be stored in shared memory,
  int* M_curr = NULL; //thus we must use a single-dimensional array to represent our matrix
  //Necessary size for shared memory, enough for array of semaphores
  //and two NxN matrices
  int size1 = sizeof(sem_t)*numProcs; //size for semaphore array
  int size2 = sizeof(int)*numVerts*numVerts; //size for matrix
  
  //IDs for shared memory
  int fd1;
  char* sem_name= "/shared_sems";
  int fd2;
  char* mp_name = "/shared_mp";
  int fd3;
  char* mc_name = "/shared_mc";
  //Create shared memory file
  fd1=shm_open(sem_name, O_RDWR | O_CREAT, 0666);
  fd2=shm_open(mp_name, O_RDWR | O_CREAT, 0666);
  fd3=shm_open(mc_name, O_RDWR | O_CREAT, 0666);
  if(fd1==-1||fd2==-1||fd3==-1){
		fprintf(stderr, "shm_open failure\n");
		exit(1);
	}
	//Map shared memory
	sems = (sem_t*)mmap(0,size1,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	M_prev = (int*)mmap(0,size2,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	M_curr = (int*)mmap(0,size2,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	int k;
  //Initialize all the semaphores
  for(k=0; k<numProcs; k++){
	  if(sem_init(&sems[k], 1, 1)==-1){
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
  
	int pid = 0;
  if(!pid){ //i.e. I'm in the parent
	  	  
  }
  else{ //i.e. I'm in the child
	  
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
  parent(4,4,mat);
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
