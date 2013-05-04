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

char* request_handler(char* request){


	char* tokenreq = strtok(request," ,");
	char* firstarg = tokenreq;

	if( !strcmp(firstarg,"readdir") )
	{
		tokenreq = strtok(NULL, " ,");
		return remote_readdir(tokenreq);
	} else if( !strcmp(firstarg,"getattr")){
		tokenreq = strtok(NULL, " ,");
		return remote_getattr(tokenreq);
	}
	//switch on first argument of request which is system call name
	
}

char* remote_getattr(const char *path){
	struct stat* stat_buf = (struct stat*)malloc(sizeof(struct stat));
	lstat(path,stat_buf);
	char* status = malloc(sizeof(stat_buf));
	memcpy(status, stat_buf, sizeof(struct stat));
	return status;
}

char* remote_readdir(const char *path){
	DIR *dir;
	char* entry_list = (char*)malloc(1);
	struct dirent *ent;
	if ((dir = opendir (path)) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    entry_list = (char*)realloc(entry_list,sizeof(remote_readdir)+strlen(ent->d_name)+2);
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


int main()
{
	port=5000;
        int sock, connected, bytes_recieved , true = 1;  
        char rec_data[1024];    

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
			printf("waiting\n");

            sin_size = sizeof(struct sockaddr_in);

            connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

            printf("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

			//In this exmaple, server is only single-threaded
            {
				
              bytes_recieved = recv(connected,rec_data,1024,0);
				
             rec_data[bytes_recieved]='\0';
				              
              printf("SUP? PATH = %s\n",rec_data);
              
               fflush(stdout);
              
              char* response = request_handler(rec_data);
              
              printf("Got my response, it's %d big and its %s\n",strlen(response),response);
              
              send(connected,response,strlen(response),0);
			
				printf("And I sent it\n");
			 // printf("\nClient wants to read file = %s " , recv_data);
   	
			 

			  //Assume only file01 and file02 are existing now
			  
               
              close(connected);

            }
        }       

      close(sock);
      return 0;
} 
