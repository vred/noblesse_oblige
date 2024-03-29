
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
	char* request = calloc(10+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"getattr, ");
	strcat(request, path);
	//request the attributes of the path
	sendRequestToServer(request, response, 4096);
	//fill out the stbuf with the response
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
	if(stbuf->st_mode<=0||stbuf->st_uid<=0||stbuf->st_gid<=0)
		retstat=-ENOENT;
	free(response);
	free(request);
	return retstat;
}



//Readdir implementation
static int SNFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi){
	int retstat = 0;
	char* request = calloc(10+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"readdir, ");
	strcat(request, path);
	//request all the things in the directory from the server
	sendRequestToServer(request, response, 4096);
	
	//call filler on each of those things
	char* token = strtok(response, " ,");
	while (token != NULL) {
		filler(buf,token,NULL,0);
		token = strtok(NULL, " ,");
	}
	free(response);
	free(request);
	return retstat;
}

static int SNFS_open(const char *path, struct fuse_file_info *fi){
	int retstat = 0;
    
    char* request = calloc(42+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"open, ");
	strcat(request, path);
	char flag_string[32];
	sprintf(flag_string,", %d",fi->flags);
	strcat(request,flag_string);
	//request to open a file
	sendRequestToServer(request, response, 4096);
    fi->fh = atoi(response);
	if(atoi(response)==0){
		retstat=-ENOENT;
	}
	free(response);
	free(request);
    return retstat;
}

static int SNFS_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi){
	
	int retstat = 0;
    char* request = calloc(74,sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"read, ");
	char info[64];
	sprintf(info,"%lu, %d, %d",fi->fh, (int)size, (int)offset);
	strcat(request, info);
	//request the info from the file
	sendRequestToServer(request, response, 4096);
	strcpy(buf,response);
	retstat = strlen(buf);
	free(response);
	free(request);
    return retstat;		  
}

static int SNFS_opendir(const char *path, struct fuse_file_info *fi){
	int retstat = 0;
	char* request = calloc(10+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"opendir, ");
	strcat(request, path);
	//request the directory pointer from the server
	sendRequestToServer(request, response, 4096);
	fi->fh = (intptr_t) strtoull(response,NULL,0);
	if(atoi(response)==0){
		retstat=-ENOENT;
	}
	free(response);
	free(request);
	return retstat;
}

static int SNFS_releasedir(const char *path, struct fuse_file_info *fi){
    int retstat = 0;
    char* request = calloc(47,sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"releasedir, ");
	char fh[32];
	sprintf(fh,"%lu",fi->fh);
	strcat(request, fh);
	//request the directory to close
	sendRequestToServer(request, response, 4096);
	free(response);
	free(request);
    return retstat;
}

static int SNFS_mkdir(const char *path, mode_t mode)
{
    int retstat = 0;
   
	char* request = calloc(42+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"mkdir, ");
	strcat(request, path);
	char mode_string[32];
	sprintf(mode_string,", %d",mode);
	strcat(request,mode_string);
	//request the server to make a directory
	sendRequestToServer(request, response, 4096);
    retstat = atoi(response);
    free(response);
	free(request);        
    return retstat;
}

static int SNFS_release(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    char* request = calloc(42,sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"release, ");
	char fh[32];
	sprintf(fh,"%lu",fi->fh);
	strcat(request, fh);
	//request the file to close
	sendRequestToServer(request, response, 4096);
    free(response);
	free(request);
    return retstat;
}

static int SNFS_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
    int retstat = 0;
    
    char* request = calloc(74+strlen(buf),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"write, ");
	char info[64];
	sprintf(info,"%lu, %d, %d, ",fi->fh, (int)size, (int)offset);
	strcat(request, info);
	strcat(request, buf);
	//send the info to the file
	sendRequestToServer(request, response, 4096);
	retstat = atoi(response);
	free(response);
	free(request);
    return retstat;
}

static int SNFS_truncate(const char *path, off_t newsize)
{
    int retstat = 0;  
	char* request = calloc(42+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"truncate, ");
	strcat(request,path);
	char off[32];
	sprintf(off,", %d",(int)newsize);
	strcat(request, off);
	//request the directory to close
	sendRequestToServer(request, response, 4096);
    
    retstat = atoi(response);
    free(response);
	free(request);
    return retstat;
}

static int SNFS_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    char* request = calloc(42+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"create, ");
	strcat(request, path);
	char mode_string[32];
	sprintf(mode_string,", %d",mode);
	strcat(request,mode_string);
	//request to make a new file
	sendRequestToServer(request, response, 4096);
    fi->fh = atoi(response);
	if(atoi(response)==0){
		retstat=-ENOENT;
	}
	free(response);
	free(request);
    return retstat;
        
}

static int SNFS_unlink(const char *path)
{
    int retstat = 0;
    
    char* request = calloc(10+strlen(path),sizeof(char));
	char* response = calloc(4096,sizeof(char));
	strcpy(request,"unlink, ");
	strcat(request, path);
	sendRequestToServer(request, response, 4096);
	free(response);
	free(request);
    return retstat;
	
}

static struct fuse_operations SNFS_oper = {
	.create		= SNFS_create,
	.open		= SNFS_open,
	.write		= SNFS_write,
	.release	= SNFS_release,
	.truncate	= SNFS_truncate,
	.read		= SNFS_read,
	.opendir	= SNFS_opendir,
	.readdir	= SNFS_readdir,
	.releasedir	= SNFS_releasedir,
	.mkdir		= SNFS_mkdir,
	.getattr	= SNFS_getattr,
	.unlink 	= SNFS_unlink,
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
		return -1;	
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
		return -1;	
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
		return -1;	
	}
	
	char** fuse_pass_argument = malloc(2*sizeof(char*));
	fuse_pass_argument[0] = strdup(argv[0]);
	fuse_pass_argument[1] = strdup(mount_path);
	
	free(mount_path);
		
	return fuse_main(2, fuse_pass_argument, &SNFS_oper, NULL);
}
