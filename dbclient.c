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

#define BUFFER_SIZE 1000

typedef struct {
    char addr[ADDR_LEN];
    int port;
    int sockfd;
}server_link;

int master_sockfd;
char DBName[BUFFER_SIZE] = "\0";

int servers_cnt = 0;
server_link *servers;
// server_link servers[servers_cnt];

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
            for(i = 0; i < servers_cnt; ++i) {
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
        SendMsg(servers[key % servers_cnt].sockfd, buffer, send_size);
        back_size = RecvMsg(servers[key % servers_cnt].sockfd, buffer, BUFFER_SIZE);
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
        SendMsg(servers[key % servers_cnt].sockfd, buffer, send_size);
        back_size = RecvMsg(servers[key % servers_cnt].sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
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
        SendMsg(servers[key % servers_cnt].sockfd, buffer, send_size);
        back_size = RecvMsg(servers[key % servers_cnt].sockfd, buffer, BUFFER_SIZE);
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
        for(i = 0; i < servers_cnt; ++i) {
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
        for(i = 0; i < servers_cnt; ++i) {
            CreateMsg0(buffer, &send_size, EXIT);
            SendMsg(servers[i].sockfd, buffer, send_size);
            CloseSocket(servers[i].sockfd);
        }
        CloseSocket(master_sockfd);
        exit(0);
    }
    // else
    else {
        printf("Wrong Command, try again.\n");
    }
}

int main() {
    // speck with master
    char master_addr[ADDR_LEN];
    int master_port;
    while(1) {
        printf("Master Address: ");
        scanf("%s", master_addr);
        printf("Master Port: ");
        scanf("%d", &master_port);
        printf("\n");
        printf("*************** Init Start ****************\n");
        master_sockfd = InitializeClient(master_addr, master_port);
        if (master_sockfd < 0) {
            printf("Connect to Master failed, try again.\n\n");
            continue;
        }
        break;
    }

    char buf[BUFFER_SIZE];
    int len = RecvMsg(master_sockfd, &buf, BUFFER_SIZE);
    memset(buf, 0, BUFFER_SIZE);
    
    int size1, size2, send_size, back_size, back_code;
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    CreateMsg0(buf, &send_size, GET_SERVER_LIST);
    SendMsg(master_sockfd, buf, send_size);
    back_size = RecvMsg(master_sockfd, buf, BUFFER_SIZE);
    AnalyseMsg(buf, &back_code, data1, &size1, data2, &size2);
    if (back_code != SERVER_LIST_OK) {
        printf("Master server error.\n");
        exit(-1);
    }
    servers_cnt = *(int *)data1;
    if (servers_cnt == 0) {
        printf("No server avaliable.\n");
        exit(-1);
    }
    servers = (server_link *)malloc(servers_cnt * sizeof(server_link));
    int i;
    for(i = 0; i < servers_cnt; ++i) {
        memcpy(servers[i].addr, data2 + i * sizeof(SERVER_INFO), ADDR_LEN);
        memcpy(&(servers[i].port), data2 + i * sizeof(SERVER_INFO) + ADDR_LEN, sizeof(int));
    }
    printf("--------Server List---------\n");
    for(i = 0; i < servers_cnt; ++i) {
        printf("Server %d : %s:%d\n", i, servers[i].addr, servers[i].port);
    }
    printf("----------------------------\n");

    // init socket
    for(i = 0; i < servers_cnt; ++i) {
        servers[i].sockfd = InitializeClient(servers[i].addr, servers[i].port);
    }

	// receive welcome message from the server
    char buffer[BUFFER_SIZE];
    for(i = 0; i < servers_cnt; ++i) {
        int len = RecvMsg(servers[i].sockfd, &buffer, BUFFER_SIZE);
        buffer[len] = '\0';
        printf("Server %d (%s:%d) returns: %s", i, servers[i].addr, servers[i].port, buffer);
    }
    printf("*************** Init End ****************\n\n");

    char cmdbuf[BUFFER_SIZE];
    HELP_INFO();
    getchar();
    while(1) {
        printf("Pudge@%s > ", DBName);
        fgets(cmdbuf, BUFFER_SIZE, stdin);
        cmdbuf[strlen(cmdbuf) - 1] = '\0';
        ExecCommand(cmdbuf);
    }
    
    CloseSocket(master_sockfd);
    return 0;
}

