#include "linklist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LINKLIST LinklistCreate(int data_size) {
    // head is like [data_size][node_cnt][first_node_ptr][last_node_ptr]
    LINKLIST linklist = (LINKLIST)malloc(HEAD_SIZE);
    memset(linklist, 0, HEAD_SIZE);
    memcpy(linklist, &data_size, sizeof(int));
    return linklist;
}

NODE_PTR NewNode(int data_size, void *data) {
    // a node is like [prev_node_ptr][data][next_node_ptr]
    NODE_PTR new = (NODE_PTR)malloc(data_size + 2 * sizeof(NODE_PTR));
    memset(new, 0, data_size + 2 * sizeof(NODE_PTR));
    memcpy(new + sizeof(NODE_PTR), data, data_size);
    return new;
}

int LinklistPushBack(LINKLIST linklist, void *data) {
    int data_size;
    int node_cnt;
    NODE_PTR first, last;
    memcpy(&data_size, linklist, sizeof(int));
    memcpy(&node_cnt, linklist + sizeof(int), sizeof(int));
    NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    if (node_cnt == 0) {
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(NODE_PTR), &new, sizeof(NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    else {
        memcpy(&last, linklist + 2 * sizeof(int) + sizeof(NODE_PTR), sizeof(NODE_PTR));
        memcpy(last + sizeof(NODE_PTR) + data_size, &new, sizeof(NODE_PTR));
        memcpy(new, &last, sizeof(NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(NODE_PTR), &new, sizeof(NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int LinklistGetSize(LINKLIST linklist) {
    int size = 0;
    memcpy(&size, linklist + sizeof(int), sizeof(int));
    return size;
}
void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int), sizeof(NODE_PTR));
}

void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int) + sizeof(NODE_PTR), sizeof(NODE_PTR));
}

int LinklistIteratorAtEnd(Linklist_Iterator *it) {
    if (it->ptr == NULL) 
        return 1;
    else
        return 0;
}

int LinklistIteratorAtHead(Linklist_Iterator *it) {
    if (it->ptr == NULL) 
        return 1;
    else
        return 0;
}

void LinklistIteratorToNext(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr) + sizeof(NODE_PTR) + (it->data_size), sizeof(NODE_PTR));
}

void LinklistIteratorToPrev(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr), sizeof(NODE_PTR));
}

void *LinklistGetDataPtr(Linklist_Iterator *it) {
    void *data = (it->ptr) + sizeof(NODE_PTR);
    return data;
}
