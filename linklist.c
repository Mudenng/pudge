#include "linklist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LINKLIST LinklistCreate(int data_size) {
    // head is like [data_size][node_cnt][first_node_ptr][last_node_ptr]
    LINKLIST linklist = (LINKLIST)malloc(LL_HEAD_SIZE);
    memset(linklist, 0, LL_HEAD_SIZE);
    memcpy(linklist, &data_size, sizeof(int));
    return linklist;
}

LL_NODE_PTR NewNode(int data_size, void *data) {
    // a node is like [prev_node_ptr][data][next_node_ptr]
    LL_NODE_PTR new = (LL_NODE_PTR)malloc(data_size + 2 * sizeof(LL_NODE_PTR));
    if (new == NULL)
        return NULL;
    memset(new, 0, data_size + 2 * sizeof(LL_NODE_PTR));
    memcpy(new + sizeof(LL_NODE_PTR), data, data_size);
    return new;
}

int LinklistPushBack(LINKLIST linklist, void *data) {
    int data_size;
    int node_cnt;
    LL_NODE_PTR last;
    memcpy(&data_size, linklist, sizeof(int));
    memcpy(&node_cnt, linklist + sizeof(int), sizeof(int));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    if (node_cnt == 0) {
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    else {
        memcpy(&last, linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
        memcpy(last + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        memcpy(new, &last, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int LinklistPushHead(LINKLIST linklist, void *data) {
    int data_size;
    int node_cnt;
    LL_NODE_PTR first;
    memcpy(&data_size, linklist, sizeof(int));
    memcpy(&node_cnt, linklist + sizeof(int), sizeof(int));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    if (node_cnt == 0) {
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    else {
        memcpy(&first, linklist + 2 * sizeof(int), sizeof(LL_NODE_PTR));
        memcpy(first, &new, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &first, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
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

int LinklistInsertAfter(Linklist_Iterator *it, void *data) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    LL_NODE_PTR last;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    memcpy(&last, (it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    // if insert after last one
    if (memcmp(&last, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        LinklistPushBack(it->linklist, data);
    }
    else {
        LL_NODE_PTR oldnext;
        memcpy(&oldnext, (it->ptr) + sizeof(LL_NODE_PTR) + data_size, sizeof(LL_NODE_PTR));
        memcpy(new, &(it->ptr), sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &oldnext, sizeof(LL_NODE_PTR));
        memcpy(oldnext, &new, sizeof(LL_NODE_PTR));
        memcpy((it->ptr) + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int LinklistInsertBefore(Linklist_Iterator *it, void *data) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    LL_NODE_PTR first;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    memcpy(&first, (it->linklist) + 2 * sizeof(int), sizeof(LL_NODE_PTR));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    // if insert before first one
    if (memcmp(&first, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        LinklistPushHead(it->linklist, data);
    }
    else {
        LL_NODE_PTR oldprev;
        memcpy(&oldprev, it->ptr, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &(it->ptr), sizeof(LL_NODE_PTR));
        memcpy(new, &oldprev, sizeof(LL_NODE_PTR));
        memcpy(oldprev + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        memcpy(it->ptr, &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int LinklistDelete(Linklist_Iterator *it) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    LL_NODE_PTR prev, next;
    memcpy(&prev, it->ptr, sizeof(LL_NODE_PTR));
    memcpy(&next, (it->ptr) + sizeof(LL_NODE_PTR) + data_size, sizeof(LL_NODE_PTR));
    // if only one left
    if (node_cnt <= 1) {
        memset((it->linklist) + 2 * sizeof(int), 0, sizeof(LL_NODE_PTR));
        memset((it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), 0, sizeof(LL_NODE_PTR));
    }
    // if delete first one
    else if (prev == NULL) {
        memset(next, 0, sizeof(LL_NODE_PTR));
        memcpy((it->linklist) + 2 * sizeof(int), &next, sizeof(LL_NODE_PTR));
    }
    // if delete last one
    else if (next == NULL) {
        memset(prev + sizeof(LL_NODE_PTR) + data_size, 0, sizeof(LL_NODE_PTR));
        memcpy((it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &prev, sizeof(LL_NODE_PTR));
    }
    else {
        memcpy(prev + sizeof(LL_NODE_PTR) + data_size, &next, sizeof(LL_NODE_PTR));
        memcpy(next, &prev, sizeof(LL_NODE_PTR));
    }
    node_cnt -= 1;
    memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    free(it->ptr);
    it->ptr = next;
    return 0;
}

void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int), sizeof(LL_NODE_PTR));
}

void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
}

int LinklistIteratorAtEnd(Linklist_Iterator *it) {
    LL_NODE_PTR last;
    memcpy(&last, (it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
    if (memcmp(&(it->ptr), &last, sizeof(LL_NODE_PTR)) == 0) 
        return 1;
    else
        return 0;
}

int LinklistIteratorAtHead(Linklist_Iterator *it) {
    LL_NODE_PTR first;
    memcpy(&first, (it->linklist) + 2 * sizeof(int), sizeof(LL_NODE_PTR));
    if (memcmp(&(it->ptr), &first, sizeof(LL_NODE_PTR)) == 0) 
        return 1;
    else
        return 0;
}

void LinklistIteratorToNext(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr) + sizeof(LL_NODE_PTR) + (it->data_size), sizeof(LL_NODE_PTR));
}

void LinklistIteratorToPrev(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr), sizeof(LL_NODE_PTR));
}

void *LinklistGetDataPtr(Linklist_Iterator *it) {
    void *data = (it->ptr) + sizeof(LL_NODE_PTR);
    return data;
}

// Circular link list
int CirLinklistPushBack(CLINKLIST linklist, void *data) {
    int data_size;
    int node_cnt;
    LL_NODE_PTR first, last;
    memcpy(&data_size, linklist, sizeof(int));
    memcpy(&node_cnt, linklist + sizeof(int), sizeof(int));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    if (node_cnt == 0) {
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        // new->next = new, new->prev = new
        memcpy(new, &new, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));

        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    else {
        memcpy(&last, linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
        memcpy(&first, linklist + 2* sizeof(int), sizeof(LL_NODE_PTR));
        memcpy(last + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        memcpy(new, &last, sizeof(LL_NODE_PTR));
        // new->next = first
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &first, sizeof(LL_NODE_PTR));
        // first->prev = new
        memcpy(first, &new, sizeof(LL_NODE_PTR));

        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int CirLinklistPushHead(CLINKLIST linklist, void *data) {
    int data_size;
    int node_cnt;
    LL_NODE_PTR first, last;
    memcpy(&data_size, linklist, sizeof(int));
    memcpy(&node_cnt, linklist + sizeof(int), sizeof(int));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    if (node_cnt == 0) {
        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
        memcpy(linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &new, sizeof(LL_NODE_PTR));
        // new->next = new, new->prev = new
        memcpy(new, &new, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));

        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    else {
        memcpy(&last, linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
        memcpy(&first, linklist + 2 * sizeof(int), sizeof(LL_NODE_PTR));
        memcpy(first, &new, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &first, sizeof(LL_NODE_PTR));
        // new->prev = last
        memcpy(new, &last, sizeof(LL_NODE_PTR));
        // last->next = new
        memcpy(last + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));

        memcpy(linklist + 2 * sizeof(int), &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy(linklist + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int CirLinklistInsertAfter(CLinklist_Iterator *it, void *data) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    LL_NODE_PTR last;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    memcpy(&last, (it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    // if insert after last one
    if (memcmp(&last, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        CirLinklistPushBack(it->linklist, data);
    }
    else {
        LL_NODE_PTR oldnext;
        memcpy(&oldnext, (it->ptr) + sizeof(LL_NODE_PTR) + data_size, sizeof(LL_NODE_PTR));
        memcpy(new, &(it->ptr), sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &oldnext, sizeof(LL_NODE_PTR));
        memcpy(oldnext, &new, sizeof(LL_NODE_PTR));
        memcpy((it->ptr) + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int CirLinklistInsertBefore(Linklist_Iterator *it, void *data) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    LL_NODE_PTR first;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    memcpy(&first, (it->linklist) + 2 * sizeof(int), sizeof(LL_NODE_PTR));
    LL_NODE_PTR new = NewNode(data_size, data);
    if (new == NULL)
        return -1;
    // if insert before first one
    if (memcmp(&first, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        CirLinklistPushHead(it->linklist, data);
    }
    else {
        LL_NODE_PTR oldprev;
        memcpy(&oldprev, it->ptr, sizeof(LL_NODE_PTR));
        memcpy(new + sizeof(LL_NODE_PTR) + data_size, &(it->ptr), sizeof(LL_NODE_PTR));
        memcpy(new, &oldprev, sizeof(LL_NODE_PTR));
        memcpy(oldprev + sizeof(LL_NODE_PTR) + data_size, &new, sizeof(LL_NODE_PTR));
        memcpy(it->ptr, &new, sizeof(LL_NODE_PTR));
        node_cnt += 1;
        memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    }
    return 0;
}

int CirLinklistDelete(Linklist_Iterator *it) {
    if (it->ptr == NULL)
        return -1;
    int data_size;
    int node_cnt;
    LL_NODE_PTR first, last;
    memcpy(&data_size, it->linklist, sizeof(int));
    memcpy(&node_cnt, (it->linklist) + sizeof(int), sizeof(int));
    memcpy(&last, (it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
    memcpy(&first, (it->linklist) + 2 * sizeof(int), sizeof(LL_NODE_PTR));
    LL_NODE_PTR prev, next;
    memcpy(&prev, it->ptr, sizeof(LL_NODE_PTR));
    memcpy(&next, (it->ptr) + sizeof(LL_NODE_PTR) + data_size, sizeof(LL_NODE_PTR));
    // if delete first one
    if (memcmp(&first, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        memcpy((it->linklist) + 2 * sizeof(int), &next, sizeof(LL_NODE_PTR));
    }
    // if delete last one
    else if (memcmp(&last, &(it->ptr), sizeof(LL_NODE_PTR)) == 0) {
        memcpy((it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), &prev, sizeof(LL_NODE_PTR));
    }
    memcpy(prev + sizeof(LL_NODE_PTR) + data_size, &next, sizeof(LL_NODE_PTR));
    memcpy(next, &prev, sizeof(LL_NODE_PTR));
    node_cnt -= 1;
    memcpy((it->linklist) + sizeof(int), &node_cnt, sizeof(int));
    free(it->ptr);
    it->ptr = next;
    return 0;
}
