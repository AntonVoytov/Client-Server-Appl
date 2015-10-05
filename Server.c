#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 7500
#define BUFF_SIZE 1024

void *receive(void *socket)
{
    int sock, n;
    char buff[BUFF_SIZE];
    sock = (int)socket;
    memset(buff, 0, BUFF_SIZE);
    while(1)
    {
        n = recv(sock, buff, BUFF_SIZE, 0);
        if (n < 0)
        {
            printf("Error receiving\n");
        }
        else
        {
            send(sock, buff, BUFF_SIZE, 0);
        }
    }
    pthread_exit(NULL);
}

int main()
{
    int bindSock, connSock, serverSize, n;
    struct sockaddr_in server;
    pthread_t thread;
    pid_t childpid;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    bindSock = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(bindSock, (struct sockaddr*) &server, sizeof(server)) < 0)
    {
        perror("Error bind");
        exit(EXIT_FAILURE);
    }

    if (listen(bindSock, 5) < 0)
    {
        perror("Error listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening....\n");

    while(1)
    {
        serverSize = sizeof(server);
        connSock = accept(bindSock, (struct sockaddr*) &server, &serverSize);
        if (connSock < 0)
        {
            perror("Error accept\n");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");

        if ((childpid = fork()) == 0)
        {
            close(bindSock);

            n = pthread_create(&thread, NULL, receive, (void*)connSock);
            if (n < 0)
            {
                printf("Error thread code %d\n", n);
                exit(EXIT_FAILURE);
            }
            pthread_join(thread, NULL);
        }
        close(connSock);
    }

    return 0;
}
