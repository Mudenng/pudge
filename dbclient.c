/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  dbclient.c                             */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  dbclient                               */
/*  MODULE NAME           :  dbclient                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/12/14                             */
/*  DESCRIPTION           :  Client of key-value database           */
/********************************************************************/

/*
 * Revision log:
 *
 * Client, created by Pudge Group, 2012/12/14
 * Fix some bugs, lei, 2012/12/21
 *
 */

#include "network.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define HELP_INFO()                                                 \
        printf("-------------------------------------------\n");    \
        printf("          You can do these:\n");                    \
        printf("-------------------------------------------\n");    \
        printf(" '?'             - Get help\n");                    \
        printf(" 'open filename' - Open a data file\n");            \
        printf(" 'put key value' - Store a key-value\n");           \
        printf(" 'get key'       - Get value by key\n");            \
        printf(" 'delete key'    - Delete a key-value\n");          \
        printf(" 'close'         - Close current data file\n");     \
        printf(" 'exit'          - Exit\n");                        \
        printf("-------------------------------------------\n");

//#define ADDR "127.0.0.1"
//#define PORT 9999
#define BUFFER_SIZE 1000
#define SERVER_NUM 3 

typedef struct {
    char addr[50];
    int port;
    int sockfd;
}server_link;

char DBName[BUFFER_SIZE] = "\0";
//int sockfd = -1;
server_link servers[SERVER_NUM];

int CommandMatching(char *command, char *pattern) {
    regex_t reg;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regcomp(&reg, pattern, REG_EXTENDED | REG_ICASE | REG_NOSUB);
    if ( regexec(&reg, command, nmatch, pmatch, 0) == 0 )
    {
        regfree(&reg);
        return 0;
    }
    regfree(&reg);
    return -1;
}

void ExecCommand(char *command) {
    char buffer[BUFFER_SIZE];
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int back_code;
    int size1;
    int size2;
    int back_size;
    int send_size;
    int i;

    if ( CommandMatching(command, "\\?") == 0 ) {
        HELP_INFO();
    } 
    // OPEN
    else if ( CommandMatching(command, "open( +|\\t).+\\.hdb$") == 0 ) {
        if (strlen(DBName) != 0) {
            printf("Close current data file '%s' first.\n", DBName);
        }
        else {
            sscanf(command, "open %s", DBName);
            for(i = 0; i < SERVER_NUM; ++i) {
                CreateMsg1(buffer, &send_size, OPEN, DBName, strlen(DBName));
                SendMsg(servers[i].sockfd, buffer, send_size);
                back_size = RecvMsg(servers[i].sockfd, buffer, BUFFER_SIZE);
                AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
                if (back_code == ERROR) {
                    printf("Open failed. Try again.\n");
                    return;
                }
            }
            printf("Open successfully.\n");
        }
    }
    else if ( DBName[0] == '\0' && CommandMatching(command, "put|get|delete|close") == 0 ) {
        printf("Open a data file first!\n");
    }
    // PUT
    else if ( CommandMatching(command, "put( +|\\t)[0-9]+( +|\\t).+") == 0 ) {
        int key;
        char p[BUFFER_SIZE] = "\0";
        char temp[BUFFER_SIZE] = "\0";
        sscanf(command, "put %d %s", &key, p);
        char *l = strstr(command, p);
        strcpy(temp, l);
        CreateMsg2(buffer, &send_size, PUT, &key, sizeof(int), temp, strlen(temp));
        SendMsg(servers[key % SERVER_NUM].sockfd, buffer, send_size);
        back_size = RecvMsg(servers[key % SERVER_NUM].sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == PUT_OK) {
            printf("Put Successfully.\n");
        }
        else if (back_code == ERROR) {
            printf("Put failed. \n");
        }
    }
    // GET
    else if ( CommandMatching(command, "get( +|\\t)[0-9]+$") == 0 ) {
        int key;
        sscanf(command, "get %d", &key);
        CreateMsg1(buffer, &send_size, GET, &key, sizeof(int));
        SendMsg(servers[key % SERVER_NUM].sockfd, buffer, send_size);
        printf("0\n");
        back_size = RecvMsg(servers[key % SERVER_NUM].sockfd, buffer, BUFFER_SIZE);
        printf("1\n");
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        printf("2\n");
        if (back_code == GET_OK) {
            data1[size1] = '\0';
            printf("record: %d -> %s\n", key, data1);
        }
        else if (back_code == ERROR) {
            printf("Can't find record key = %d .\n", key);
        }
    }
    // DELETE
    else if ( CommandMatching(command, "delete( +|\\t)[0-9]+$") == 0 ) {
        int key = -1;
        sscanf(command, "delete %d", &key);
        CreateMsg1(buffer, &send_size, DELETE, &key, sizeof(int));
        SendMsg(servers[key % SERVER_NUM].sockfd, buffer, send_size);
        back_size = RecvMsg(servers[key % SERVER_NUM].sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == DELETE_OK) {
            printf("Delete record key = %d successfully.\n", key);
        }
        else if (back_code == ERROR) {
            printf("Delete failed.\n");
        }
    }
    // CLOSE
    else if ( CommandMatching(command, "close") == 0 ) {
        for(i = 0; i < SERVER_NUM; ++i) {
            CreateMsg0(buffer, &send_size, CLOSE);
            SendMsg(servers[i].sockfd, buffer, send_size);
            back_size = RecvMsg(servers[i].sockfd, buffer, BUFFER_SIZE);
            AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        }
        DBName[0] = '\0';
        printf("Close successfully.\n");
    }
    // EXIT
    else if ( CommandMatching(command, "exit") == 0 ) {
        for(i = 0; i < SERVER_NUM; ++i) {
            CreateMsg0(buffer, &send_size, EXIT);
            SendMsg(servers[i].sockfd, buffer, send_size);
            CloseSocket(servers[i].sockfd);
        }
        exit(0);
    }
    // else
    else {
        printf("Wrong Command, try again.\n");
    }
}

int main() {
    char line[BUFFER_SIZE];
    FILE *fp;
    if ( (fp = fopen("client.conf","r")) == NULL ) {
        printf("Open configure file error.\n");
        exit(-1);
    }
    int i = 0;
    while (fgets(line,BUFFER_SIZE, fp) && i < SERVER_NUM) {
        sscanf(line, "%s %d", servers[i].addr, &servers[i].port);
        ++i;
    }

    // init socket
    for(i = 0; i < SERVER_NUM; ++i) {
        servers[i].sockfd = InitializeClient(servers[i].addr, servers[i].port);
    }

	// receive welcome message from the server
    char buffer[BUFFER_SIZE];
    for(i = 0; i < SERVER_NUM; ++i) {
        int len = RecvMsg(servers[i].sockfd, &buffer, BUFFER_SIZE);
        buffer[len] = '\0';
        printf("Server(%s:%d) returns: %s",servers[i].addr, servers[i].port, buffer);
    }

    char cmdbuf[BUFFER_SIZE];
    HELP_INFO();
    while(1) {
        printf("Pudge@%s > ", DBName);
        fgets(cmdbuf, BUFFER_SIZE, stdin);
        cmdbuf[strlen(cmdbuf) - 1] = '\0';
        ExecCommand(cmdbuf);
    }
    return 0;
}

