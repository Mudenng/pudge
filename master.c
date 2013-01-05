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

LINKLIST server_list;

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
        printf("***New Server connect\n");
        LinklistPushBack(server_list, data1);
        CreateMsg0(buffer, &back_size, MASTER_OK);
        SendMsg(sockfd, buffer, back_size);
        Linklist_Iterator it;
        LinklistIteratorSetBegin(server_list, &it);
        printf("Servers now online:\n");
        for( ; !LinklistIteratorAtEnd(&it); LinklistIteratorToNext(&it)) {
            SERVER_INFO *servinfo = LinklistGetDataPtr(&it);
            printf("  - %s:%d -\n", servinfo->addr, servinfo->port);
        }
    }
    else if (cmd_code == GET_SERVER_LIST) {
        printf("***Client request server list\n");
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
    pthread_t main_thread;
    SERVER_START_ARG sarg;
    sarg.ip = ip;
    sarg.port = PORT;
    sarg.max_conn = MAX_CONNECTS;
    sarg.recv_callback_fn = recv_callback_fn;
    pthread_create(&main_thread, NULL, (void *)StartServer, &sarg);
    printf("Master start at %s:%d\n", ip, PORT);

    // close master
    int master_sockfd;
    pthread_join(main_thread, (void *)&master_sockfd);
    CloseSocket(master_sockfd);
    printf("Master stoped.\n");

}
