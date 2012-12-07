/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  hdbapi.h                               */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  hdbapi                                 */
/*  MODULE NAME           :  hdbapi                                 */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Any                                    */
/*  DATE OF FIRST RELEASE :  2012/11/29                             */
/*  DESCRIPTION           :  Interface of database access           */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/11/29
 *
 */

#ifndef PUDGE_HDBAPI_H_
#define PUDGE_HDBAPI_H_

typedef void* DBHANDLE;

typedef struct value_struct {
    void *content;  // value content
    int size;       // size of value
} value_struct;

/*
 * Open an Database
 * input    : path - database file path
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return (DBHANDLE)Database handle
 *          : if FAILURE return NULL
 */
DBHANDLE OpenHDB(char *path);

/*
 * Close the Database
 * input    : DBHANDLE hdb - database handle
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int CloseHDB(DBHANDLE hdb);

/*
 * Set key/value
 * input    : int key, value_struct value - one key/value
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int PutKeyValue(DBHANDLE hdb, int key, const value_struct *value);


/*
 * Get value using key
 * input    : int key
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return (value_struct *)pointer to value struct
 *          : if FAILURE return NULL
 */
value_struct *GetValue(DBHANDLE hdb, int key);

/*
 * Delete a value struct
 * input    : value_struct *value - pointer to a value struct
 * output   : None
 * in/out   : None
 * return   : None
 */
void FreeValueStruct(value_struct *value);

/*
 * Delete a key/value
 * input    : int key - key
 * output   : None
 * in/out   : None
 * return   : if SUCCESS return 0
 *          : if FAILURE return -1
 */
int DelKeyValue(DBHANDLE hdb, int key);

#endif
