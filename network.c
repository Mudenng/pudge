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


#include "network.h"

#include <event2/event.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    struct event_base *base;
    event_callback_fn callback_fn;
}callback_arg;

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
 * Accept client callback function
 */
void AcceptHandle(int server_sockfd, short event, void *arg) {
    callback_arg *callarg = (callback_arg *)arg;
    struct event_base *base = callarg->base;
    event_callback_fn recv_callback_fn = callarg->callback_fn;
    struct sockaddr_in clientaddr;
    socklen_t addr_len = sizeof(struct sockaddr);
    
    // accept client connection
    int newfd = accept(server_sockfd, (struct sockaddr*)&clientaddr, &addr_len);
    if (newfd < 0) {
        printf("Accept error.\n");
        return;
    }
    printf("Accept client (%s) at sockfd = %d\n", inet_ntoa(clientaddr.sin_addr), newfd);

    // add new client event
    struct event *recv_event = event_new(base, newfd, EV_READ | EV_PERSIST, recv_callback_fn, NULL);
    event_add(recv_event, NULL);

    // send welcome message to client
	int len = SendMsg(newfd, "Welcome to this server!\n", 24);
	if (len < 0) {
		printf("Send welcome to %s error\n", inet_ntoa(clientaddr.sin_addr));
	}
}

/*
 * Start server init event, and wait client
 */
int StartServer(void *arg) {
    char *ip = ((SERVER_START_ARG *)arg)->ip;
    int port = ((SERVER_START_ARG *)arg)->port;
    int max_conn = ((SERVER_START_ARG *)arg)->max_conn;
    event_callback_fn recv_callback_fn = ((SERVER_START_ARG *)arg)->recv_callback_fn;
    // prepare socket
	int server_sockfd = PrepareSocket();
	if ( InitServer(server_sockfd, ip, port, max_conn) != 0 ) {
		printf("Server start error.\n");
		return -1;
	}

    // make non_blocking
    evutil_make_listen_socket_reuseable(server_sockfd);

    // init event
    struct event_base *base = event_base_new();
    assert(base != NULL);
    struct event *listen_event;
    callback_arg callarg;
    callarg.base = base;
    callarg.callback_fn = recv_callback_fn;
    listen_event = event_new(base, server_sockfd, EV_READ|EV_PERSIST, AcceptHandle, (void *)&callarg);
    event_add(listen_event, NULL);

    // start event
	printf("Server Start\n");
    event_base_dispatch(base);

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
		printf("Connect error.\n");
		close(client_sockfd);
		return -1;
	}
    printf("Connect to server(%s:%d) successfully\n", ip, port);
    return client_sockfd;
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

/*
 * Close a socket fd
 */
void CloseSocket(int sockfd) {
    close(sockfd);
}

/*
 * Get a free port
 */
int get_free_port()
{
    int fd = -1;

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(0);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(fd < 0){
        printf("socket() error\n");
         return -1;
    }
    
    if(bind(fd, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        printf("bind() error\n");
        close(fd);
        return -1;
    }

    int len = sizeof(sin);
    if(getsockname(fd, (struct sockaddr *)&sin, &len) != 0)
    {
        printf("getsockname() error\n");
        close(fd);
        return -1;
    }

    if(fd != -1)
        close(fd);
    
    return sin.sin_port;
}

int get_local_ip(char *interface_name, char *ip) {
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct != NULL) {
        // check it is IP4
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { 
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp(ifAddrStruct->ifa_name, interface_name) == 0) {
                strncpy(ip, addressBuffer, INET_ADDRSTRLEN);
                return 1;
            }
            // printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer); 
        }   
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return -1;
}
