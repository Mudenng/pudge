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
#include "linklist.h"
#include "conhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>

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

// server list struct
typedef struct {
    char addr[ADDR_LEN];
    int port;
    int sockfd;
}SERVER;

int master_sockfd;

char DBName[BUFFER_SIZE] = "\0";

CONHASH conhash;

/*
 * Command match
 */
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

/*
 * Update conhash before request
 */
int update_conhash() {
    srand((unsigned)time(0));
    char buf[BUFFER_SIZE];    
    int servers_cnt;
    int size1, size2, send_size, back_size, back_code;
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int max_try = 5;
    while(max_try) {
        // pick a random server
        int sid = rand() % ConhashGetSize(conhash);
        int sockfd = 0;
        CLinklist_Iterator cit;
        LinklistIteratorSetBegin(conhash, &cit);
        int i;
        for(i = 0; i <= sid; ++i)
            LinklistIteratorToNext(&cit);
        Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
        sockfd = ((SERVER *)(nptr->info))->sockfd;

        // set timeout
        struct timeval timeout = {3,0};
        setsockopt(sockfd, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
        setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

        // try to request new server list
        CreateMsg0(buf, &send_size, GET_SERVER_LIST);
        SendMsg(sockfd, buf, send_size);
        back_size = RecvMsg(sockfd, buf, BUFFER_SIZE);
        AnalyseMsg(buf, &back_code, data1, &size1, data2, &size2);
        if (back_size <= 0 || back_code != UPDATE_SERVER_LIST) {
            --max_try;
        }
        else {
            break;
        }
    }
    // if can't get list from servers, try to get from master
    if (back_size <= 0 || back_code != UPDATE_SERVER_LIST) {
        // set timeout
        struct timeval timeout = {3,0};
        setsockopt(master_sockfd, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
        setsockopt(master_sockfd, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

        // try to request new server list
        CreateMsg0(buf, &send_size, GET_SERVER_LIST);
        SendMsg(master_sockfd, buf, send_size);
        back_size = RecvMsg(master_sockfd, buf, BUFFER_SIZE);
        AnalyseMsg(buf, &back_code, data1, &size1, data2, &size2);
        if (back_size <= 0 || back_code != SERVER_LIST_OK) {
            printf("No server is aviliable.\n");
            return -1;
        }

    }
    servers_cnt = *(int *)data1;
    if (servers_cnt == 0) {
        printf("No server is aviliable.\n");
        return -1;
    }
    SERVER *servers = NULL;
    servers = (SERVER *)malloc(servers_cnt * sizeof(SERVER));
    int i;
    for(i = 0; i < servers_cnt; ++i) {
        memcpy(servers[i].addr, data2 + i * sizeof(SERVER_INFO), ADDR_LEN);
        memcpy(&(servers[i].port), data2 + i * sizeof(SERVER_INFO) + ADDR_LEN, sizeof(int));
    }
    // check if any node will be remove
    CLinklist_Iterator cit;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
        int flag = 0;
        for(i = 0; i < servers_cnt; ++i) {
            if (strcmp(((SERVER *)(nptr->info))->addr, servers[i].addr) == 0 && ((SERVER *)(nptr->info))->port == servers[i].port) {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
            CirLinklistDelete(&cit);
        if (LinklistIteratorAtEnd(&cit))
            break;
        LinklistIteratorToNext(&cit);
    }
    // check if any node will be add
    for(i = 0; i < servers_cnt; ++i) {
        int flag = 0;
        LinklistIteratorSetBegin(conhash, &cit);
        while(1) {
            Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
            if (strcmp(((SERVER *)(nptr->info))->addr, servers[i].addr) == 0 && ((SERVER *)(nptr->info))->port == servers[i].port) {
                flag = 1;
                break;
            }
            if (LinklistIteratorAtEnd(&cit))
            break;
            LinklistIteratorToNext(&cit);
        }
        // if find a new server
        if (flag == 0) {
            // connect to server
            servers[i].sockfd = InitializeClient(servers[i].addr, servers[i].port);
            // receive welcome message from the server
            char buffer[BUFFER_SIZE];
            int len = RecvMsg(servers[i].sockfd, &buffer, BUFFER_SIZE);
            if  (len < 0)
                printf("Server (%s:%d) connect error.\n", servers[i].addr, servers[i].port);
            // add node to consistent hash table
            if (ConhashAddNode(conhash, &(servers[i]), sizeof(SERVER)) < 0)
                printf("Add node to consistent hash error.\n");
            // open DB at the new server
            int size1, size2, send_size, back_size, back_code;
            char data1[BUFFER_SIZE];
            char data2[BUFFER_SIZE];
            CreateMsg1(buffer, &send_size, OPEN, DBName, strlen(DBName));
            SendMsg(servers[i].sockfd, buffer, send_size);
            back_size = RecvMsg(servers[i].sockfd, buffer, BUFFER_SIZE);
            AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
            if (back_code == ERROR) {
                printf("Open failed. Try again.\n");
                return -1;
            }
        }
    }
    printf("--------Server List---------\n");
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
        printf("Id = %d sockfd = %d %s:%d\n", \
                nptr->id, ((SERVER *)(nptr->info))->sockfd, ((SERVER *)(nptr->info))->addr, ((SERVER *)(nptr->info))->port);
        if (LinklistIteratorAtEnd(&cit))
            break;
        LinklistIteratorToNext(&cit);
    }
    printf("----------------------------\n");
    return 0;

}

/*
 * Exec command
 */
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
            // update server list
            if (update_conhash() < 0) {
                printf("Can't exec command\n");
                return;
            }
            sscanf(command, "open %s", DBName);
            CLinklist_Iterator cit;
            LinklistIteratorSetBegin(conhash, &cit);
            // open at every server
            for(i = 0; i < ConhashGetSize(conhash); ++i) {
                Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
                int sockfd = ((SERVER *)(nptr->info))->sockfd;
                CreateMsg1(buffer, &send_size, OPEN, DBName, strlen(DBName));
                SendMsg(sockfd, buffer, send_size);
                back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
                AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
                if (back_code == ERROR) {
                    printf("Open failed. Try again.\n");
                    return;
                }
                LinklistIteratorToNext(&cit);
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
        // update server list
        if (update_conhash() < 0) {
            printf("Can't exec command\n");
            return;
        }
        // find which server to use
        Node *nptr = ConhashGetNode(conhash, key);
        int sockfd = ((SERVER *)(nptr->info))->sockfd;
        // put to right server
        SendMsg(sockfd, buffer, send_size);
        // put to the next one server, too
        nptr = ConhashGetNodeAfter(conhash, key, 1);
        if (nptr != NULL) {
            int nextsockfd = ((SERVER *)(nptr->info))->sockfd;
            SendMsg(nextsockfd, buffer, send_size);
            back_size = RecvMsg(nextsockfd, buffer, BUFFER_SIZE);
        }
        // receive msg from right server
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
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
        // update server list
        if (update_conhash() < 0) {
            printf("Can't exec command\n");
            return;
        }
        // find which server to use
        Node *nptr = ConhashGetNode(conhash, key);
        int sockfd = ((SERVER *)(nptr->info))->sockfd;
        // request
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
    // DELETE
    else if ( CommandMatching(command, "delete( +|\\t)[0-9]+$") == 0 ) {
        int key = -1;
        sscanf(command, "delete %d", &key);
        CreateMsg1(buffer, &send_size, DELETE, &key, sizeof(int));
        // update server list
        if (update_conhash() < 0) {
            printf("Can't exec command\n");
            return;
        }
        // find which server to use
        Node *nptr = ConhashGetNode(conhash, key);
        int sockfd = ((SERVER *)(nptr->info))->sockfd;
        // delete
        SendMsg(sockfd, buffer, send_size);
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_code == DELETE_OK) {
            printf("Delete record key = %d successfully.\n", key);
        }
        else if (back_code == ERROR) {
            printf("Delete failed.\n");
        }
        // delete from next server, too
        nptr = ConhashGetNodeAfter(conhash, key, 1);
        if (nptr != NULL) {
            sockfd = ((SERVER *)(nptr->info))->sockfd;
            SendMsg(sockfd, buffer, send_size);
            back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        }
    }
    // CLOSE
    else if ( CommandMatching(command, "close") == 0 ) {
        // update server list
        if (update_conhash() < 0) {
            printf("Can't exec command\n");
            return;
        }
        CLinklist_Iterator cit;
        LinklistIteratorSetBegin(conhash, &cit);
        // close at every server
        for(i = 0; i < ConhashGetSize(conhash); ++i) {
            Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
            int sockfd = ((SERVER *)(nptr->info))->sockfd;
            CreateMsg0(buffer, &send_size, CLOSE);
            SendMsg(sockfd, buffer, send_size);
            back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
            AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
            LinklistIteratorToNext(&cit);
        }
        DBName[0] = '\0';
        printf("Close successfully.\n");
    }
    // EXIT
    else if ( CommandMatching(command, "exit") == 0 ) {
        // update server list
        if (update_conhash() < 0) {
            printf("Can't exec command\n");
            return;
        }
        CLinklist_Iterator cit;
        LinklistIteratorSetBegin(conhash, &cit);
        // exit at every server
        for(i = 0; i < ConhashGetSize(conhash); ++i) {
            Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
            int sockfd = ((SERVER *)(nptr->info))->sockfd;
            CreateMsg0(buffer, &send_size, EXIT);
            SendMsg(sockfd, buffer, send_size);
            CloseSocket(sockfd);
            LinklistIteratorToNext(&cit);
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

    // welcome message
    char buf[BUFFER_SIZE];
    int len = RecvMsg(master_sockfd, &buf, BUFFER_SIZE);
    memset(buf, 0, BUFFER_SIZE);
    
    // request server list
    int servers_cnt;
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

    // create consistent hash table
    conhash = ConhashCreate();
    int i;
    for(i = 0; i < servers_cnt; ++i) {
        SERVER serv;
        memcpy(serv.addr, data2 + i * sizeof(SERVER_INFO), ADDR_LEN);
        memcpy(&(serv.port), data2 + i * sizeof(SERVER_INFO) + ADDR_LEN, sizeof(int));
        // init socket
        serv.sockfd = InitializeClient(serv.addr, serv.port);
        // receive welcome message from the server
        char buffer[BUFFER_SIZE];
        int len = RecvMsg(serv.sockfd, &buffer, BUFFER_SIZE);
        if  (len < 0)
            printf("Server (%s:%d) connect error.\n", serv.addr, serv.port);
        // add node to consistent hash table
        if (ConhashAddNode(conhash, &serv, sizeof(SERVER)) < 0)
            printf("Add node to consistent hash error.\n");
    }
    
    printf("--------Server List---------\n");
    CLinklist_Iterator cit;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *nptr = (Node *)CirLinklistGetDataPtr(&cit);
        printf("Id = %d sockfd = %d %s:%d\n", \
                nptr->id, ((SERVER *)(nptr->info))->sockfd, ((SERVER *)(nptr->info))->addr, ((SERVER *)(nptr->info))->port);
        if (LinklistIteratorAtEnd(&cit))
            break;
        LinklistIteratorToNext(&cit);
    }
    printf("----------------------------\n");
    printf("*************** Init End ****************\n\n");

    // start to get and exec command
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

