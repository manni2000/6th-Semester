#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<fcntl.h>
#include<unistd.h>

#define MAXLINE 100000 
#define SERV_PORT 3000 

int
main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;

 char fileBuffer[MAXLINE],filePath[1000],fileName[1000],buf[MAXLINE];

 if (argc !=2) {
  perror("Usage: TCPClient <IP address of the server");
  exit(1);
 }


 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }


 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(SERV_PORT); 

 
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
 else{
    printf("\nConnection Successful\n");
 }
 

 printf("\nEnter File name : ");   
 
 fgets(fileName,1000,stdin);
 
 printf("\nEnter File Path : ");

 scanf("%[^\n]%*c", filePath);
 
 int fd=open(filePath,O_RDONLY|O_CREAT);

 if(fd==-1){
    printf("\nError in opening file\n");
    exit(4);
 }

 read(fd,fileBuffer,MAXLINE);

 snprintf(buf,MAXLINE,"%s%s",fileName,fileBuffer);

if(send(sockfd, buf, strlen(buf), 0)==-1){
    printf("\nUnable to send file to the server\n");
    exit(5);
 }
 
 else{
    printf("\nFile sent successfully\n");
    close(fd);
 }

 exit(0);
}