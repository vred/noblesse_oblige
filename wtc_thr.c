#include "wtc.h" 

struct pt_mat {
	long tid; 
	pthread_mutex_t * p2c; // parent to child array
	pthread_mutex_t * c2p; // child to parent array
	int numVerts;
	int numThr;
	int** M_prev;
	int** M_curr;
};

void *Thr(void *thrargs) {
	struct pt_mat *mat; // mat contains all passed arguments 
	mat = (struct pt_mat *)thrargs;
  long t = (mat->tid);
	int i,j,k;
	for(k=0; k<(mat->numVerts); k++) {
		pthread_mutex_lock( &(mat->p2c[t]) ); // locks child to parent
		for (i = (mat->tid); j < (mat->numVerts); i+=(mat->numThr)) {
			for(j = 0; j < (mat->numVerts); j++) { 
				if ((mat->M_prev[i][j])==1) {
					(mat->M_curr[i][j]) = 1;
				}
			} 
		}
		pthread_mutex_unlock( &(mat->c2p[t])); // operations complete, unlocks and moves 
	}
	
	pthread_exit (NULL) ;
}

int wtc_thr(int numThr, int numVerts, int** matrix) {


  	pthread_t thra[numThr]; // initialzies number of threads based on input argument.
  	struct pt_mat matsrc[numThr];
    pthread_mutex_t * p2c = NULL; // parent to child array
    pthread_mutex_t * c2p = NULL;// child to parent array

  int k; // Initializing all the mutexes as follows:
  for(k=0; k<numThr; k++) {
  	if(pthread_mutex_init(&p2c[k], NULL)) {
  		fprintf(stderr, "Dang, I messed up.");
  		exit(1);
  	}
  }
  int i, j, rc;
  
  // Create desired number of threads 
  for (k=0; k<numThr; k++) {
  		matsrc[k].tid = k;
  		matsrc[k].M_prev[i][j] = matrix[i][j];
      matsrc[k].numVerts = numVerts;
      matsrc[k].numThr = numThr;
      
  		rc = pthread_create(&thra[k], NULL, Thr, (void *)&matsrc[k]) ;
  		  if( rc ) {
  		  	printf("ERROR; return code is %d \n", rc);
  		    exit( -1 );
  		}
  }

  int m,y,z;
  for (m=0; m<numVerts;m++) {
  	for (k=0; k<numThr; k++) {
  		pthread_mutex_unlock(&p2c[k]);
  	}
  	for (k=0; k<numThr; k++) {
  		pthread_mutex_lock(&c2p[k]);
  	}
  	for(y=0; y<numVerts; y++) {
  		for(z=0;z<numVerts;z++) {
  			(matsrc->M_prev[y][z])=(matsrc->M_curr[y][z]);
  		}
  	}
  }

  for (k=0; k<numThr; k++) {
  		pthread_join(thra[k], NULL );
  		pthread_mutex_destroy( &p2c[k] );
  		pthread_mutex_destroy( &c2p[k] );
}
  int* printmat; // Initializes matrix to print
  for(i=0; i<numVerts; i++){
    for(j=0; j<numVerts; j++){
      printmat[i*numVerts+j]=(matsrc->M_curr[i][j]);
    }
  }
  printf("Output:\n");
  printArrayMatrix(printmat,numVerts);

  pthread_exit(NULL);
}

  //Copy original matrix to shared memory, which has it represented
  //as a 1-dimensional array d
