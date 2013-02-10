#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <string.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h> // Needed for unix file manipulations and commands.
#include <sys/shm.h> // Needed for shared memory manipulation
#include <sys/sem.h> // needed for semaphores.

int printMatrix(int** M, int n);
