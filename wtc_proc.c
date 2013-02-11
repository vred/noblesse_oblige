#include "wtc.h" // include everything

typedef struct sharedData{
  sem_t* p2c;
  sem_t* c2p;
  int x;
  } sharedData;

int child(int n) {
  int i, j;
  for (j = 1; j < n; j++) {

  }
}

int parent(int numProcs) {
  printf("hey\n");
  int shmid;
  void* shared_memory=(void*)0;
  sharedData* shared;
  shmid = shmget((key_t)1111, sizeof(sharedData), 0666 | IPC_CREAT);
  if(shmid==-1){
    fprintf(stderr,"Shget failed! \n");
    exit(EXIT_FAILURE);
  }
  shared_memory=shmat(shmid, (void*)0, 0);
  if(shared_memory == (void*)-1) {
    fprintf(stderr, "Shmat failed!\n");
    exit(EXIT_FAILURE);
  }
  shared = (sharedData*)shared_memory;

// Two arrays of semaphores, 1 for parent to lock, 1 for child to lock
  shared->p2c = (sem_t*)malloc(numProcs*sizeof(sem_t));
  shared->c2p = (sem_t *)malloc(numProcs*sizeof(sem_t));
 shared->x = 0;
 pid_t pid = fork();
  if(!pid){
    shared->x++;
    printf("Yo, I'm the parent and x is %d.\n", shared->x);
  }
  else{
    shared->x=shared->x+2;
    printf("child, x is %d\n", shared->x);
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
