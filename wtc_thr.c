#include "wtc.h" 

struct pt_mat {
	unsigned int tid; 
	pthread_mutex_t * p2c; // parent to child array
	pthread_mutex_t * c2p; // child to parent array
	int numVerts;
	int numThr;
	int** M_prev;
	int** M_cur;
};

void *Thr(void *thrargs) {
	struct pt_mat *mat; // mat contains all passed arguments 
	mat = (struct pt_mat *)thrargs;
	int i,j,k;
	for(k=0; k<(mat->numVerts); k++) {
		pthread_mutex_lock( &(mat->p2c[tid]) ); // locks child to parent
		for (i = (mat->tid); j < (mat->numVerts); i+=(mat->numThr)) {
			for(j = 0; j < (mat->numVerts); j++) {
				if ((mat->M_prev[i][j])==1) {
					(mat->M_curr[i][j]) = 1;
				}
			} 
		}
		pthread_mutex_unlock( &(mat->c2p[tid])) // operations complete, unlocks and moves 
	}
	
	pthread_exit (NULL) ;
}

int wtc_thr(int numThr, int numVerts, int** matrix) {


  	pthread_t thra[numThr]; // initialzies number of threads based on input argument.
  	struct pt_mat matarg[numThr];

  int k; // Initializing all the mutexes as follows:
  for(k=0; k<numThr; k++) {
  	if(pthread_mutex_init(&p2c[k], NULL)) {
  		fprintf(stderr, "Dang, I messed up.");
  		exit(1);
  	}
  }
  int i, j, rc;
  long tid;
  // Create desired number of threads 
  for (k=0; k<numThr; k++) {
  		matarg[k].tid = k;
  		matarg[k].M_prev[i][j] = matrix[i][j];
  		rc = pthread_create(&thr[k], NULL, Thr, (void *)&matarg[k]) ;
  		  if( rc ) {
  		  	printf("ERROR; return code is %d \n", rc)l
  		    exit( -1 );
  		}
  }

  int m,y,z;
  for (m=0; m<numVerts;m++) {
  	for (k=0; k<numThr; k++) {
  		pthread_mutex_unlock(&p2c[k])
  	}
  	for (k=0; k<numThr; k++) {
  		pthread_mutex_lock(&c2p)
  	}
  	for(y=0; y<numVerts; y++) {
  		for(z=0;z<numVerts;z++) {
  			M_prev[y][z]=M_curr[y][z];
  		}
  	}
  }

  for (k=0; k<numThr; k++) { // cleaning up after the threads
  		pthread_join(thr[k], NULL );
  		pthread_mutex_destroy( &p2c[k] );
  		pthread_mutex_destroy( &c2p[k] );
}
  printf("Output:\n");
  printArrayMatrix(M_curr,numVerts);

  pthread_exit(NULL);
}

