#include "wtc.h"


//Process for child
int childProcess(int processNumber, sem_t* p2c, sem_t* c2p, int numVerts, int numProcs, int* M_prev, int* M_curr, int* queue, int* iterator, sem_t* queueSem)
{
	//Outer loop which loops at every iteration
	while ((*queue<numVerts)||(*iterator<numVerts))
	{
		sem_wait(&p2c[processNumber]);

		//Inner loop that allows for children to grab from the queue
		while(*queue<numVerts)
		{

			//Wait for any other process to stop accessing this sensitive data
			sem_wait(queueSem);

			//Dequeue
			int row = *queue;
			*queue+=1;

			//Allow other processes to pass the wait stage
			sem_post(queueSem);

			//Check if number is useable
			if(row < numVerts)
			{

				//Funtion that computes row result
				int j;
				for (j = 0; j < numVerts; j++)
					if ( (M_prev[(*iterator)*(numVerts)+j]==1&&M_prev[(row)*(numVerts)+(*iterator)]) || M_prev[(row)*(numVerts)+j]==1 )
						M_curr[row*numVerts+j] = 1;
			}
		}
		sem_post(&c2p[processNumber]);
	}
	exit(0);
}

int wtc_btproc(int numProcs, int numVerts, int** matrix) 
{
	struct timeval startt, endt;
	gettimeofday(&startt, NULL );

	//Create shared memory
	sem_t* p2c = NULL;
	sem_t* c2p = NULL;
	int* M_prev = NULL; //matrixes
	int* M_curr = NULL;
	int* queue = NULL;
	int* iterator = NULL;
	sem_t* queueSem = NULL;

	//Calculate sizes of both
	int size1 = sizeof(sem_t)*numProcs;
	int size2 = sizeof(int)*numVerts*numVerts;
	int size3 = sizeof(int);
	int size4 = sizeof(sem_t);

	//IDs for shared memory
	char* p2c_name = "/shared_sems"; //semaphore for process to alert children
	char* c2p_name = "/shared_sems2";
	char* mc_name = "/shared_curr";
	char* mp_name = "/shared_prev";
	char* q_name = "/shared_queue";
	char* i_name = "/shared_iterator";
	char* qs_name = "/shared_queueSem";

	//Create shared memory file and assign pointers
	int fd1=shm_open(p2c_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd2=shm_open(c2p_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd3=shm_open(mc_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd4=shm_open(mp_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd5=shm_open(q_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd6=shm_open(i_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	int fd7=shm_open(qs_name, O_RDWR | O_CREAT | O_TRUNC, 0666);

	//make the memory file the right size
	ftruncate(fd1,size1*2); 
	ftruncate(fd2,size1*2);
	ftruncate(fd3,size2*2);
	ftruncate(fd4,size2*2);
	ftruncate(fd5,size3*2);
	ftruncate(fd6,size3*2);
	ftruncate(fd7,size4*2);

	//Map files to memory
	p2c = (sem_t*)mmap(0,size1*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
	c2p = (sem_t*)mmap(0,size1*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
	M_curr = (int*)mmap(0,size2*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);
	M_prev = (int*)mmap(0,size2*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd4, 0);
	queue = (int*)mmap(0,size3*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd5, 0);
	iterator = (int*)mmap(0,size3*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd6, 0);
	queueSem = (sem_t*)mmap(0,size4*2,PROT_READ | PROT_WRITE, MAP_SHARED, fd7, 0);

	//Initialize all semaphores
	int k;
	for(k=0; k<numProcs; k++)
	{
		if(sem_init(&p2c[k], 1, 0)==-1||sem_init(&c2p[k], 1, 0)==-1)
		{
			fprintf(stderr,"sem_init failed\n");
			exit(1);
		}
	}

	if(sem_init(queueSem, 1, 1)==-1)
	{
		fprintf(stderr,"sem_init failed\n");
		exit(1);
	}

	//Initialize queue and iterator
	*queue = 0;
	*iterator = 0;

	//Copy original matrix to shared memory
	int i, j;
	for(i=0; i<numVerts; i++)
		for(j=0; j<numVerts; j++)
			M_prev[i*numVerts+j]=matrix[i][j];

	//Create the desired number of processes
	int pid;
	for(k=0; k<numProcs; k++){
		pid=fork();
		if(!pid)
			childProcess(k,p2c,c2p,numVerts,numProcs,M_prev,M_curr,queue, iterator, queueSem);
	}

	int y,z;
	//Counter for each iteration
	for(; *iterator<numVerts; *iterator+=1) 
	{	
		//reset queue
		*queue = 0;

		//Make the processes stop waiting
		for(k=0; k<numProcs; k++)
			sem_post(&p2c[k]);

		//Waits for all children to finish the processes
		for(k=0; k<numProcs; k++){
			sem_wait(&c2p[k]);	
		}

		//Moves the current matrix into the previous one
		for(y=0; y<numVerts; y++)
			for(z=0; z<numVerts; z++)
				M_prev[y*numVerts+z]=M_curr[y*numVerts+z];
	}
	
	//Destroy all semaphores
	for(k=0; k<numProcs; k++){
		if(sem_destroy(&p2c[k])==-1||sem_destroy(&c2p[k])==-1){
			fprintf(stderr,"sem_destroy failed\n");
			exit(1);
		}
	}

	//Print result
	//printf("Output:\n");
	//printArrayMatrix(M_curr,numVerts);

	//Remove the shared memory
	close(fd1);
	shm_unlink(p2c_name);
	close(fd2);
	shm_unlink(mc_name);
	close(fd3);
	shm_unlink(mp_name);
	close(fd4);
	shm_unlink(c2p_name);
	close(fd5);
	shm_unlink(q_name);
	close(fd6);
	shm_unlink(i_name);
	close(fd7);
	shm_unlink(qs_name);

	gettimeofday(&endt, NULL);
	int elapsedTime;
	elapsedTime = (endt.tv_usec - startt.tv_usec);
	//printf("%u,", elapsedTime);
	//printf("microseconds \n");
	return 0;
}
