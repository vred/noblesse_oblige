#include "wtc.h" // include everything

//Processes its assigned rows, based on the passed in argument
//n, which represents the id of the process--i.e. which number child it is
//and which semaphore applies to it
int child(int n, sem_t* p2c, sem_t* c2p, int numVerts, int numProcs, int* M_prev, int* M_curr) {
  int i,j,k;
  for(k=0; k<numVerts; k++){
		sem_wait(&p2c[n]);
		//These are the rows that this process is responsible for
		//(where i%numProcs = n)
		if(k==0&&n==1){
		}
		for(i = n; i<numVerts; i+=numProcs){
			for (j = 0; j < numVerts; j++) {
				if ((M_prev[k*numVerts+j]==1&&M_prev[i*numVerts+k])||M_prev[i*numVerts+j]==1) {
					M_curr[i*numVerts+j] = 1;
				}
			}
		}
		if(k==0&&n==1){
			printArrayMatrix(M_prev,numVerts);
			printArrayMatrix(M_curr,numVerts);
		}
		sem_post(&c2p[n]);
		
	}
  exit(0);
}

//Manages the children
int wtc_proc(int numProcs, int numVerts, int** matrix) {
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
  int fd1,fd2,fd3,fd4;
  char* p2c_name= "/shared_sems"; //semaphore for process to alert children
  char* c2p_name= "/shared_sems2";
  char* mc_name= "/shared_curr";
  char* mp_name= "/shared_prev";
  //Create shared memory file and assign pointers
  fd1=shm_open(p2c_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  fd2=shm_open(c2p_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  fd3=shm_open(mc_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  fd4=shm_open(mp_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
  ftruncate(fd1,size1*2); //make the memory file the right size
  ftruncate(fd2,size1*2);
  ftruncate(fd3,size2*2);
  ftruncate(fd4,size2*2);
  p2c = (sem_t*)mmap(0,size1*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  c2p = (sem_t*)mmap(0,size1*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
  M_curr = (int*)mmap(0,size2*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);
  M_prev = (int*)mmap(0,size2*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);
	
	int k;
  //Initialize all the semaphores
  for(k=0; k<numProcs; k++){
	  if(sem_init(&p2c[k], 1, 0)==-1||sem_init(&c2p[k], 1, 0)==-1){
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

	//Create the desired number of processes
	for(k=0; k<numProcs; k++){
		pid=fork();
		if(!pid){ //i.e. I'm in the child
	  	child(k,p2c,c2p,numVerts,numProcs,M_prev,M_curr);
		}
		//Else I'm still in the parent
	}
	
	int m,y,z;
	//This m is actually refers to the k iterations
	for(m=0; m<numVerts; m++){
		//We can move onto the next iteration, so release the semaphores
		for(k=0; k<numProcs; k++){
			sem_post(&p2c[k]);
		}
		//Wait for all the child to parent semaphores to unlock to signal
		//that all the children have done their rows before moving to the next
		//iteration
		for(k=0; k<numProcs; k++){
			sem_wait(&c2p[k]);
		}
		for(y=0; y<numVerts; y++){
			for(z=0; z<numVerts; z++){
				M_prev[y*numVerts+z]=M_curr[y*numVerts+z];
			}
		}
		//printf("%dth iteration:\n",m);
		//printArrayMatrix(M_curr,numVerts);
	}
	
	//Destroy all semaphores
	for(k=0; k<numProcs; k++){
	  if(sem_destroy(&p2c[k])==-1||sem_destroy(&c2p[k])==-1){
			fprintf(stderr,"sem_destroy failed\n");
			exit(1);
	  }
  }
  
  printf("Output:\n");
  printArrayMatrix(M_curr,numVerts);
  
  //Remove the shared memory
  close(fd1);
  shm_unlink(p2c_name);
  close(fd2);
  shm_unlink(mc_name);
  close(fd3);
  shm_unlink(mp_name);
  close(fd4);
  shm_unlink(c2p_name);
  return 0;
}

/*
int wtc_proc(int** matrix, int nvertices, int nprocs) {
  int PID = fork();
  if (PID == 0)
    child();
  else
    parent();
}*/
