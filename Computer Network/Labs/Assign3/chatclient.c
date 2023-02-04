#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void *_chat(void *);
void _sendImage(FILE *, int);
void _recvImage(FILE *, int);

int main(void)
{
    system("clear");

    int cli_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    // set parameters
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4201);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (
        connect(
            cli_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("could not connect to server\n");
        exit(EXIT_FAILURE);
    }

    printf("-- connected to server\n\n");

    pthread_t thread;
    pthread_create(&thread, NULL, _chat, (void *)&cli_sock);

    // input requests and send to server
    while (1)
    {
        char buff[1024];
        scanf("%s", buff);

        if (strcmp(buff, "LIST") == 0)
        {
            send(cli_sock, buff, 1024, 0);
        }
        else if (strcmp(buff, "SEND") == 0)
        {
            send(cli_sock, buff, 1024, 0);

            scanf("%s", buff);
            send(cli_sock, buff, 1024, 0);

            scanf("%s", buff);
            send(cli_sock, buff, 1024, 0);

            int type = 1;
            if (strcmp(buff, "~IMG") == 0)
            {
                type = 0;
            }

            if (type == 1)
            {
                scanf("%[^\n]s", buff);
                send(cli_sock, buff, 1024, 0);
            }
            else
            {
                FILE *fp = fopen("res/x31_f18.ascii.pgm", "r");

                _sendImage(fp, cli_sock);

                // free file handler
                fclose(fp);
            }
        }
    }

    close(cli_sock);
}

void *_chat(void *sockfd)
{
    int cli_sock = *((int *)sockfd);
    while (1)
    {
        char buff[1024];
        int len_m = recv(cli_sock, buff, 1024, 0);
        buff[len_m] = '\0'; // (\0 at position = length: end of string)

        if (strcmp(buff, "~LST~") == 0 || strcmp(buff, "~TXT~") == 0)
        {
            len_m = recv(cli_sock, buff, 1024, 0);
            buff[len_m] = '\0'; // (\0 at position = length: end of string)

            printf("%s\n", buff);
        }
        else
        {
            FILE *fp = fopen("res/recv_img.ascii.pgm", "w+");

            _recvImage(fp, cli_sock);

            printf(" image received\n");

            // free file handler
            fclose(fp);
        }
    }
}

void _sendImage(FILE *fp, int connfd)
{
    uint32_t _net_b;

    // read and send magic number (file format)
    char _magic[8];
    fgets(_magic, sizeof(_magic), fp);
    send(connfd, &_magic, sizeof(_magic), 0);

    // read and send width of image
    int _width;
    fscanf(fp, "%d ", &_width);
    _net_b = htonl(_width);
    send(connfd, &_net_b, sizeof(_net_b), 0);

    // read and send height of image
    int _height;
    fscanf(fp, "%d ", &_height);
    _net_b = htonl(_height);
    send(connfd, &_net_b, sizeof(_net_b), 0);

    // read and send max grayscale value
    int _gmax;
    fscanf(fp, "%d ", &_gmax);
    _net_b = htonl(_gmax);
    send(connfd, &_net_b, sizeof(_net_b), 0);

    // read, invert, and send per pixel value
    for (int i = 0; i < _width * _height; i++)
    {
        int _temp;
        fscanf(fp, "%d ", &_temp);
        _net_b = htonl(_temp);
        send(connfd, &_net_b, sizeof(_net_b), 0);
    }

    printf("-- image sent\n");
}

void _recvImage(FILE *fp, int connfd)
{
    // receive and write magic number (file format)
    char _magic[8];
    recv(connfd, _magic, sizeof(_magic), 0);
    fprintf(fp, "%s", _magic);

    // receive and write width of image
    int _width;
    recv(connfd, &_width, sizeof(_width), 0);
    fprintf(fp, "%d ", ntohl(_width));

    // receive and write height of image
    int _height;
    recv(connfd, &_height, sizeof(_height), 0);
    fprintf(fp, "%d ", ntohl(_height));

    // receive and write max grayscale value
    int _gmax;
    recv(connfd, &_gmax, sizeof(_gmax), 0);
    fprintf(fp, "%d ", ntohl(_gmax));

    // read and write per pixel value
    for (int i = 0; i < ntohl(_width) * ntohl(_height); i++)
    {
        uint32_t _temp;
        recv(connfd, &_temp, sizeof(_temp), 0);
        fprintf(fp, "%d ", ntohl(_temp));
    }
}