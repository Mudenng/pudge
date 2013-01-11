/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  linklist.h                             */
/*  PRINCIPAL AUTHOR      :  Pudge                                  */
/*  SUBSYSTEM NAME        :  base structs                           */
/*  MODULE NAME           :  linklist                               */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2013/01/05                             */
/*  DESCRIPTION           :  Interface to a general linklist        */
/********************************************************************/

#ifndef BASE_STRUCTION_LINKLIST_H
#define BASE_STRUCTION_LINKLIST_H

#define LL_HEAD_SIZE (sizeof(LL_NODE_PTR) * 2 + sizeof(int) * 2)

typedef char* LINKLIST;
typedef char* LL_NODE_PTR;

typedef struct {
    LL_NODE_PTR ptr;
    LINKLIST linklist;
    int data_size;
}Linklist_Iterator;

/*
 * Create a linklist
 * input    : node size
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return linklist handle
 *          : if FAILURE return NULL
 */
LINKLIST LinklistCreate(int data_size);

/*
 * Push back a new node 
 * input    : linklist handle, node data
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int LinklistPushBack(LINKLIST linklist, void *data);

/*
 * Push ahead a new node
 * input    : linklist handle, node data
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int LinklistPushHead(LINKLIST linklist, void *data);

/*
 * Get the count of nodes
 * input    : linklist handle
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return count
 *          : if FAILURE return 0
 */
int LinklistGetSize(LINKLIST linklist);

/*
 * Insert a node after current node
 * input    : linklist iterator, node data
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int LinklistInsertAfter(Linklist_Iterator *it, void *data);

/*
 * Insert a node before current node
 * input    : linklist iterator, node data
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int LinklistInsertBefore(Linklist_Iterator *it, void *data);

/*
 * Delete current node
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int LinklistDelete(Linklist_Iterator *it);

/*
 * Return current node's data pointer
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return data pointer
 *          : if FAILURE return NULL
 */
void *LinklistGetDataPtr(Linklist_Iterator *it);

/*
 * Set linklist iterator at head
 * input    : linklist handle, linklist iterator
 * output   : None
 * in/out   : None
 * return   : None
 */
void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it);

/*
 * Set linklist iterator at tail
 * input    : linklist handle, linklist iterator
 * output   : None
 * in/out   : None
 * return   : None
 */
void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it);

/*
 * Check if the iterator is at tail
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : if YES 1
 *          : if NO  0
 */
int LinklistIteratorAtEnd(Linklist_Iterator *it);

/*
 * Check if the iterator is at head
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : if YES 1
 *          : if NO  0
 */
int LinklistIteratorAtHead(Linklist_Iterator *it);

/*
 * Set iterator to next
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : None
 */
void LinklistIteratorToNext(Linklist_Iterator *it);

/*
 * Set iterator to prev
 * input    : linklist iterator
 * output   : None
 * in/out   : None
 * return   : None
 */
void LinklistIteratorToPrev(Linklist_Iterator *it);


/*          Circular link list          */
/* Functions has same usage as linklist */
typedef LINKLIST CLINKLIST;
typedef Linklist_Iterator CLinklist_Iterator;

#define CirLinklistCreate(data_size) LinklistCreate(data_size)

#define CirLinklistGetSize(linklist) LinklistGetSize(linklist)

int CirLinklistPushBack(CLINKLIST linklist, void *data);

int CirLinklistPushHack(CLINKLIST linklist, void *data);

int CirLinklistInsertAfter(CLinklist_Iterator *it, void *data);

int CirLinklistInsertBefore(Linklist_Iterator *it, void *data); 

int CirLinklistDelete(Linklist_Iterator *it);

#define CirLinklistGetDataPtr(cit) LinklistGetDataPtr(cit)

#endif
