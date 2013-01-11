#include "network.h"
#include "protocol.h"
#include "linklist.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 8888
#define MAX_CONNECTS 100
#define BUFFER_SIZE 1000
#define MAX_SERVER_THREAD 100

typedef struct {
    int sockfd;
    SERVER_INFO info;
}SERVER_THREAD_ARG;

// server list
LINKLIST server_list;

// sockfd list
LINKLIST sockfd_list;

// one thread per server
pthread_t server_thread[MAX_SERVER_THREAD];
int usable_tid = 0;

void push_list() {
    // create data
    int back_size;
    int list_size = LinklistGetSize(server_list);
    int total_size = list_size * (sizeof(SERVER_INFO));
    char *data = (char *)malloc(total_size);
    Linklist_Iterator it;
    LinklistIteratorSetBegin(server_list, &it);
    int i;
    for(i = 0; i < list_size; ++i) {
        memcpy(data + sizeof(SERVER_INFO) * i, ((SERVER_INFO *)LinklistGetDataPtr(&it))->addr, ADDR_LEN);
        memcpy(data + sizeof(SERVER_INFO) * i + ADDR_LEN, &(((SERVER_INFO *)LinklistGetDataPtr(&it))->port), sizeof(int));
        LinklistIteratorToNext(&it);
    }
    char *buf = (char *)malloc(total_size + sizeof(int) * 2);
    CreateMsg2(buf, &back_size, UPDATE_SERVER_LIST, &list_size, sizeof(int), data, total_size);
    LinklistIteratorSetBegin(sockfd_list, &it);
    // send data to every server
    while(1) {
        int fd = *(int *)LinklistGetDataPtr(&it);
        SendMsg(fd, buf, back_size);
        if (LinklistIteratorAtEnd(&it))
            break;
        LinklistIteratorToNext(&it);
    }
    free(data);
    free(buf); 
}

