#ifndef BASE_STRUCTION_LINKLIST_H
#define BASE_STRUCTION_LINKLIST_H

#define HEAD_SIZE (sizeof(NODE_PTR) * 2 + sizeof(int) * 2)

typedef char* LINKLIST;
typedef char* NODE_PTR;

typedef struct {
    LINKLIST linklist;
    NODE_PTR ptr;
    int data_size;
}Linklist_Iterator;

LINKLIST LinklistCreate(int data_size);

int LinklistPushBack(LINKLIST linklist, void *data);

int LinklistGetSize(LINKLIST linklist);

void LinklistIteratorSetBegin(LINKLIST linklist, Linklist_Iterator *it);

void LinklistIteratorSetTail(LINKLIST linklist, Linklist_Iterator *it);

int LinklistIteratorAtEnd(Linklist_Iterator *it);

int LinklistIteratorAtHead(Linklist_Iterator *it);

void LinklistIteratorToNext(Linklist_Iterator *it);

void LinklistIteratorToPrev(Linklist_Iterator *it);

void *LinklistGetDataPtr(Linklist_Iterator *it);

#endif
