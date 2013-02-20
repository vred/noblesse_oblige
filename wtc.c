#include "wtc.h"
#include "wtc_proc.c"
#include "wtc_btproc.c"
#include "wtc_thr.c"
#include "wtc_btthr.c"
#include "wtc_btproc.c"
//Print out a matrix to stdout
int printMatrix(int** M, int n){

	int i,j;
	printf(" ");
	for(i = 0; i<n; i++){
		for(j = 0; j<n; j++){
			printf("%d ",M[i][j]);
		}
		printf("\n ");
	}
	printf("\n");

	return 0;
}

int printArrayMatrix(int* M, int n){
	int i,j;
	printf(" ");
	for(i = 0; i<n; i++){
		for(j = 0; j<n; j++){
			printf("%d ",M[i*n+j]);
		}
		printf("\n ");
	}
	printf("\n");

	return 0;
}

int main(int argc, char** argv){

	//Check for correct # inputs
	if(argc!=3){
		fprintf(stderr,"Invalid number of arguments!\n");
		return -1;
	}

	//Represents the algorithm which the user desires to calculated
	//the transitive closure matrix with. Must be 1, 2, 3, or 4.
	int choice = atoi(argv[1]);
	if(choice<1||choice>4){
		fprintf(stderr,"Invalid choice!\n");
		return -1;
	}

	FILE* fp;
	fp = fopen(argv[2],"r+");
	//Make sure that the file is valid
	if(!fp){
		fprintf(stderr,"Error opening file!\n");
		return -1;
	}

	//Store the number of processes/threads to create and the
	//number of vertices (read from the input file);
	int num_pt = 0;
	int num_ver = 0;
	fscanf(fp,"%d",&num_pt);
	fscanf(fp,"%d",&num_ver);

	int i,j;
	//Create the adjacency matrix
	int** A = (int**)malloc(num_ver*sizeof(int*));
	for(i=0; i<num_ver; i++){
		A[i] = (int*)malloc(num_ver*sizeof(int));
	}
	//Initialize the adjacency matrix;
	for(i=0; i<num_ver; i++){
		for(j=0; j<num_ver; j++){
			A[i][j]=0;
		}
	}
	//Fill out the adjacency matrix based on the input
	while(1){
		if(fscanf(fp,"%d",&i)==EOF){break;}
		if(fscanf(fp,"%d",&j)==EOF){break;}
		A[i-1][j-1]=1;
	}

	//printMatrix(A,num_ver);

	//i.e. Calculate with processes, using algorithm 2
	if(choice==1){
		wtc_proc(num_pt,num_ver,A);
	}
	//i.e. Calculate with threads, using algorithm 2
	else if(choice==2){
		wtc_thr(num_pt, num_ver, A);
	}
	//i.e. Calculate with processes, using bag of tasks algorithm
	else if(choice==3){
		wtc_btproc(num_pt, num_ver, A);
	}
	//i.e. Calculate with threads, using bag of tasks algorithm
	else{
		wtc_btthr(num_pt, num_ver, A);
	}
	for(i=0; i<num_ver; i++){
		free(A[i]);
	}
	free(A);
	fclose(fp);
	return 0;
}

