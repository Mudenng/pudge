/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  conhasn.h                              */
/*  PRINCIPAL AUTHOR      :  pudge                                  */
/*  SUBSYSTEM NAME        :  base structs                           */
/*  MODULE NAME           :  conhash                                */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2013/01/09                             */
/*  DESCRIPTION           :  Interface to a conhash                 */
/********************************************************************/

#ifndef ADVANCED_STRUCTION_CONSISTENT_HASH_H 
#define ADVANCED_STRUCTION_CONSISTENT_HASH_H 

#include "linklist.h"
#include <limits.h>

#define BITS_IN_int     ( sizeof(int) * CHAR_BIT )
#define THREE_QUARTERS  ((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH      ((int) (BITS_IN_int / 8))
#define HIGH_BITS       ( ~((unsigned int)(~0) >> ONE_EIGHTH ))

#define HASH_MAX 4294967296
#define HASH_INCREMENT 65536
#define MAX_INFO_SIZE 50 

typedef CLINKLIST CONHASH;

typedef struct {
    int id;
    char info[MAX_INFO_SIZE];
}Node;


/*
 * Create a conhash table 
 * input    : None
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return conhash handle
 *          : if FAILURE return -1
 */
CONHASH ConhashCreate();

/*
 * Add node to conhash
 * input    : conhash handle, node infomation, infomation length
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int ConhashAddNode(CONHASH conhash, void *info, int info_size);

/*
 * Remove node from conhash
 * input    : conhash handle, node id
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int ConhashRemoveNode(CONHASH conhash, int id);

/*
 * Using key to get the right node
 * input    : conhash handle, key
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return Node pointer
 *          : if FAILURE return NULL
 */
Node *ConhashGetNode(CONHASH conhash, int key);

/*
 * Get the node after right node
 * input    : conhash handle, key, Nth after
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return Node pointer
 *          : if FAILURE return NULL
 */
Node *ConhashGetNodeAfter(CONHASH conhash, int key, int n);

/*
 * Get the node before right node
 * input    : conhash handle, key, Nth before
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return Node pointer
 *          : if FAILURE return NULL
 */
Node *ConhashGetNodeBefore(CONHASH conhash, int key, int n);

/*
 * Get the count of nodes in conhash
 * input    : conhash handle
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return count
 *          : if FAILURE return 0
 */
int ConhashGetSize(CONHASH conhash);

/*
 * Get the space size of conhash
 * input    : conhash handle
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return space size
 *          : if FAILURE return 0
 */
int ConhashGetSpaceSize(CONHASH conhash);

#endif
