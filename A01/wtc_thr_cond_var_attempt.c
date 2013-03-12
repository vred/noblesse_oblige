#include "wtc.h" 

struct pt_mat {
	int tid; 
	pthread_mutex_t * p2c; // parent to child array
	pthread_cond_t * c2p; // child to parent conditional
	int numVerts;
	int numThr;
	int** M_prev;
	int** M_curr;
};

void *Thr(void *thrargs) {
	struct pt_mat *mat; // mat contains all passed arguments 
	mat = (struct pt_mat *)thrargs;
  int t = (mat->tid);
	int i,j,k;
	for(k=0; k<(mat->numVerts); k++) {
		 // locks child to parent
    
		for (i = t; i < (mat->numVerts); i+=(mat->numThr)) {
			for(j = 0; j < (mat->numVerts); j++) { 
        if ( ( (mat->M_prev[k][j]) && (mat->M_prev[i][k]) ) || (mat->M_prev[i][j]) ) 
          (mat->M_curr[i][j]) = 1;
			} 
		}
    
   // pthread_mutex_unlock(&(mat->p2c[t])); // done with operation, unlocks
   // pthread_cond_wait(&(mat->c2p[t]),&(mat->p2c[t]));	
    // printf("Signaling %p \n", &(mat->c2p[t]));
  }
  pthread_cond_signal(&(mat->c2p[t]));
	pthread_exit (NULL) ;
}

int wtc_thr(int nThr, int nVerts, int** matrix) {
  struct timeval startt, endt;
  gettimeofday(&startt, NULL );

  	pthread_t thra[nThr]; // initialzies number of threads based on input argument.
  	struct pt_mat matsrc[nThr];
    pthread_mutex_t * p2c = malloc(sizeof(pthread_mutex_t)*nThr); // parent to child array
    pthread_cond_t * c2p = malloc(sizeof(pthread_cond_t)*nThr);// child to parent array

  int k; // Initializing all the mutexes as follows:
    
  for(k=0; k<nThr; k++) {
    pthread_mutex_init(p2c, NULL);
    pthread_cond_init(c2p, NULL);
    
  }
  int rc;
  
  // Create desired number of threads 
  for (k=0; k<nThr; k++) {
  		matsrc[k].tid = k;
  		matsrc[k].M_prev = matrix;
      matsrc[k].M_curr = matrix;
      matsrc[k].numVerts = nVerts;
      matsrc[k].numThr = nThr;
      matsrc[k].p2c =  p2c;
      matsrc[k].c2p = c2p;
      
  		rc = pthread_create(&thra[k], NULL, Thr, (void *)&matsrc[k]) ;
  		  if( rc ) {
  		    exit( -1 );
  		}
  }

  int m,y,z; // m represents k-iterations
  for (m=0; m<nVerts;m++) {
    for (k=0; k<nThr; k++)
      pthread_mutex_unlock(&p2c[k]);
    // pthread_cond_signal(&c2p[k]);
    // moving to the next iteration; releasing mutex
    for (k=0; k<nThr; k++)
      pthread_cond_wait(&c2p[k],&p2c[k]); 

  	for(y=0; y<nVerts; y++) {
  		for(z=0; z<nVerts; z++) {
  			 (matsrc->M_prev[y][z])=(matsrc->M_curr[y][z]);
         
  		}
  	}
  }
  
// Joining pthreads and destroying the mutexes below:
  for (k=0; k<nThr; k++) {
  		pthread_join( thra[k], NULL );
  		pthread_mutex_destroy( (matsrc[k].p2c));
  		pthread_cond_destroy( (matsrc[k].c2p));

}

  printf("Output:\n");
  printMatrix(matsrc->M_curr, nVerts);
  // printArrayMatrix(printmat,numVerts);
  gettimeofday(&endt, NULL);
  int elapsedTime;
  elapsedTime = (endt.tv_usec - startt.tv_usec);
  printf("time = %u ", 
      elapsedTime);
  printf("microseconds \n");
  pthread_exit(NULL);
  free(c2p);
  free(p2c);
}

  //Copy original matrix to shared memory, which has it represented
  //as a 1-dimensional array d
