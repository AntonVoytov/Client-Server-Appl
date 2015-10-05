#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 7500
#define BUFF_SIZE 1024
#define LOCAL "127.0.0.1"

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
            printf("server: ");
            fputs(buff, stdout);
        }
    }
    pthread_exit(NULL);
}

int main()
{
    int connSock, n;
    struct sockaddr_in client;
    char toServer[BUFF_SIZE];
    pthread_t thread;

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(LOCAL);
    client.sin_port = htons(PORT);

    connSock = socket(AF_INET, SOCK_STREAM, 0);
    if (connSock < 0)
    {
        perror("Error socket\n");
        exit(EXIT_FAILURE);
    }

    if (connect(connSock, (struct sockaddr*) &client, sizeof(client)) < 0)
    {
        perror("Error connect\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
    n = pthread_create(&thread, NULL, receive, (void*) connSock);
    if (n < 0)
    {
        printf("Error thread code %d\n", n);
        exit(EXIT_FAILURE);
    }

    while(fgets(toServer, BUFF_SIZE, stdin) != NULL)
    {
        n = send(connSock, toServer, BUFF_SIZE, 0);
        if (n < 0)
        {
            perror("Error sending data\n");
        }
        if (strcmp(toServer, "q") == 0) break;
    }

    pthread_join(thread, NULL);
    close(connSock);
    return 0;
}
