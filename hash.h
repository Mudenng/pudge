/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  hasn.h                                 */
/*  PRINCIPAL AUTHOR      :  lei                                    */
/*  SUBSYSTEM NAME        :  base structs                           */
/*  MODULE NAME           :  hash table                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/12/01                             */
/*  DESCRIPTION           :  Interface to a general hash table      */
/********************************************************************/

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

/*
 * Create a hash table 
 * input    : table length, key size, value size, hash function
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return hash table handle
 *          : if FAILURE return -1
 */
HASHTABLE HashCreateTable(int length, int key_size, int val_size, HASH_FUN hashfun);

#define HashCreateTablePJW(length, key_size, val_size) HashCreateTable(length, key_size, val_size, HashPJW)

/*
 * Destroy a hash table
 * input    : hash table handle
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int HashDestroyTable(HASHTABLE table);

/*
 * Insert a node to hash table, using tail insert
 * input    : hash talbe handle, key, value
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int HashAddNode_tail(HASHTABLE table, void *key, void *val); 

/*
 * Insert a node to hash table, using head insert
 * input    : hash talbe handle, key, value
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int HashAddNode(HASHTABLE table, void *key, void *val);

/*
 * Get value by key 
 * input    : hash talbe handle, key
 * output   : None
 * in/out   : buffer
 * return   : if SUCCESS return size of value
 *          : if FAILURE return 0
 */
int HashGetValue(HASHTABLE table, void *key, void *buf);

/*
 * Delete key-value by key 
 * input    : hash talbe handle, key
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0 
 *          : if FAILURE return -1
 */
int HashDelete(HASHTABLE table, void *key);

/*
 * A default hash function 
 * input    : some thing to hash
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return key 
 *          : if FAILURE return -1
 */
unsigned int HashPJW (const char *datum);	

#endif
