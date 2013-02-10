//#include "wtc.h"  include everything
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int child(n) {
 /* int i, j;
  for (j = 1; j < n; j++) {

  }*/
}

int parent() {
 // shm_open
}

int wtc_proc(int** matrix, int nvertices, int nprocs) {
  int PID = fork();
  if (PID == 0)
    child();
  else
    parent();
}
