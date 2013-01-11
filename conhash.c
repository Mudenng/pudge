#include "conhash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned int HashPJW (const char *datum, int size) {
    unsigned int hash_value, i;

    hash_value = 0;
    int j;
	for (j = 0; j < size; ++j)
	{
		hash_value = ( hash_value << ONE_EIGHTH ) + *datum;	
        if (( i = hash_value & HIGH_BITS ) != 0 )
			hash_value = ( hash_value ^ ( i >> THREE_QUARTERS )) &~HIGH_BITS;
        ++datum;
	}

	return ( hash_value );
}

CONHASH ConhashCreate() {
    CLINKLIST clist = CirLinklistCreate(sizeof(Node));
    return clist; 
}

int ConhashAddNode(CONHASH conhash, void *info, int info_size) {
    Node new_node;
    memcpy(new_node.info, info, info_size);
    new_node.info[info_size] = '\0';
    // get an unique id
    int id = HashPJW(new_node.info, info_size - sizeof(int)) % HASH_MAX;
    CLinklist_Iterator cit;
    LinklistIteratorSetBegin(conhash, &cit);
    int flag = CirLinklistGetSize(conhash) ? 1 : 0;
    while(flag) {
        while(1) {
            Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
            if (id == temp->id) {
                flag = 1;
                id = (id + HASH_INCREMENT) % HASH_MAX; 
            }
            if (LinklistIteratorAtEnd(&cit)) {
                flag = 0;
                break;
            }
            LinklistIteratorToNext(&cit);
        }
    }
    new_node.id = id;
    // insert to right position
    if (CirLinklistGetSize(conhash) == 0) {
        if (CirLinklistPushBack(conhash, &new_node) == 0)
            return new_node.id;
        else
            return -1;
    }
    else {
        LinklistIteratorSetBegin(conhash, &cit);
        while(1) {
            Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
            // insert before the first bigger one
            if (new_node.id < temp->id) {
                if (CirLinklistInsertBefore(&cit, &new_node) == 0)
                    return new_node.id;
                else 
                    return -1;
            }
            // no bigger one, insert to tail
            else if (LinklistIteratorAtEnd(&cit)) {
                if (CirLinklistPushBack(conhash, &new_node) == 0)
                    return new_node.id;
                else
                    return -1;
            }
            else {
                LinklistIteratorToNext(&cit);
            }
        }
    }
}

int ConhashRemoveNode(CONHASH conhash, int id) {
    if (CirLinklistGetSize(conhash) == 0)
        return -1;
    CLinklist_Iterator cit;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
        if (id == temp->id) {
            CirLinklistDelete(&cit);
            return 0;
        }
        else if (LinklistIteratorAtEnd(&cit)) {
            return -1;
        }
        else {
            LinklistIteratorToNext(&cit);
        }
    }
}

Node *ConhashGetNode(CONHASH conhash, int key) {
    if (CirLinklistGetSize(conhash) == 0)
        return NULL;
    CLinklist_Iterator cit;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
        if (key <= temp->id) {
            return temp;
        }
        // if key > last node id, return first node
        else if (LinklistIteratorAtEnd(&cit)) {
            LinklistIteratorToNext(&cit);
            temp = (Node *)CirLinklistGetDataPtr(&cit);
            return temp;
        }
        else {
            LinklistIteratorToNext(&cit);
        }
    }
}

Node *ConhashGetNodeAfter(CONHASH conhash, int key, int n) {
    if (CirLinklistGetSize(conhash) == 0)
        return NULL;
    CLinklist_Iterator cit;
    // find right node
    int right_id = 0;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
        if (key <= temp->id) {
            right_id = temp->id;
            break;
        }
        // if key > last node id, node = first node
        else if (LinklistIteratorAtEnd(&cit)) {
            LinklistIteratorToNext(&cit);
            temp = (Node *)CirLinklistGetDataPtr(&cit);
            right_id = temp->id;
            break;
        }
        else {
            LinklistIteratorToNext(&cit);
        }
    }
    // return next Nth node
    int i;
    for(i = 0; i < n; ++i) {
        LinklistIteratorToNext(&cit);
    }
    Node *nextn = (Node *)CirLinklistGetDataPtr(&cit);
    if (nextn->id != right_id)
        return nextn;
    else
        return NULL;
    
}

Node *ConhashGetNodeBefore(CONHASH conhash, int key, int n) {
    if (CirLinklistGetSize(conhash) == 0)
        return NULL;
    CLinklist_Iterator cit;
    // find right node
    int right_id = 0;
    LinklistIteratorSetBegin(conhash, &cit);
    while(1) {
        Node *temp = (Node *)CirLinklistGetDataPtr(&cit);
        if (key <= temp->id) {
            right_id = temp->id;
            break;
        }
        // if key > last node id, node = first node
        else if (LinklistIteratorAtEnd(&cit)) {
            LinklistIteratorToNext(&cit);
            temp = (Node *)CirLinklistGetDataPtr(&cit);
            right_id = temp->id;
            break;
        }
        else {
            LinklistIteratorToNext(&cit);
        }
    }
    // return next Nth node
    int i;
    for(i = 0; i < n; ++i) {
        LinklistIteratorToPrev(&cit);
    }
    Node *nextn = (Node *)CirLinklistGetDataPtr(&cit);
    if (nextn->id != right_id)
        return nextn;
    else
        return NULL;
    
}

int ConhashGetSize(CONHASH conhash) {
    return CirLinklistGetSize(conhash);
}

int ConhashGetSpaceSize(CONHASH conhash) {
    int cnt = ConhashGetSize(conhash);
    return LL_HEAD_SIZE + cnt * (sizeof(Node) + 2 * sizeof(LL_NODE_PTR));
}
