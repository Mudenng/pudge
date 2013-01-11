/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  linklist.c                             */
/*  PRINCIPAL AUTHOR      :  Pudge                                  */
/*  SUBSYSTEM NAME        :  base structs                           */
/*  MODULE NAME           :  linklist                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2013/01/05                             */
/*  DESCRIPTION           :  General linklist                       */
/********************************************************************/

#include "linklist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Create new linklist
 */
LINKLIST LinklistCreate(int data_size) {
    // head is like [data_size][node_cnt][first_node_ptr][last_node_ptr]
    LINKLIST linklist = (LINKLIST)malloc(LL_HEAD_SIZE);
    memset(linklist, 0, LL_HEAD_SIZE);
    memcpy(linklist, &data_size, sizeof(int));
    return linklist;
}

/*
 * Create a new node
 */
LL_NODE_PTR NewNode(int data_size, void *data) {
    // a node is like [prev_node_ptr][data][next_node_ptr]
    LL_NODE_PTR new = (LL_NODE_PTR)malloc(data_size + 2 * sizeof(LL_NODE_PTR));
    if (new == NULL)
        return NULL;
    memset(new, 0, data_size + 2 * sizeof(LL_NODE_PTR));
    memcpy(new + sizeof(LL_NODE_PTR), data, data_size);
    return new;
}

/*
 * Push new node to back
 */
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

/*
 * Push new node to head
 */
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

/*
 * Get linklist node count
 */
int LinklistGetSize(LINKLIST linklist) {
    int size = 0;
    memcpy(&size, linklist + sizeof(int), sizeof(int));
    return size;
}

/*
 * Insert node after current
 */
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

/*
 * Insert node before current
 */
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

/*
 * Delete current node
 */
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

/*
 * Set iterator at head
 */
void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int), sizeof(LL_NODE_PTR));
}

/*
 * Set iterator at tail
 */
void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it) {
    it->linklist = linklist;
    memcpy(&(it->data_size), linklist, sizeof(int));
    memcpy(&(it->ptr), linklist + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
}

/*
 * Check if iterator at end
 */
int LinklistIteratorAtEnd(Linklist_Iterator *it) {
    LL_NODE_PTR last;
    memcpy(&last, (it->linklist) + 2 * sizeof(int) + sizeof(LL_NODE_PTR), sizeof(LL_NODE_PTR));
    if (memcmp(&(it->ptr), &last, sizeof(LL_NODE_PTR)) == 0) 
        return 1;
    else
        return 0;
}

/*
 * Check if iterator at head
 */
int LinklistIteratorAtHead(Linklist_Iterator *it) {
    LL_NODE_PTR first;
    memcpy(&first, (it->linklist) + 2 * sizeof(int), sizeof(LL_NODE_PTR));
    if (memcmp(&(it->ptr), &first, sizeof(LL_NODE_PTR)) == 0) 
        return 1;
    else
        return 0;
}

/*
 * Move iterator to next
 */
void LinklistIteratorToNext(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr) + sizeof(LL_NODE_PTR) + (it->data_size), sizeof(LL_NODE_PTR));
}

/*
 * Move iterator to prev
 */
void LinklistIteratorToPrev(Linklist_Iterator *it) {
    memcpy(&(it->ptr), (it->ptr), sizeof(LL_NODE_PTR));
}

/*
 * Get node data pointer
 */
void *LinklistGetDataPtr(Linklist_Iterator *it) {
    void *data = (it->ptr) + sizeof(LL_NODE_PTR);
    return data;
}

// Circular link list

/*
 * Push new node back
 */
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

/*
 * Push new node ahead
 */
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

/*
 * Insert node after current
 */
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

/*
 * Insert node before current
 */
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

/*
 * Delete current node
 */
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
