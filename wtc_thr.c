#include "wtc.h"

typedef struct pt_mat {
  int tid;
  pthread_mutex_t * p2c; // parent to child array
  pthread_mutex_t * c2p; // child to parent array
  int numVerts;
  int numThr;
  int** M_prev;
  int** M_curr;
} pt_mat;

void *Thr(void *thrargs) {
  struct pt_mat *mat; // mat contains all passed arguments
  mat = (struct pt_mat *)thrargs;
  int t = (mat->tid);
  int i,j,k;
  for(k=0; k<(mat->numVerts); k++) {
    pthread_mutex_lock( &(mat->p2c[t]) ); // locks child to parent
    for (i = t; i < (mat->numVerts); i+=(mat->numThr)) {
      for(j = 0; j < (mat->numVerts); j++) {
        if ( ( (mat->M_prev[k][j])==1 && (mat->M_prev[i][k]) ) || (mat->M_prev[i][j])==1 )
          (mat->M_curr[i][j]) = 1;
      }
    }
    pthread_mutex_unlock(&(mat->c2p[t])); // done with operation, unlocks
  }
  pthread_exit (NULL) ;
}

int wtc_thr(int nThr, int nVerts, int** matrix) {
  struct timeval startt, endt; // setting timer 
  gettimeofday(&startt, NULL );

// allocating memory as follows: 
  pthread_t* thra = (pthread_t*)malloc(sizeof(pthread_t)*nThr); // initialzies number of threads based on input argument.
  int i,j;
  pt_mat** matsrc = (pt_mat**)malloc(sizeof(pt_mat*)*nThr);
  for(i=0; i<nThr; i++){
    matsrc[i] = (pt_mat*)malloc(sizeof(pt_mat));;
  }
  int** cmatrix = (int**)malloc(nVerts*sizeof(int*));
  for(i=0; i<nVerts; i++){
    cmatrix[i] = (int*)malloc(nVerts*sizeof(int));
    for(j = 0; j<nVerts; j++){
      cmatrix[i][j]=0;
    }
  }
  pthread_mutex_t * p2c = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)*nThr); // parent to child array
  pthread_mutex_t * c2p = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)*nThr);// child to parent array

  int k; // Initializing all the mutexes as follows:

  for(k=0; k<nThr; k++) {
    pthread_mutex_init(&p2c[k], NULL);
    pthread_mutex_init(&c2p[k], NULL);
    pthread_mutex_lock(&p2c[k]);
    pthread_mutex_lock(&c2p[k]);
  }
  int rc;

  // Create desired number of threads
  for (k=0; k<nThr; k++) {
    matsrc[k]->tid = k;
    matsrc[k]->M_prev = matrix;
    matsrc[k]->M_curr = cmatrix;
    matsrc[k]->numVerts = nVerts;
    matsrc[k]->numThr = nThr;
    matsrc[k]->p2c =  p2c;
    matsrc[k]->c2p = c2p;

    rc = pthread_create(&thra[k], NULL, Thr, (void *)matsrc[k]) ;
    if( rc ) {
      exit( -1 );
    }
  }

  int m,y,z; // m represents k-iterations
  for (m=0; m<nVerts;m++) {
    for(k=0;k<nThr;k++){pthread_mutex_unlock(&p2c[k]);}

    for(k=0;k<nThr;k++){
      pthread_mutex_lock(&c2p[k]);

    }
    // moving to the next iteration; releasing mutex

    for(y=0; y<nVerts; y++) {
      for(z=0; z<nVerts; z++) {
        matrix[y][z]=cmatrix[y][z];
      }
    }
  }

  // Joining pthreads and destroying the mutexes below:
  for (k=0; k<nThr; k++) {
    pthread_join( thra[k], NULL );
    pthread_mutex_destroy(&p2c[k]);
    pthread_mutex_destroy(&c2p[k]);

  }

  free(c2p);
  free(p2c);
  free(thra);
  for(i=0; i<nThr; i++){
    free(matsrc[i]);
  }
  free(matsrc);
  for(i=0; i<nVerts; i++){
    free(cmatrix[i]);
  }
  free(cmatrix);

  printf("Output:\n");
  printMatrix(matrix, nVerts);
  // printArrayMatrix(printmat,numVerts);
  gettimeofday(&endt, NULL);
  int elapsedTime; // Finding the elapsed time. 
  elapsedTime = (endt.tv_usec - startt.tv_usec);
  printf("time = %u ",
      elapsedTime);
  printf("microseconds \n");
  return 0;

}

