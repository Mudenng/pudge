/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  protocol.h                             */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  protocol                               */
/*  MODULE NAME           :  protocol                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  protocol between server and client     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/12/14
 *
 */

#ifndef PUDGE_PROTOCOL_H
#define PUDGE_PROTOCOL_H

/* define command code */
#define OPEN 1
#define CLOSE 2
#define PUT 3
#define GET 4
#define DELETE 5
#define EXIT 6

#define OPEN_OK -1
#define CLOSE_OK -2
#define PUT_OK -3
#define GET_OK -4
#define DELETE_OK -5

#define NEW_SERVER 100
#define GET_SERVER_LIST 101
#define SERVER_LIST_OK 102
#define MASTER_OK 199

#define ERROR 0

#define ADDR_LEN 30
typedef struct {
    char addr[ADDR_LEN];
    int port;
}SERVER_INFO;

/*
 * Create message - 0-2 data
 * input    : command code, data, data size
 * in/out   : buf - store message
 * in/out   : size - message size
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int CreateMsg0(void *buf, int *size, int cmd_code);
int CreateMsg1(void *buf, int *size, int cmd_code, void *data1, int size1);
int CreateMsg2(void *buf, int *size, int cmd_code, void *data1, int size1, void *data2, int size2);

/*
 * AnalyseMsg - 0-2 data
 * input	: buf - point to received message 
 * output   : command code
 * output   : data1, data1_size, data2, data2_size; if no data, *dataX == '\0', *data_size == 0
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int AnalyseMsg(void *buf, int *cmd_code, void *data1, int *size1, void *data2, int *size2);

#endif
