#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include "../SNFS.h"


//contains the port where the server is listening
int port;
char* root_path;

char* request_handler(char* request){
	char* request_copy = strdup(request);
	char* tokenreq = strtok(request," ,");
	char* firstarg = tokenreq;
	//switch on first argument of request which is system call name
	if( !strcmp(firstarg,"readdir") )
	{
		tokenreq = strtok(NULL, " ,");
		return remote_readdir(tokenreq);
	} else if( !strcmp(firstarg,"getattr")){
		tokenreq = strtok(NULL, " ,");
		return remote_getattr(tokenreq);
	} else if( !strcmp(firstarg,"opendir")){
		tokenreq = strtok(NULL, " ,");
		return remote_opendir(tokenreq);
	} else if( !strcmp(firstarg,"releasedir")){
		tokenreq = strtok(NULL, " ,");
		return remote_releasedir(tokenreq);
	} else if( !strcmp(firstarg,"mkdir")){
		tokenreq = strtok(NULL, " ,");
		mode_t mode = (mode_t)atoi(strtok(NULL, " ,"));
		return remote_mkdir(tokenreq,mode);
	} else if( !strcmp(firstarg,"read")){
		tokenreq = strtok(NULL, " ,");
		size_t size = (size_t)atoi(strtok(NULL, " ,"));
		off_t offset = (off_t)atoi(strtok(NULL, " ,"));
		return remote_read(tokenreq,size,offset);
	} else if( !strcmp(firstarg,"open")){
		tokenreq = strtok(NULL, " ,");
		int flags = atoi(strtok(NULL, " ,"));
		return remote_open(tokenreq, flags);
	} else if( !strcmp(firstarg,"release")){
		tokenreq = strtok(NULL, " ,");
		return remote_release(tokenreq);
	} else if( !strcmp(firstarg,"write")){
		tokenreq = strtok(NULL, " ,");
		size_t size = (size_t)atoi(strtok(NULL, " ,"));
		off_t offset = (off_t)atoi(strtok(NULL, " ,"));
		//cut off the string so it just has the info to be written
		return remote_write(tokenreq,size,offset,request_copy);
	} else if( !strcmp(firstarg,"truncate")){
		tokenreq = strtok(NULL, " ,");
		off_t newsize = (off_t)atoi(strtok(NULL, " ,"));
		return remote_truncate(tokenreq,newsize);
	}
	
}

char* remote_truncate(const char *path, off_t newsize){
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	
	int retstat = truncate(full_path, newsize);
	char* resp=(char*)malloc(sizeof(int)+1);;
	sprintf(resp,"%d",retstat);
	return resp;	
}

char* remote_write(const char *pointer, size_t size, off_t offset, char* buffer){
	char* buffer_freer = buffer;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	int x = pwrite(atoi(pointer),buffer,size,offset);
	char* written=(char*)malloc(sizeof(int)+1);;
	sprintf(written,"%d",x);
	free(buffer_freer);
	return written;
}

char* remote_release(const char *pointer){
	close(atoi(pointer));
	return "";
}

char* remote_read(const char *pointer, size_t size, off_t offset){
	char* buffer = (char*)calloc(size,sizeof(char));
	pread(atoi(pointer),buffer,size,offset);
	return buffer;
}

char* remote_mkdir(const char *path, mode_t mode){
	
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	printf("mkdir here, full_path=%s and mode=%08x\n",full_path,mode);
	int retstat = mkdir(full_path, mode);
    if (retstat < 0){
		//error
	}
	return "";
}

char* remote_releasedir(const char *pointer){
	closedir((DIR *) (uintptr_t) atoi(pointer));
	return "";
}

char* remote_opendir(const char *path){
	DIR* dp;
	
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	
	char* direct = (char*)calloc(sizeof(DIR*),sizeof(char));
	dp = opendir(full_path);
	if(dp==NULL){
		//error
	}
	sprintf(direct,"%d",dp);
	return direct;
}

char* remote_getattr(const char *path){
	struct stat* stat_buf = (struct stat*)malloc(sizeof(struct stat));
	
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	
	lstat(full_path,stat_buf);
		
	char* status = (char*)calloc(sizeof(struct stat),sizeof(char));
	sprintf(status,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d",stat_buf->st_mode,stat_buf->st_ino,stat_buf->st_dev,stat_buf->st_uid,
						stat_buf->st_gid,stat_buf->st_atime,stat_buf->st_ctime,
							stat_buf->st_mtime,stat_buf->st_nlink,stat_buf->st_size);
	return status;
}

char* remote_readdir(const char *path){
	DIR *dir;
	char* entry_list = (char*)calloc(1,sizeof(char));
	struct dirent *ent = (struct dirent*)malloc(sizeof(struct dirent));
	
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	
	if ((dir = opendir (full_path)) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    entry_list = (char*)realloc(entry_list,strlen(entry_list)+strlen(ent->d_name)+2);
	    entry_list = strcat(entry_list,", ");
	    entry_list = strcat(entry_list,ent->d_name);
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("");
	  return NULL;
	}
	return entry_list;			 
}

char* remote_open(const char *path, int flags){
	int fd;
	
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	
	char* file_pointer = (char*)calloc(sizeof(int),sizeof(char));
	fd = open(full_path, flags);

	sprintf(file_pointer,"%d",fd);
	return file_pointer;
}


int main(int argc, char *argv[]){
	if(argc != 5){
		fprintf(stderr,"Invalid number of arguments!\n");
		return -1;
	}
	
	//Check the flags
	if(!strcmp(argv[1],"-port")){
		port = atoi(argv[2]);
	}
	else if(!strcmp(argv[1],"-mount")){
		root_path = strdup(argv[2]);
	}
	else{
		perror("Incorrect arguments!");	
	}
	if(!strcmp(argv[3],"-port")){
		port = atoi(argv[4]);
	}
	else if(!strcmp(argv[3],"-mount")){
		root_path = strdup(argv[4]);
	}
	else{
		perror("Incorrect arguments!");	
	}
	
	int sock, connected, bytes_recieved , true = 1;  
	char rec_data[4096];    

	struct sockaddr_in server_addr,client_addr;    
	int sin_size;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}
	
	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(port);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
																   == -1) {
		perror("Unable to bind");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(1);
	}
	
	printf("\nTCPServer Waiting for client on port %d\n",port);
	fflush(stdout);


	while(1)
	{  
		sin_size = sizeof(struct sockaddr_in);

		connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

		printf("I got a connection from (%s , %d)\n",
			   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

		//In this exmaple, server is only single-threaded
		{
			
		  bytes_recieved = recv(connected,rec_data,1024,0);
			
		 rec_data[bytes_recieved]='\0';
						  
		  printf("SUP? REQUEST = %s\n",rec_data);
		  
		   fflush(stdout);
		  
		  char* response = request_handler(rec_data);
		  
		  printf("Got my response, it's %d big and its %s\n",strlen(response),response);
		  
		  send(connected,response,4096,0);
		 // printf("\nClient wants to read file = %s " , recv_data);

		 

		  //Assume only file01 and file02 are existing now
		  
		   
		  close(connected);

		}
	}       

  close(sock);
  return 0;
} 
