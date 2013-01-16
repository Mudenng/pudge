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


#ifndef PUDGE_NETWORK_H
#define PUDGE_NETWORK_H

#include <event2/event.h>
#include <arpa/inet.h>

#define MAX_CONNS 1024

typedef struct {
    char *ip;
    int port;
    int max_conn;
    event_callback_fn recv_callback_fn;
}SERVER_START_ARG;

/*
 * Start server, init event and wait client 
 * input    : ip, port, max_conn, recv_callback_fn
 * output   : server_sockfd
 * in/out   : None
 * return   : if SUCCESS return server_sockfd
 *          : if FAILURE return -1
 */
int StartServer(void *arg);

/*
 * Start Master, init event and wait client 
 * input    : ip, port, max_conn, recv_callback_fn
 * output   : server_sockfd
 * in/out   : None
 * return   : if SUCCESS return server_sockfd
 *          : if FAILURE return -1
 */
int StartMaster(void *arg);

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
 * Close socket fd
 * input    : sockfd
 * output   : None
 * in/out   : None
 * return   : None
 */
void CloseSocket(int sockfd);

/*
 * Get a free port
 * input    : None
 * output   : None
 * in/out   : None
 * return   : a free port
 */
int get_free_port();

/*
 * Show all local ips
 * input    : None
 * output   : all IPs
 * in/out   : None
 */
void show_local_ip();

/*
 * pick an ip
 * input    : ip index
 * output   : ip
 * in/out   : None
 */
void pick_local_ip(int index, char *ip);

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
