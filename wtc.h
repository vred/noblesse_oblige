#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h> // Needed for unix file manipulations and commands.
//#include <sys/shm.h> // Needed for shared memory manipulation
#include <sys/sem.h> // needed for semaphores.
#include <sys/stat.h> //For mode constants
#include <fcntl.h> //For O_ constants
#include <sys/types.h>

int printMatrix(int** M, int n);
int printArrayMatrix(int* M, int n);

//proc functions
int child(int n, sem_t* p2c, sem_t* c2p, int numVerts, int numProcs, int* M_prev, int* M_curr);
int wtc_proc(int numProcs, int numVerts, int** matrix);

//thr functions
void *Thr(void *thrargs);
int wtc_thr(int nThr, int nVerts, int** matrix);

//btproc functions
int childProcess(int processNumber, sem_t* p2c, sem_t* c2p, int numVerts, int numProcs, int* M_prev, int* M_curr, int* queue, int* iterator, sem_t* queueSem);
int wtc_btproc(int numProcs, int numVerts, int** matrix);

//btthr functions
void *thread(void *thrargs);
int wtc_btthr(int numThr, int numVerts, int** matrix);
