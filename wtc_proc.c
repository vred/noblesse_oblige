#include "wtc.h" // include everything
/*
int child(n) {
  int i, j;
  for (j = 1; j < n; j++) {

  }
}

int parent() {
 // shm_open
}
*/

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
