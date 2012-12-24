#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned int HashPJW (const char *datum) {
    unsigned int hash_value, i;

	for ( hash_value = 0; *datum; ++datum )
	{
		hash_value = ( hash_value << ONE_EIGHTH ) + *datum;	
        if (( i = hash_value & HIGH_BITS ) != 0 )
			hash_value = ( hash_value ^ ( i >> THREE_QUARTERS )) &~HIGH_BITS;
	}

	return ( hash_value );
}

HASHTABLE HashCreateTable(int length, int key_size, int val_size, HASH_FUN hashfun) {
    HASHTABLE newtable= (HASHTABLE)malloc(sizeof(int) * 3 + sizeof(NODE_PTR) * length);
    memcpy(newtable, &length, sizeof(int)); 
    memcpy(newtable + sizeof(int), &key_size, sizeof(int));
    memcpy(newtable + sizeof(int) * 2, &val_size, sizeof(int));
    memcpy(newtable + sizeof(int) * 3, &hashfun, sizeof(HASH_FUN));
    memset(newtable + HEAD_SIZE, 0, sizeof(NODE_PTR) * length);
    return newtable;
}

NODE_PTR GetNodePtr(HASHTABLE table, int index) {
    int length = *(int *)table;
    NODE_PTR ptr;
    if (index < 0 || index >= length) {
        printf("HashTable index error.\n");
        return NULL;
    }
    memcpy(&ptr, table + HEAD_SIZE + sizeof(NODE_PTR) * index, sizeof(NODE_PTR));
    return ptr;
}

NODE_PTR GetNextPtr(NODE_PTR curr) {
    NODE_PTR next;
    memcpy(&next, curr, sizeof(NODE_PTR));
    return next;
}

int HashAddNode(HASHTABLE table, void *key, void *val) {
    int length = *(int *)(table);
    int key_size = *(int *)(table + sizeof(int));
    int val_size = *(int *)(table + sizeof(int) * 2);
    HASH_FUN hashfun;
    memcpy(&hashfun, table + sizeof(int) * 3, sizeof(HASH_FUN));
    char *tempkey = (char *)malloc(key_size + 1);
    memcpy(tempkey, key, key_size);
    tempkey[key_size] = '\0';
    int index = hashfun(tempkey) % length;
    // printf("index = %d\n", index);
    free(tempkey);
    NODE_PTR link = GetNodePtr(table, index);
    if (link == NULL) {
        NODE_PTR new = (NODE_PTR)malloc(sizeof(NODE_PTR) + key_size + val_size);
        memset(new, 0, sizeof(NODE_PTR));
        memcpy(new + sizeof(NODE_PTR), key, key_size);
        memcpy(new + sizeof(NODE_PTR) + key_size, val, val_size);
        memcpy(table + HEAD_SIZE + sizeof(NODE_PTR) * index, &new, sizeof(NODE_PTR));
        // printf("%ld = %ld\n", (long)new, (long)GetNodePtr(table, index));
        // printf("v = %d\n", *(int *)(GetNodePtr(table, index) + sizeof(NODE_PTR) + sizeof(int)));
        return 0;
    }
    NODE_PTR new = (NODE_PTR)malloc(sizeof(NODE_PTR) + key_size + val_size);
    memcpy(new, link, sizeof(NODE_PTR));
    memcpy(new + sizeof(NODE_PTR), key, key_size);
    memcpy(new + sizeof(NODE_PTR) + key_size, val, val_size);
    memcpy(link, &new, sizeof(NODE_PTR));
    return 0;
}

