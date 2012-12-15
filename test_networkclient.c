#include "network.h"
#include <stdio.h>
#include <string.h>

#define ADDR "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1000

int main () {
    printf("*****Test network_client...*****\n");
    char buffer[BUFFER_SIZE];
    // init
    int sockfd = InitializeClient(ADDR, PORT);
	// receive welcome message from the server
	int len = RecvMsg(sockfd, &buffer, BUFFER_SIZE);
	buffer[len] = '\0';
    if (strcmp(buffer, "Welcome to this server!\n") != 0) {
        printf("Client receive msg failed.\n");
    }
    len = SendMsg(sockfd, "hello", 5);
	if (len < 0) {
		printf("Client send msg failed.\n");
	}
    close(sockfd);
    return 0;
}
