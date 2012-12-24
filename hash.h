#ifndef PUDGE_HASH_H
#define PUDGE_HASH_H

#include <limits.h>

#define HEAD_SIZE (sizeof(int) * 3 + sizeof(HASH_FUN))
#define BITS_IN_int     ( sizeof(int) * CHAR_BIT )
#define THREE_QUARTERS	((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH		((int) (BITS_IN_int / 8))
#define HIGH_BITS		( ~((unsigned int)(~0) >> ONE_EIGHTH ))

typedef char* HASHTABLE;
typedef char* NODE_PTR;
typedef unsigned int (*HASH_FUN)(const char *key);

HASHTABLE HashCreateTable(int length, int key_size, int val_size, HASH_FUN hashfun);

#define HashCreateTablePJW(length, key_size, val_size) HashCreateTable(length, key_size, val_size, HashPJW)

int HashDestroyTable(HASHTABLE table);

int HashAddNode_tail(HASHTABLE table, void *key, void *val); 

int HashAddNode(HASHTABLE table, void *key, void *val);

int HashGetValue(HASHTABLE table, void *key, void *buf);

int HashDelete(HASHTABLE table, void *key);

unsigned int HashPJW (const char *datum);	

#endif
