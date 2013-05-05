
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include "../SNFS.h"

//contains the IP address where the server sits
char ip[17];
//contains the port where the server is listening
int port;
//Contains the mount path
char* mount_path;

//send request to server through socket and get feedback
char* sendRequestToServer(char* request_data, char* buf, int size){

 		int sock, bytes_recieved;  
        struct hostent *host;
        struct sockaddr_in server_addr;  

        host = gethostbyname(ip);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket error");
            return NULL;
        }

        server_addr.sin_family = AF_INET;     
        server_addr.sin_port = htons(port);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8); 

        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
        {
            perror("Connect error");
            return NULL;
        }
        
        //Send our struct WTF is its size?
		send(sock,request_data,strlen(request_data), 0);
				
		//Receive the content of file
		bytes_recieved=recv(sock,buf,size,0);
        
        buf[bytes_recieved] = '\0';
					
		close(sock);

		return buf;
}

//Getattr implementation
static int SNFS_getattr(const char *path, struct stat *stbuf){
	
	int retstat = 0;
	memset(stbuf, 0, sizeof(struct stat));
	char* request = calloc(1000,sizeof(char));
	char* response = calloc(11111,sizeof(char));
	strcpy(request,"getattr, ");
	strcat(request, path);
	//request the attributes of the path
	sendRequestToServer(request, response, 4096);
	//call filler on each of those things
	char* token = strtok(response, " ,");
	stbuf->st_mode = (mode_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_ino = (ino_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_dev = (dev_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_uid = (uid_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_gid = (gid_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_atime = (time_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_ctime = (time_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_mtime = (time_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_nlink = (nlink_t)atoi(token);
	token = strtok(NULL, " ,");
	stbuf->st_size = (off_t)atoi(token);
	token = strtok(NULL, " ,");	
	
	return retstat;
}



//Readdir implementation
static int SNFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi){
	int retstat = 0;
	char* request = calloc(1000,sizeof(char));
	char* response = calloc(11111,sizeof(char));
	strcpy(request,"readdir, ");
	strcat(request, path);
	//request all the things in the directory from the server
	sendRequestToServer(request, response, 1024);
	
	//call filler on each of those things
	char* token = strtok(response, " ,");
	while (token != NULL) {
		filler(buf,token,NULL,0);
		token = strtok(NULL, " ,");
	}
	
	return retstat;
}

/*static int SNFS_open(const char *path, struct fuse_file_info *fi){

}

static int SNFS_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi){
				  
}*/

static struct fuse_operations SNFS_oper = {
	//.create		= SNFS_create,
	//.open		= SNFS_open,
	//.write		= SNFS_write,
	//.close		= SNFS_close,
	//.truncate	= SNFS_truncate,
	//.opendir	= SNFS_opendir,
	.readdir	= SNFS_readdir,
	//.releasedir	= SNFS_releasedir,
	//.mkdir		= SNFS_mkdir,
	.getattr	= SNFS_getattr,
};

int main(int argc, char *argv[])
{
	if(argc != 7){
		fprintf(stderr,"Invalid number of arguments!\n");
		return -1;
	}
	
	//Check the flags
	if(!strcmp(argv[1],"-port")){
		port = atoi(argv[2]);
	}
	else if(!strcmp(argv[1],"-address")){
		strcpy(ip,argv[2]);
	}
	else if(!strcmp(argv[1],"-mount")){
		mount_path = strdup(argv[2]);
	}
	else{
		perror("Incorrect arguments!");	
	}
	if(!strcmp(argv[3],"-port")){
		port = atoi(argv[4]);
	}
	else if(!strcmp(argv[3],"-address")){
		strcpy(ip,argv[4]);
	}
	else if(!strcmp(argv[3],"-mount")){
		mount_path = strdup(argv[4]);
	}
	else{
		perror("Incorrect arguments!");	
	}
	if(!strcmp(argv[5],"-port")){
		port = atoi(argv[6]);
	}
	else if(!strcmp(argv[5],"-address")){
		strcpy(ip,argv[6]);
	}
	else if(!strcmp(argv[5],"-mount")){
		mount_path = strdup(argv[6]);
	}
	else{
		perror("Incorrect arguments!");	
	}
	
	char** fuse_pass_argument = malloc(2*sizeof(char*));
	fuse_pass_argument[0] = strdup(argv[0]);
	fuse_pass_argument[1] = strdup(mount_path);
		
	return fuse_main(2, fuse_pass_argument, &SNFS_oper, NULL);
}
