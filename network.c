/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  network.c                              */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  network                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  Interface to socket operation          */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/12/14
 *
 */

#include "network.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Create socket
 */
int PrepareSocket() {
	int sockfd = -1;

	// create socket
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sockfd >= 0);
	
	return sockfd;
}

/*
 * Init server address , bind socket, listen
 */
int InitServer(int sockfd, char *ip, int port, int max_conn) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	memset(&addr.sin_zero, 0, 8);

	// bind 
	if ( bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr) ) < 0 ) {
		printf("bind error.\n");
		close(sockfd);
		return -1;
	}

	// listen
	if ( listen(sockfd, max_conn) < 0 ) {
		printf("listen error.\n");
		close(sockfd);
		return -1;
	}
    return 0;
}

/*
 * Wait a client
 */
int WaitClient(int server_sockfd, struct sockaddr_in *client_addr, int *sin_size) {
	int client_sockfd = -1;

	memset(client_addr, 0, sizeof(struct sockaddr_in));
	// accept cilent's connection
	client_sockfd = accept(server_sockfd, (struct sockaddr *)client_addr, sin_size);
	if (client_sockfd < 0) {
		printf("accept error.\n");
	}
	printf("accept client - %s\n", inet_ntoa(client_addr->sin_addr));

	// send welcome message to client
	int len = SendMsg(client_sockfd, "Welcome to this server!\n", 24);
	if (len < 0) {
		printf("send welcome error.\n");
	}	
	return client_sockfd;
}

/*
 * Init server
 */
int InitializeServer(char *ip, int port, int max_conn) {
    // prepare socket
	int server_sockfd = PrepareSocket();
	if ( InitServer(server_sockfd, ip, port, max_conn) != 0 ) {
		printf("Server start error.\n");
		return -1;
	}
	printf("Server start ok.\n");
    return server_sockfd;
}

/*
 * Init client
 */
int InitializeClient(char *ip, int port) {
    // prepare socket
    int client_sockfd = PrepareSocket();

    // init
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	memset(&server_addr.sin_zero, 0, 8);

    // connect to server
	if ( connect(client_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0 ) {
		printf("connect error.\n");
		close(client_sockfd);
		return -1;
	}
    printf("connected server(%s:%d) successfully.\n", ip, port);
    return client_sockfd;
}

/*
 * Close server
 */
void CloseServer(int server_sockfd, int client_sockfd) {
    close(server_sockfd);
    close(client_sockfd);
}

/*
 * Close client
 */
void CloseClient(int client_sockfd) {
    close(client_sockfd);
}

/*
 * Send a message
 */
int SendMsg(int sockfd, void *buf, int size) {
    int len = send(sockfd, buf, size, 0);
    if (len < 0)
        return -1;
    return len;
}

/*
 * Received a message
 */
int RecvMsg(int sockfd, void *buf, int max_size) {
    int len = recv(sockfd, buf, max_size, 0);
    if (len < 0)
        return -1;
    return len;
}