int HashAddNode_tail(HASHTABLE table, void *key, void *val) {
    int length = *(int *)(table);
    int key_size = *(int *)(table + sizeof(int));
    int val_size = *(int *)(table + sizeof(int) * 2);
    HASH_FUN hashfun;
    memcpy(&hashfun, table + sizeof(int) * 3, sizeof(HASH_FUN));
    char *tempkey = (char *)malloc(key_size + 1);
    memcpy(tempkey, key, key_size);
    tempkey[key_size] = '\0';
    int index = hashfun(tempkey) % length;
    // printf("index = %d\n", index);
    free(tempkey);
    NODE_PTR link = GetNodePtr(table, index);
    if (link == NULL) {
        NODE_PTR new = (NODE_PTR)malloc(sizeof(NODE_PTR) + key_size + val_size);
        memset(new, 0, sizeof(NODE_PTR));
        memcpy(new + sizeof(NODE_PTR), key, key_size);
        memcpy(new + sizeof(NODE_PTR) + key_size, val, val_size);
        memcpy(table + HEAD_SIZE + sizeof(NODE_PTR) * index, &new, sizeof(NODE_PTR));
        // printf("%ld = %ld\n", (long)new, (long)GetNodePtr(table, index));
        // printf("v = %d\n", *(int *)(GetNodePtr(table, index) + sizeof(NODE_PTR) + sizeof(int)));
        return 0;
    }
    while(GetNextPtr(link) != NULL)
        link = GetNextPtr(link);
    NODE_PTR new = (NODE_PTR)malloc(sizeof(NODE_PTR) + key_size + val_size);
    memset(new, 0, sizeof(NODE_PTR));
    memcpy(new + sizeof(NODE_PTR), key, key_size);
    memcpy(new + sizeof(NODE_PTR) + key_size, val, val_size);
    memcpy(link, &new, sizeof(NODE_PTR));
    return 0;
}

int HashGetValue(HASHTABLE table, void *key, void *buf) {
    int length = *(int *)(table);
    int key_size = *(int *)(table + sizeof(int));
    int val_size = *(int *)(table + sizeof(int) * 2);
    HASH_FUN hashfun = *(HASH_FUN *)(table + sizeof(int) * 3);
    char *tempkey = (char *)malloc(key_size + 1);
    memcpy(tempkey, key, key_size);
    tempkey[key_size] = '\0';
    int index = hashfun(tempkey) % length;
    free(tempkey);
    NODE_PTR nptr = GetNodePtr(table, index);
    while(nptr != NULL) {
        if (memcmp(nptr + sizeof(NODE_PTR), key, key_size) == 0) {
            break;
        }
        nptr = GetNextPtr(nptr);
    }
    if (nptr == NULL)
        return 0;
    memcpy(buf, nptr + sizeof(NODE_PTR) + key_size, val_size);
    return val_size;
}

int HashDelete(HASHTABLE table, void *key) {
    int length = *(int *)(table);
    int key_size = *(int *)(table + sizeof(int));
    int val_size = *(int *)(table + sizeof(int) * 2);
    HASH_FUN hashfun = *(HASH_FUN *)(table + sizeof(int) * 3);
    char *tempkey = (char *)malloc(key_size + 1);
    memcpy(tempkey, key, key_size);
    tempkey[key_size] = '\0';
    int index = hashfun(tempkey) % length;
    free(tempkey);
    NODE_PTR nptr = GetNodePtr(table, index);
    if (nptr == NULL)
        return 0;
    else if (memcmp(nptr + sizeof(NODE_PTR), key, key_size) == 0) {
        memcpy(table + HEAD_SIZE + sizeof(NODE_PTR) * index, nptr, sizeof(NODE_PTR));
        free(nptr);
        return 1;
    }
    else {
        NODE_PTR prev = nptr;
        NODE_PTR curr = nptr;
        while(nptr != NULL && memcmp(nptr + sizeof(NODE_PTR), key, key_size) != 0) {
            prev = nptr;
            nptr = GetNextPtr(nptr);
        }
        if (nptr == NULL)
            return 0;
        memcpy(prev, nptr, sizeof(NODE_PTR));
        free(nptr);
        return 1;
    }
    
}

int HashDestroyTable(HASHTABLE table) {
    int length = *(int *)table;
    int i;
    for(i = 0; i < length; ++i) {
        NODE_PTR curr = GetNodePtr(table, i);
        NODE_PTR next;
        while(curr != NULL) {
            next = GetNextPtr(curr);
            free(curr);
            curr = next;
        }
    }
    free(table);
    return 0;
}