void Thread_Server(void *arg) {
    int sockfd = ((SERVER_THREAD_ARG *)arg)->sockfd;
    printf("***New Server connect\n");
    // add new server
    if (LinklistPushBack(server_list, &(((SERVER_THREAD_ARG *)arg)->info)) < 0)
        printf("Add node to list error.\n");
    char buffer[BUFFER_SIZE];
    int send_size, back_size;
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int cmd_code;
    int size1;
    int size2;
    CreateMsg0(buffer, &back_size, MASTER_OK);
    SendMsg(sockfd, buffer, back_size);
    Linklist_Iterator it;
    LinklistIteratorSetBegin(server_list, &it);
    printf("Servers now online:\n");
    while(1) {
        SERVER_INFO *servinfo = LinklistGetDataPtr(&it);
        printf("  - %s:%d -\n", servinfo->addr, servinfo->port);
        if (LinklistIteratorAtEnd(&it))
            break;
        LinklistIteratorToNext(&it);
    }
    // push server list
    printf("Push new server list to all servers.\n");     
    push_list();

    // heart beat
    struct timeval timeout = {3,0};
    setsockopt(sockfd, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    while(1) {
        sleep(2);
        CreateMsg0(buffer, &send_size, HEART_BEAT);
        if (SendMsg(sockfd, buffer, send_size) < send_size) {
            break;
        }
        int back_code;
        back_size = RecvMsg(sockfd, buffer, BUFFER_SIZE);
        AnalyseMsg(buffer, &back_code, data1, &size1, data2, &size2);
        if (back_size <= 0 || back_code != HEART_BEAT_OK) {
            break;
        }
    }
    printf("!!!One server (%s:%d) down.\n", ((SERVER_THREAD_ARG *)arg)->info.addr, ((SERVER_THREAD_ARG *)arg)->info.port);

    // delete server from list
    int i;
    LinklistIteratorSetBegin(server_list, &it);
    for(i = 0; i < LinklistGetSize(server_list); ++i) {
        SERVER_INFO *servinfo = LinklistGetDataPtr(&it);
        if (memcmp(servinfo->addr, ((SERVER_THREAD_ARG *)arg)->info.addr, ADDR_LEN) == 0 \
                && servinfo->port == ((SERVER_THREAD_ARG *)arg)->info.port) {
            LinklistDelete(&it);
            break;
        }
        LinklistIteratorToNext(&it);
    }
    // delete socketfd frome list
    LinklistIteratorSetBegin(sockfd_list, &it);
    for(i = 0; i < LinklistGetSize(sockfd_list); ++i) {
        int fd = *(int *)LinklistGetDataPtr(&it);
        if (fd == sockfd) {
            LinklistDelete(&it);
            break;
        }
        LinklistIteratorToNext(&it);
    }

    // show again
    LinklistIteratorSetBegin(server_list, &it);
    printf("Servers now online:\n");
    for(i = 0; i < LinklistGetSize(server_list); ++i) {
        SERVER_INFO *servinfo = LinklistGetDataPtr(&it);
        printf("  - %s:%d -\n", servinfo->addr, servinfo->port);
        LinklistIteratorToNext(&it);
    }

    // push server list
    if (LinklistGetSize(sockfd_list) > 0) {
        printf("Push new server list to all servers.\n");     
        push_list();
    }

    free(arg);
    CloseSocket(sockfd);
}


void recv_callback_fn(int sockfd, short event, void *arg) {
    char buffer[BUFFER_SIZE];
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int cmd_code;
    int size1;
    int size2;
    int back_size;
    int len = RecvMsg(sockfd, &buffer, BUFFER_SIZE);
    if (len == -1)
        return;
    AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
    if (cmd_code == NEW_SERVER) {
        SERVER_THREAD_ARG *thread_arg = (SERVER_THREAD_ARG *)malloc(sizeof(SERVER_THREAD_ARG));
        thread_arg->sockfd = sockfd;
        LinklistPushBack(sockfd_list, &sockfd);
        memcpy(&(thread_arg->info), data1, size1);
        if (usable_tid >= MAX_SERVER_THREAD) {
            printf("Server num is over upper limit\n");
            return;
        }
        // start a new server thread
        pthread_create(&server_thread[usable_tid], NULL, (void *)Thread_Server, thread_arg);
        ++usable_tid;
    }
    else if (cmd_code == GET_SERVER_LIST) {
        printf("***Client request\n");
        int list_size = LinklistGetSize(server_list);
        int total_size = list_size * (sizeof(SERVER_INFO));
        char *data = (char *)malloc(total_size);
        Linklist_Iterator it;
        LinklistIteratorSetBegin(server_list, &it);
        int i;
        for(i = 0; i < list_size; ++i) {
            memcpy(data + sizeof(SERVER_INFO) * i, ((SERVER_INFO *)LinklistGetDataPtr(&it))->addr, ADDR_LEN);
            memcpy(data + sizeof(SERVER_INFO) * i + ADDR_LEN, &(((SERVER_INFO *)LinklistGetDataPtr(&it))->port), sizeof(int));
            LinklistIteratorToNext(&it);
        }
        char *buf = (char *)malloc(total_size + sizeof(int) * 2);
        CreateMsg2(buf, &back_size, SERVER_LIST_OK, &list_size, sizeof(int), data, total_size);
        SendMsg(sockfd, buf, back_size);
        free(data);
        free(buf);      
        CloseSocket(sockfd);
    }
}

int main() {
    // get ip
    char interface_name[20];
    char ip[INET_ADDRSTRLEN];
    while(1) {
        printf("Interface name: ");
        scanf("%s", interface_name);
        if (get_local_ip(interface_name, ip) == -1) {
            printf("Get IP error, check Interface name.\n\n");
            continue;
        }
        break;
    }

    // start master main thread
    server_list = LinklistCreate(sizeof(SERVER_INFO));
    sockfd_list = LinklistCreate(sizeof(int));
    pthread_t main_thread;
    SERVER_START_ARG sarg;
    sarg.ip = ip;
    sarg.port = PORT;
    sarg.max_conn = MAX_CONNECTS;
    sarg.recv_callback_fn = recv_callback_fn;
    pthread_create(&main_thread, NULL, (void *)StartMaster, &sarg);
    printf("Master start at %s:%d\n", ip, PORT);

    // close master
    int master_sockfd;
    pthread_join(main_thread, (void *)&master_sockfd);
    CloseSocket(master_sockfd);
    printf("Master stoped.\n");

}
