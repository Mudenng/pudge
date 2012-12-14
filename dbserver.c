/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  dbserver.c                             */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  dbserver                               */
/*  MODULE NAME           :  dbserver                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  Server of key-value database           */
/********************************************************************/

/*
 * Revision log:
 *
 * Server, created by Pudge Group, 2012/12/14
 *
 */

#include "hdbapi.h"
#include "network.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


#define ADDR "127.0.0.1"
#define PORT 9999
#define MAX_CONNECTS 100
#define BUFFER_SIZE 1000

DBHANDLE db = NULL;

int main() {
    int server_sockfd = InitializeServer(ADDR, PORT, MAX_CONNECTS);
    int client_sockfd;
    struct sockaddr_in client_addr;
    int sin_size = sizeof(struct sockaddr_in);
    if (server_sockfd != -1) {
        client_sockfd = WaitClient(server_sockfd, &client_addr, &sin_size);
    }

    while (1) {
        char buffer[BUFFER_SIZE];
        char data1[BUFFER_SIZE];
        char data2[BUFFER_SIZE];
        int cmd_code;
        int size1;
        int size2;
        int back_size;
        int len = RecvMsg(client_sockfd, &buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
        switch(cmd_code) {
            case OPEN:
                data1[size1] = '\0';
                db = OpenHDB(data1);
                if (db != NULL) {
                    CreateMsg0(buffer, &back_size, OPEN_OK);
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
                SendMsg(client_sockfd, buffer, back_size);
                printf("handled 'OPEN %s' from '%s'\n", data1, inet_ntoa(client_addr.sin_addr));
                break;
            case CLOSE:
                CloseHDB(db);
                db = NULL;
                CreateMsg0(buffer, &back_size, CLOSE_OK);
                SendMsg(client_sockfd, buffer, back_size);
                printf("handled 'CLOSE' from '%s'\n", inet_ntoa(client_addr.sin_addr));
                break;
            case EXIT:
                if (db)
                    CloseHDB(db);
                db = NULL;
                CreateMsg0(buffer, &back_size, CLOSE_OK);
                SendMsg(client_sockfd, buffer, back_size);
                CloseClient(client_sockfd);
                printf("handled 'EXIT' from '%s'\n", inet_ntoa(client_addr.sin_addr));
                printf("client '%s' disconnected.\n", inet_ntoa(client_addr.sin_addr));
                break;
            case PUT:
                if (db != NULL) {
                    int key = (int)*data1;
                    value_struct value;
                    data2[size2] = '\0';
                    value.content = data2;
                    value.size = strlen((char *)value.content);
                    if (PutKeyValue(db, key, &value) == 0) {
                        CreateMsg0(buffer, &back_size, PUT_OK);
                    }
                    else {
                        CreateMsg0(buffer, &back_size, ERROR);
                    }
                    printf("handled 'PUT %d %s' from '%s'\n", key, data2, inet_ntoa(client_addr.sin_addr));
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
                SendMsg(client_sockfd, buffer, back_size);
                break;
            case GET:
                if (db != NULL) {
                    int key = (int)*data1;
                    value_struct *value;
                    value = GetValue(db, key);
                    if (value) {
                        CreateMsg1(buffer, &back_size, GET_OK, value->content, value->size);
                        FreeValueStruct(value);
                    }
                    else {
                        CreateMsg0(buffer, &back_size, ERROR);
                    }
                    printf("handled 'GET %d' from '%s'\n", key, inet_ntoa(client_addr.sin_addr));
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
                SendMsg(client_sockfd, buffer, back_size);
                break;
            case DELETE:
                if (db != NULL) {
                    int key = (int)*data1;
                    if (DelKeyValue(db, key) == 0) {
                        CreateMsg0(buffer, &back_size, DELETE_OK);
                    }
                    else {
                        CreateMsg0(buffer, &back_size, ERROR);
                    }
                    printf("handled 'DELETE %d' from '%s'\n", key, inet_ntoa(client_addr.sin_addr));
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
                SendMsg(client_sockfd, buffer, back_size);
                break;

        }
    }
    CloseServer(server_sockfd, client_sockfd);
    return 0;
}
