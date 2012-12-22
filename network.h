/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  network.h                              */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  network                                */
/*  MODULE NAME           :  network                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  Interface to socket operation          */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/12/14
 *
 */

#ifndef PUDGE_NETWORK_H
#define PUDGE_NETWORK_H

#include <arpa/inet.h>

#define MAX_CONNS 1024

/*
 * Init server
 * input    : address, port, max_connects_num
 * output   : server_sockfd
 * in/out   : None
 * return   : if SUCCESS return server_sockfd
 *          : if FAILURE return -1
 */
int InitializeServer(char *ip, int port, int max_conn);

/*
 * Wait a client
 * input    : server_sockfd, client address struct
 * output   : client_sockfd
 * in/out   : None
 * return   : if SUCCESS return client_sockfd
 *          : if FAILURE return -1
 */
int WaitClient(int server_sockfd, struct sockaddr_in *client_addr, int *sin_size);

/*
 * Init client
 * input    : address, port
 * output   : sockfd
 * in/out   : None
 * return   : if SUCCESS return sockfd
 *          : if FAILURE return -1
 */
int InitializeClient(char *ip, int port);

/*
 * Close server
 * input    : server_sockfd, client_sockfd
 * output   : None
 * in/out   : None
 * return   : None
 */
void CloseServer(int server_sockfd, int client_sockfd);

/*
 * Close client
 * input    : sockfd
 * output   : None
 * in/out   : None
 * return   : None
 */
void CloseClient(int client_sockfd);

/*
 * Send a message
 * input    : sockfd, message buffer, message size
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return sended length
 *          : if FAILURE return -1
 */
int SendMsg(int sockfd, void *buf, int size);

/*
 * Received a message
 * input    : sockfd, max message size
 * output   : message
 * in/out   : None
 * return   : if SUCCESS return received length
 *          : if FAILURE return -1
 */
int RecvMsg(int sockfd, void *buf, int max_size);

#endif
