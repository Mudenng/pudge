/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  protocol.c                             */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  protocol                               */
/*  MODULE NAME           :  protocol                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  protocol between server and client     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/12/14
 *
 */

#include "protocol.h"
#include <stdlib.h>
#include <string.h>

/*
 * Create message with 0 data
 */
int CreateMsg0(void *buf, int *size, int cmd_code) {
    memcpy(buf, (void *)&cmd_code, sizeof(int));
    *size = sizeof(int);
    return 0;
}

/*
 * Create message with 1 data
 */

int CreateMsg1(void *buf, int *size, int cmd_code, void *data1, int size1) {
    memcpy(buf, (void *)&cmd_code, sizeof(int));
    memcpy((char *)buf + sizeof(int), (void *)&size1, sizeof(int));
    memcpy((char *)buf + sizeof(int) * 2, data1, size1);
    *size = sizeof(int) * 2 + size1;
    return 0;
}

/*
 * Create message with 2 data
 */
int CreateMsg2(void *buf, int *size, int cmd_code, void *data1, int size1, void *data2, int size2) {
    memcpy(buf, (void *)&cmd_code, sizeof(int));
    memcpy((char *)buf + sizeof(int), (void *)&size1, sizeof(int));
    memcpy((char *)buf + sizeof(int) * 2, data1, size1);
    memcpy((char *)buf + sizeof(int) * 2 + size1, (void *)&size2, sizeof(int));
    memcpy((char *)buf + sizeof(int) * 3 + size1, data2, size2);
    *size = sizeof(int) * 3 + size1 + size2;
    return 0;
}

/*
 * Analyse message
 */
int AnalyseMsg(void *buf, int *cmd_code, void *data1, int *size1, void *data2, int *size2) {
    ((char *)data1)[0] = '\0';
    ((char *)data2)[0] = '\0';
    *size1 = *size2 = 0;
    memcpy(cmd_code, buf, sizeof(int));
    switch(*cmd_code) {
        case CLOSE:
        case OPEN_OK:
        case PUT_OK:
        case CLOSE_OK:
        case DELETE_OK:
        case ERROR:
            break;
        case OPEN:
        case GET:
        case GET_OK:
        case DELETE:
            memcpy(size1, buf + sizeof(int), sizeof(int));
            memcpy(data1, buf + sizeof(int) * 2, *size1);
            break;
        case PUT:
            memcpy(size1, buf + sizeof(int), sizeof(int));
            memcpy(data1, buf + sizeof(int) * 2, *size1);
            memcpy(size2, buf + sizeof(int) * 2 + *size1, sizeof(int));
            memcpy(data2, buf + sizeof(int) * 3 + *size1, *size2);
            // printf("A d2 %s\n", data2);
            break;
    }
    return 0;
}

