
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

//contains the IP address where the server sits
char[16] ip;
//contains the port where the server is listening
int port;

//send request to server through socket and get feedback
static int sendRequestToServer(const char* path, char* buf, int size){

 		int sock, bytes_recieved;  
        struct hostent *host;
        struct sockaddr_in server_addr;  

        host = gethostbyname(ip);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket error");
            return -1;
        }

        server_addr.sin_family = AF_INET;     
        server_addr.sin_port = htons(port);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8); 

        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
        {
            perror("Connect error");
            return -1;
        }
        
        //Only read is implemented here, so just send the path to server
		send(sock,path,strlen(path), 0);
				
		//Receive the content of file
		bytes_recieved=recv(sock,buf,size,0);
        
        buf[bytes_recieved] = '\0';

		close(sock);

		return 0;
}

static struct fuse_operations SNFS_oper = {
	.create		= SNFS_create,
	.open		= SNFS_open,
	.write		= SNFS_write,
	.close		= SNFS_close,
	.truncate	= SNFS_truncate,
	.opendir	= SNFS_opendir,
	.readdir	= SNFS_readdir,
	.releasedir	= SNFS_releasedir,
	.mkdir		= SNFS_mkdir,
	.getattr	= SNFS_getattr,
};

int main(int argc, char *argv[])
{
	if(argc != 4){
		fprintf(stderr,"Invalid number of arguments!\n");
		return -1;
	}
	
	port = atoi(argv[1]);
	ip = argv[2];
	
	
	return fuse_main(argc, argv, &hello_oper, NULL);
}
