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
#include <semaphore.h>


#define ADDR "127.0.0.1"
#define PORT 9999
#define MAX_CONNECTS 100
#define BUFFER_SIZE 1000

#define DBNAME_SIZE 20

#define THREADS 3

#define RANDOM(x)                   \
        rand_id = random()%x;


HASHTABLE db_handle_table;
HASHTABLE db_name_table;

HASHTABLE thread_msg_table;
int thread_signal[THREADS];

typedef struct {
    DBHANDLE hdb;
    int links;
}DB_HANDLE_NODE;

typedef struct {
    int sockfd;
    int cmd_code;
    char *data1;
    char *data2;
    int size1;
    int size2;
}MESSAGE;

typedef struct {
    int tid;
}THREAD_ARG;

// sem_t sem[THREADS];

void RequestConduct(void *arg) {
    int client_sockfd = ((MESSAGE *)arg)->sockfd;
    int cmd_code = ((MESSAGE *)arg)->cmd_code;
    char *data1 = ((MESSAGE *)arg)->data1;
    char *data2 = ((MESSAGE *)arg)->data2;
    int size1 = ((MESSAGE *)arg)->size1;
    int size2 = ((MESSAGE *)arg)->size2;
    char buffer2[BUFFER_SIZE];
    free(arg);
    // memcpy(buffer2, data2, size2);
    // buffer2[size2] = '\0';
    // printf("arg->data2 : %s\n", buffer2);
    DBHANDLE db = NULL;
    char *dbname = (char *)malloc(DBNAME_SIZE);
    DB_HANDLE_NODE dhn;
    char buffer[BUFFER_SIZE];
    int back_size;
    switch(cmd_code) {
        case OPEN:
            data1[size1] = '\0';
            memset(dbname, 0, DBNAME_SIZE);
            memcpy(dbname, data1, size1);
            HashAddNode(db_name_table, &client_sockfd, dbname);
            if (HashGetValue(db_handle_table, dbname, &dhn) == 0) {
                db = OpenHDB(data1);
                if (db != NULL) {
                    dhn.hdb = db;
                    dhn.links = 1;
                    HashAddNode(db_handle_table, dbname, &dhn);
                    CreateMsg0(buffer, &back_size, OPEN_OK);
                }
                else {
                    CreateMsg0(buffer, &back_size, ERROR);
                }
            }
            else {
                db = dhn.hdb;
                dhn.links++;
                HashDelete(db_handle_table, dbname);
                HashAddNode(db_handle_table, dbname, &dhn);
                CreateMsg0(buffer, &back_size, OPEN_OK);
            }
            SendMsg(client_sockfd, buffer, back_size);
            printf("handled 'OPEN %s' from %d\n", data1, client_sockfd);
            free(dbname);
            break;
        case CLOSE:
            HashGetValue(db_name_table, &client_sockfd, dbname);
            HashDelete(db_name_table, &client_sockfd);
            HashGetValue(db_handle_table, dbname, &dhn);
            dhn.links--;
            if (dhn.links <= 0) {
                HashGetValue(db_handle_table, dbname, &db);
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
            free(dbname);
            break;
        case EXIT:
            if ( HashGetValue(db_name_table, &client_sockfd, dbname) != 0) {
                HashDelete(db_name_table, &client_sockfd);
                HashGetValue(db_handle_table, dbname, &dhn);
                dhn.links--;
                if (dhn.links <= 0) {
                    HashGetValue(db_handle_table, dbname, &db);
                    CloseHDB(db);
                    HashDelete(db_handle_table, dbname);
                }
                else {
                    HashDelete(db_handle_table, dbname);
                    HashAddNode(db_handle_table, dbname, &dhn);
                }
            }
            db = NULL;
            CloseSocket(client_sockfd);
            printf("handled 'EXIT' from %d\n", client_sockfd);
            printf("client %d disconnected.\n", client_sockfd);
            free(dbname);
            break;
        case PUT:
            HashGetValue(db_name_table, &client_sockfd, dbname);
            HashGetValue(db_handle_table, dbname, &db);
            if (db != NULL) {
                int key = (int)*data1;
                value_struct value;
                data2[size2] = '\0';
                printf("put %s\n", data2);
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
            free(dbname);
            break;
        case GET:
            HashGetValue(db_name_table, &client_sockfd, dbname);
            HashGetValue(db_handle_table, dbname, &db);
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
            free(dbname);
            break;
        case DELETE:
            HashGetValue(db_name_table, &client_sockfd, dbname);
            HashGetValue(db_handle_table, dbname, &db);
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
            free(dbname);
            break;
    }
}

void Thread_DB(void *arg) {
    pthread_t pt = pthread_self();
    int tid = ((THREAD_ARG *)arg)->tid;
    free(arg);
    printf("Thread %d ready\n", tid);
    while(1) {
        sem_t *sem;
        char sem_name[10];
        sprintf(sem_name, "%d", tid);
        sem = sem_open(sem_name, 0);
        sem_wait(sem);
        // sem_wait(&sem[tid]);
        printf("Thread %d wake up\n", tid);
        MESSAGE *msg;
        while(HashGetValue(thread_msg_table, &tid, &msg) != 0) {
            HashDelete(thread_msg_table, &tid);
            RequestConduct(msg);
            printf("Thread %d handled.\n", tid);
        }
    }
}

void Thread_ServerConsole(void *arg) {
    
}

void recv_callback_fn(int sockfd, short event, void *arg) {
    printf("Receive client request\n");
    char buffer[BUFFER_SIZE];
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int cmd_code;
    int size1;
    int size2;
    int len = RecvMsg(sockfd, &buffer, BUFFER_SIZE);
    if (len == -1)
        return;
    AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
    MESSAGE *msg = (MESSAGE *)malloc(sizeof(MESSAGE));
    msg->sockfd = sockfd;
    msg->cmd_code = cmd_code;
    msg->data1 = data1;
    msg->data2 = data2;
    msg->size1 = size1;
    msg->size2 = size2;
    int rand_id;
    RANDOM(THREADS);
    HashAddNode_tail(thread_msg_table, &rand_id, &msg);
    sem_t *sem;
    char sem_name[10];
    sprintf(sem_name, "%d", rand_id);
    sem = sem_open(sem_name, 0);
    sem_post(sem);
    // sem_post(&sem[rand_id]);
    printf("Start handle DB request\n");
}

unsigned int my_hash(const char *key) {
    return *(int *)key;
}

int main() {
    // start server main thread
    pthread_t socket_thread;
    SERVER_START_ARG sarg;
    sarg.ip = ADDR;
    sarg.port = PORT;
    sarg.max_conn = MAX_CONNECTS;
    sarg.recv_callback_fn = recv_callback_fn;
    pthread_create(&socket_thread, NULL, (void *)StartServer, &sarg);

    // init some structs
    db_handle_table = HashCreateTablePJW(5, DBNAME_SIZE, sizeof(DB_HANDLE_NODE));
    thread_msg_table = HashCreateTable(THREADS, sizeof(int), sizeof(MESSAGE *), my_hash);    
    db_name_table = HashCreateTablePJW(5, sizeof(int), DBNAME_SIZE);

    // init signal
    int i;
    for(i = 0; i < THREADS; ++i) {
        sem_t *sem;
        char sem_name[10];
        sprintf(sem_name, "%d", i);
        sem = sem_open(sem_name, O_RDWR | O_CREAT, S_IRUSR|S_IWUSR, 0);
        // sem_init(&sem[i], 0, 0);
    }

    // init threads
    pthread_t tid[THREADS];
    for(i = 0; i < THREADS; ++i) {
        THREAD_ARG *argv = (THREAD_ARG *)malloc(sizeof(THREAD_ARG));
        argv->tid = i;
        pthread_create(&tid[i], NULL, (void *)Thread_DB, argv);
        printf("thread %d started\n", i);
    }

    int server_sockfd;
    pthread_join(socket_thread, (void *)&server_sockfd);
    CloseSocket(server_sockfd);
    return 0;
}

