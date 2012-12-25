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

#define ADDR "127.0.0.1"
#define PORT 9999
#define BUFFER_SIZE 1000


char DBName[BUFFER_SIZE] = "\0";
int sockfd = -1;

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

    if ( CommandMatching(command, "\\?") == 0 ) {
        HELP_INFO();
    } 
    else if ( CommandMatching(command, "open( +|\\t).+\\.hdb$") == 0 ) {
        if (strlen(DBName) != 0) {
            printf("Close current data file '%s' first.\n", DBName);
        }
        else {
            sscanf(command, "open %s", DBName);
            CreateMsg1(buffer, &send_size, OPEN, DBName, strlen(DBName));
            SendMsg(sockfd, buffer, send_size);
            back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
            AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
            if (back_code == OPEN_OK) {
                printf("Open successfully.\n");
            }
            else if (back_code == ERROR) {
                printf("Open failed. Try again.\n");
                DBName[0] = '\0';
            }
        }
    }
    else if ( DBName[0] == '\0' && CommandMatching(command, "put|get|delete|close") == 0 ) {
        printf("Open a data file first!\n");
    }
    else if ( CommandMatching(command, "put( +|\\t)[0-9]+( +|\\t).+") == 0 ) {
        int key;
        char p[BUFFER_SIZE] = "\0";
        char temp[BUFFER_SIZE] = "\0";
        sscanf(command, "put %d %s", &key, p);
        char *l = strstr(command, p);
        strcpy(temp, l);
        CreateMsg2(buffer, &send_size, PUT, &key, sizeof(int), temp, strlen(temp));
        SendMsg(sockfd, buffer, send_size);
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == PUT_OK) {
            printf("Put Successfully.\n");
        }
        else if (back_code == ERROR) {
            printf("Put failed. \n");
        }
    }
    else if ( CommandMatching(command, "get( +|\\t)[0-9]+$") == 0 ) {
        int key;
        sscanf(command, "get %d", &key);
        CreateMsg1(buffer, &send_size, GET, &key, sizeof(int));
        SendMsg(sockfd, buffer, send_size);
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == GET_OK) {
            data1[size1] = '\0';
            printf("record: %d -> %s\n", key, data1);
        }
        else if (back_code == ERROR) {
            printf("Can't find record key = %d .\n", key);
        }
    }
    else if ( CommandMatching(command, "delete( +|\\t)[0-9]+$") == 0 ) {
        int key = -1;
        sscanf(command, "delete %d", &key);
        CreateMsg1(buffer, &send_size, DELETE, &key, sizeof(int));
        SendMsg(sockfd, buffer, send_size);
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == DELETE_OK) {
            printf("Delete record key = %d successfully.\n", key);
        }
        else if (back_code == ERROR) {
            printf("Delete failed.\n");
        }
    }
    else if ( CommandMatching(command, "close") == 0 ) {
        if (DBName[0] != '\0') {
            CreateMsg0(buffer, &send_size, CLOSE);
            SendMsg(sockfd, buffer, send_size);
            back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
            AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
            if (back_code == CLOSE_OK) {
                printf("Close successfully.\n");
            }
            DBName[0] = '\0';
        }
    }
    else if ( CommandMatching(command, "exit") == 0 ) {
        CreateMsg0(buffer, &send_size, EXIT);
        SendMsg(sockfd, buffer, send_size);
        CloseSocket(sockfd);
        exit(0);
    }
    else {
        printf("Wrong Command, try again.\n");
    }
}

int main() {
    char buffer[BUFFER_SIZE];
    // init
    sockfd = InitializeClient(ADDR, PORT);
	// receive welcome message from the server
	int len = RecvMsg(sockfd, &buffer, BUFFER_SIZE);
	buffer[len] = '\0';
	printf("%s", buffer);

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

