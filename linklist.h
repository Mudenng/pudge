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

LINKLIST LinklistCreate(int data_size);

int LinklistPushBack(LINKLIST linklist, void *data);

int LinklistPushHead(LINKLIST linklist, void *data);

int LinklistGetSize(LINKLIST linklist);

int LinklistInsertAfter(Linklist_Iterator *it, void *data);

int LinklistInsertBefore(Linklist_Iterator *it, void *data);

int LinklistDelete(Linklist_Iterator *it);

void *LinklistGetDataPtr(Linklist_Iterator *it);

void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it);

void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it);

int LinklistIteratorAtEnd(Linklist_Iterator *it);

int LinklistIteratorAtHead(Linklist_Iterator *it);

void LinklistIteratorToNext(Linklist_Iterator *it);

void LinklistIteratorToPrev(Linklist_Iterator *it);


// Circular link list
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
