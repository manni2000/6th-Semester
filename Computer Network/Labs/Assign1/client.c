#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define IP_ADDRESS "127.0.0.1"
#define BUFSIZE 1024

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFSIZE];
    char *hello = "Hello from client";

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen("sample.txt", "r");
    if (fp == NULL)
    {
        printf("File not found\n");
        exit(0);
    }

    while (fgets(buffer, BUFSIZE, fp) != NULL)
    {
        send(sockfd, buffer, strlen(buffer), 0);
    }

    printf("File data sent to server\n");
    fclose(fp);
    close(sockfd);

    return 0;
}