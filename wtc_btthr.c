#include "wtc.h"

typedef struct bt_mat {
  int tid;
  pthread_mutex_t * p2c; //parent to child
  pthread_mutex_t * c2p; //child to parent
  int numVerts;
  int numThr;
  int** M_prev;
  int** M_curr;
} bt_mat;

//the head of the queue
int head = 0;
int iterator = 0;
//the lock for the queue (prevents race conditions, etc.)
pthread_mutex_t queueLock;

void *thread(void *thrargs) {
  struct bt_mat *mat;
  mat = (struct bt_mat *)thrargs;
  //loop until the queue is empty or k > n
  while (iterator < mat->numVerts) {
    int j,i;
    if(iterator >= mat->numVerts)
      break;
    pthread_mutex_lock(&(mat->p2c[mat->tid]));
    while (head < mat->numVerts) {
      //get a node from the queue
      pthread_mutex_lock(&queueLock);
      i = head;
      head++;
      pthread_mutex_unlock(&queueLock);
      if (i >= mat->numVerts)
        break;
      for (j = 0; j < mat->numVerts; j++) {
        if ((mat->M_prev[i][j]==1) || ((mat->M_prev[iterator][j] == 1) && (mat->M_prev[i][iterator]  == 1))) {
          mat->M_curr[i][j] = 1;
        }
      }
    }
    pthread_mutex_unlock(&(mat->c2p[mat->tid]));
  }
  pthread_exit(0);
}

int wtc_btthr(int numThr, int numVerts, int** matrix) {
  struct timeval startt, endt;
  gettimeofday(&startt, NULL);

  //create the threads based on input
  pthread_t* thra = (pthread_t*)malloc(sizeof(pthread_t)*numThr);
  int i,j;
  //allocate data for the above structure
  bt_mat** matsrc = (bt_mat**)malloc(sizeof(bt_mat*)*numThr);
  for (i = 0; i < numThr; i++) {
    matsrc[i] = (bt_mat*)malloc(sizeof(bt_mat));
  }
  //allocate data for the matrix
  int** cmatrix = (int**)malloc(numVerts*sizeof(int*));
  for (i = 0; i < numVerts; i++) {
    cmatrix[i] = (int*)malloc(numVerts*sizeof(int*));
    for (j = 0; j < numVerts; j++) {
      cmatrix[i][j] = 0;
    }
  }
  //allocate data for the parent to child array
  pthread_mutex_t * p2c = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*numThr);
  //allocate data for the child to parent array
  pthread_mutex_t * c2p = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*numThr);
  //initialize the queue mutex
  pthread_mutex_init(&queueLock, NULL);
  int k;
  //initialize all the mutexes
  for (k = 0; k < numThr; k++) {
    pthread_mutex_init(&p2c[k], NULL);
    pthread_mutex_init(&c2p[k], NULL);
    pthread_mutex_lock(&p2c[k]);
    pthread_mutex_lock(&c2p[k]);
  }
  int rc;
  //create all the mutexes
  for (k = 0; k < numThr; k++) {
    matsrc[k]->tid = k;
    matsrc[k]->M_prev = matrix;
    matsrc[k]->M_curr = cmatrix;
    matsrc[k]->numVerts = numVerts;
    matsrc[k]->numThr = numThr;
    matsrc[k]->p2c = p2c;
    matsrc[k]->c2p = c2p;
    rc = pthread_create(&thra[k], NULL, thread, (void *)matsrc[k]);
    if (rc)
      exit (-1);
  }
  int y;
  for (iterator = 0; iterator < numVerts; iterator++) {
    pthread_mutex_lock(&queueLock);
    head = 0;
    pthread_mutex_unlock(&queueLock);
    //unlock all parent to child to tell them to start
    for (y = 0; y < numThr; y++)
      pthread_mutex_unlock(&p2c[y]);
    //wait for all the children to finish
    for (y = 0; y < numThr; y++){
      pthread_mutex_lock(&c2p[y]);

    }
    //move the current matrix to the prev one
    for (k = 0; k < numVerts; k++) {
      for (i = 0; i < numVerts; i++) {
        matrix[k][i] = cmatrix[k][i];
      }
    }
  }

  for (y = 0; y < numThr; y++)
      pthread_mutex_unlock(&p2c[y]);

  //Join the threads and destroy mutexes
  for (k = 0; k < numThr; k++){
    pthread_mutex_destroy(&p2c[k]);
    pthread_mutex_destroy(&c2p[k]);
    pthread_join(thra[k], NULL);
  }

  free(c2p);
  free(p2c);
  free(thra);
  for(i = 0; i < numThr; i++) {
    free(matsrc[i]);
  }
  free(matsrc);
  for(i = 0; i < numVerts; i++) {
    free(cmatrix[i]);
  }
  free(cmatrix);

  //printf("Output:\n");
  //printMatrix(matrix, numVerts);
  gettimeofday(&endt, NULL);
  int elapsedTime;
  elapsedTime = (endt.tv_usec - startt.tv_usec);
  printf("%u,", elapsedTime);
  return 0;
}
