#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<unistd.h>
#include<fcntl.h>

#define MAXLINE 100000 
#define SERV_PORT 3000 
#define LISTENQ 8 

int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 socklen_t clilen;
 char fileBuf[MAXLINE],fileNameBuf[1000],buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;


 listenfd = socket (AF_INET, SOCK_STREAM, 0);

 
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 servaddr.sin_port = htons(SERV_PORT);

 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 listen (listenfd, LISTENQ);

 printf("%s\n","Server running...waiting for connections.");


  clilen = sizeof(cliaddr);
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
  printf("%s\n","Received request...");
  
  if(recv(connfd,buf,MAXLINE,0)==-1){
    printf("\nUnable to receive file\n");
    exit(1);
  }

  int change=0,j=0,k=0;
  for(int i=0;i<strlen(buf);i++){
    if(buf[i]=='\n'&&(!change)){
        change=1;
        continue;
    }
    if(!change){
        fileNameBuf[j]=buf[i];
        j++;
    }
    else if(change){
        fileBuf[k]=buf[i];
        k++;
    }
  }
  fileNameBuf[j]='\0';
  fileBuf[k]='\0';

  int fd=open(fileNameBuf,O_WRONLY|O_CREAT|O_TRUNC,0777);

   if(fd==-1){
    printf("\nError in creating file\n");
    exit(2);
 }

 write(fd,fileBuf,strlen(fileBuf));
 printf("\nFile received successfully\n");
 
 close(fd);
 close(connfd);
 close (listenfd);
}