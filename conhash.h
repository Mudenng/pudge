#ifndef ADVANCED_STRUCTION_CONSISTENT_HASH_H 
#define ADVANCED_STRUCTION_CONSISTENT_HASH_H 

#include "linklist.h"
#include <limits.h>

#define BITS_IN_int     ( sizeof(int) * CHAR_BIT )
#define THREE_QUARTERS	((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH		((int) (BITS_IN_int / 8))
#define HIGH_BITS		( ~((unsigned int)(~0) >> ONE_EIGHTH ))

#define HASH_MAX 4294967296
#define HASH_INCREMENT 65536
#define MAX_INFO_SIZE 50 

typedef CLINKLIST CONHASH;

typedef struct {
    int id;
    char info[MAX_INFO_SIZE];
}Node;

CONHASH ConhashCreate();

int ConhashAddNode(CONHASH conhash, void *info, int info_size);

int ConhashRemoveNode(CONHASH conhash, int id);

Node *ConhashGetNode(CONHASH conhash, int key);

Node *ConhashGetNodeAfter(CONHASH conhash, int key, int n);

Node *ConhashGetNodeBefore(CONHASH conhash, int key, int n);

int ConhashGetSize(CONHASH conhash);

int ConhashGetSpaceSize(CONHASH conhash);

#endif
