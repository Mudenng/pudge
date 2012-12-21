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
#include "hash.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>


#define ADDR "127.0.0.1"
#define PORT 9999
#define MAX_CONNECTS 100
#define BUFFER_SIZE 1000

#define DBNAME_SIZE 20

HASHTABLE db_handle_table;

typedef struct {
    DBHANDLE hdb;
    int used;
}DB_HANDLE_NODE;


void HandleRequest(void *arg) {
    int client_sockfd = *(int *)arg;
    DBHANDLE db = NULL;
    char *dbname = (char *)malloc(DBNAME_SIZE);
    DB_HANDLE_NODE dhn;
    while(1) {
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
                memset(dbname, 0, DBNAME_SIZE);
                memcpy(dbname, data1, size1);
                if (HashGetValue(db_handle_table, dbname, &dhn) == 0) {
                    db = OpenHDB(data1);
                    if (db != NULL) {
                        dhn.hdb = db;
                        dhn.used = 1;
                        HashAddNode(db_handle_table, dbname, &dhn);
                        CreateMsg0(buffer, &back_size, OPEN_OK);
                    }
                    else {
                        CreateMsg0(buffer, &back_size, ERROR);
                    }
                }
                else {
                    db = dhn.hdb;
                    dhn.used++;
                    HashDelete(db_handle_table, dbname);
                    HashAddNode(db_handle_table, dbname, &dhn);
                    CreateMsg0(buffer, &back_size, OPEN_OK);
                }
                SendMsg(client_sockfd, buffer, back_size);
                printf("handled 'OPEN %s' from %d\n", data1, client_sockfd);
                break;
            case CLOSE:
                HashGetValue(db_handle_table, dbname, &dhn);
                dhn.used--;
                if (dhn.used <= 0) {
                    CloseHDB(db);
                    HashDelete(db_handle_table, dbname);
                }
                else {
                    HashDelete(db_handle_table, dbname);
                    HashAddNode(db_handle_table, dbname, &dhn);
                }
                db = NULL;
                memset(dbname, 0, DBNAME_SIZE);
                CreateMsg0(buffer, &back_size, CLOSE_OK);
                SendMsg(client_sockfd, buffer, back_size);
                printf("handled 'CLOSE' from %d\n", client_sockfd);
                break;
            case EXIT:
                HashGetValue(db_handle_table, dbname, &dhn);
                dhn.used--;
                if (dhn.used <= 0) {
                    CloseHDB(db);
                    HashDelete(db_handle_table, dbname);
                }
                else {
                    HashDelete(db_handle_table, dbname);
                    HashAddNode(db_handle_table, dbname, &dhn);
                }
                db = NULL;
                CreateMsg0(buffer, &back_size, CLOSE_OK);
                SendMsg(client_sockfd, buffer, back_size);
                CloseClient(client_sockfd);
                printf("handled 'EXIT' from %d\n", client_sockfd);
                printf("client %d disconnected.\n", client_sockfd);
                return;
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
                    printf("handled 'PUT %d %s' from %d\n", key, data2, client_sockfd);
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
                    printf("handled 'GET %d' from %d\n", key, client_sockfd);
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
                    printf("handled 'DELETE %d' from %d\n", key, client_sockfd);
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
                SendMsg(client_sockfd, buffer, back_size);
                break;

        }
    }
}

int main() {
    int server_sockfd = InitializeServer(ADDR, PORT, MAX_CONNECTS);
    int client_sockfd;
    struct sockaddr_in client_addr;
    int sin_size = sizeof(struct sockaddr_in);
    if (server_sockfd != -1) {
        db_handle_table = CreateTablePJW(10, DBNAME_SIZE, sizeof(DB_HANDLE_NODE));
        while(1) {
            client_sockfd = WaitClient(server_sockfd, &client_addr, &sin_size);
            pthread_t pid;
            pthread_create(&pid, NULL, (void *)HandleRequest, &client_sockfd);
            // HandleRequest(client_sockfd);
        }
    }

    CloseServer(server_sockfd, client_sockfd);
    return 0;
}

