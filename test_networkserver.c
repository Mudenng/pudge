#include "network.h"
#include <stdio.h>
#include <string.h>

#define ADDR "127.0.0.1"
#define PORT 8888
#define MAX_CONNECTS 100
#define BUFFER_SIZE 1000

int main () {
    printf("*****Test network_server...*****\n");
    int server_sockfd = InitializeServer(ADDR, PORT, MAX_CONNECTS);
    int client_sockfd;
    struct sockaddr_in client_addr;
    int sin_size = sizeof(struct sockaddr_in);
    if (server_sockfd != -1) {
        client_sockfd = WaitClient(server_sockfd, &client_addr, &sin_size);
    }
    char buffer[BUFFER_SIZE];
    int len = RecvMsg(client_sockfd, &buffer, BUFFER_SIZE);
    buffer[len] = '\0';
    if (strcmp(buffer, "hello") != 0) {
        printf("Server receive msg failed.\n");
    }

    close(server_sockfd);
    close(client_sockfd);
    return 0;

}
