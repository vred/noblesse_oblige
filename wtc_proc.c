#include "wtc.h" // include everything

/*typedef struct sharedData{
  sem_t* p2c;
  sem_t* c2p;
  } sharedData;
*/

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

int child(int n) {
  int i, j;
  for (j = 1; j < n; j++) {

  }
}

int parent(int numProcs) {
  //Create shared memory
  int semid;
  void* shared_memory=(void*)0;
  //Shmid is the ID of the shared memory
  semid = semget((key_t)2222, 1, 0666 | IPC_CREAT);
  if(semid==-1){
    fprintf(stderr,"semget failed! \n");
    exit(EXIT_FAILURE);
  }
  
  /*//Gives current process access to shared memory space
  shared_memory=shmat(shmid, (void*)0, 0);
  if(shared_memory == (void*)-1) {
    fprintf(stderr, "semat failed!\n");
    exit(EXIT_FAILURE);
  }
  //shared is our data structure containing our semaphore arrays
  shared = (sharedData*)shared_memory;

// Two arrays of semaphores, 1 for parent to inform child, 1 for child to inform parent
  shared->p2c = (sem_t *)malloc(numProcs*sizeof(sem_t));
  shared->c2p = (sem_t *)malloc(numProcs*sizeof(sem_t));
  int k;
  //Initialize all the semaphores
  for(k=0; k++ k<numProcs){
	  if(sem_init(p2c[k], 1, ){
		  
	  }
  }*/
  //Calling semop with this struct waits (lock) on the semaphore
  struct sembuf sem_lock = { 0, -1, SEM_UNDO };
  //Calling semop with this struct post (unlock) the semaphore
  struct sembuf sem_unlock = { 0, 1, SEM_UNDO };
  union semun setvalue = {1};
  printf("semaphore value is %d\n", semctl(semid,0,GETVAL));
  semctl(semid, 0, SETVAL, setvalue);
  printf("and now semaphore value is %d\n", semctl(semid,0,GETVAL));
  pid_t pid = fork();
  if(!pid){ //i.e. I'm in the parent
	  printf("P: I'm about to lock\n");
	  semop(semid, &sem_lock, 1);
	  printf("Parent: I have locked the semaphore\n");
	  semop(semid, &sem_unlock, 1);
  }
  else{ //i.e. I'm in the child
	  printf("C: I'm about to lock\n");
	  semop(semid, &sem_lock, 1);
	  printf("Child: I have locked the semaphore\n");
	  semop(semid, &sem_unlock, 1);
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
  parent(5);
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
