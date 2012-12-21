/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  hdbapi.c                               */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  hdbapi                                 */
/*  MODULE NAME           :  hdbapi                                 */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/11/29                             */
/*  DESCRIPTION           :  Interface of database access           */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/11/29
 *
 */

#include "hdbapi.h"

#include <stdio.h>
#include <tcutil.h>
#include <tchdb.h>

/*
 * Open a Database
 */
DBHANDLE OpenHDB(char *path) {
    TCHDB *newhdb;
    int error_code;

    // create the object
    newhdb = tchdbnew();

    tchdbsetmutex(newhdb);

    // open the database
    if ( !tchdbopen(newhdb, path, HDBOWRITER | HDBOCREAT) ) {
        error_code = tchdbecode(newhdb);
        printf("Open hdb error: %s\n", tchdberrmsg(error_code));
        return NULL;
    }
    return newhdb;
}

/*
 * Close a Database
 */
int CloseHDB(DBHANDLE hdb) {
    int error_code;

    if ( !tchdbclose((TCHDB *)hdb) ) {
        error_code = tchdbecode((TCHDB *)hdb);
        printf("Close hdb error: %s\n", tchdberrmsg(error_code));
        return -1;
    }
    tchdbdel((TCHDB *)hdb);

    return 0;
}

/*
 * Set a key/value
 */
int PutKeyValue(DBHANDLE hdb, int key, const value_struct *value) {
    int error_code;

    if ( !tchdbput((TCHDB *)hdb, &key, sizeof(int), value->content, value->size) ) {
        error_code = tchdbecode((TCHDB *)hdb);
        printf("Put Key-Value error: %s\n", tchdberrmsg(error_code));
        return -1;
    }

    return 0;
}

/*
 * Get value by key 
 */
value_struct *GetValue(DBHANDLE hdb, int key) {
    int error_code;
    value_struct *value;
    void *vbuf;
    int vsize;

    vbuf = tchdbget((TCHDB *)hdb, &key, sizeof(int), &vsize);
    if (vbuf) {
        value = (value_struct *)malloc(sizeof(struct value_struct));
        value->content = vbuf;
        value->size = vsize;
    } else {
        error_code = tchdbecode((TCHDB *)hdb);
        printf("Get Value error: %s\n", tchdberrmsg(error_code));
        return NULL;
    }

    return value;
}

/*
 * Delete a value struct
 */
void FreeValueStruct(value_struct *value) {
    free(value->content);
    free(value);
    value = NULL;
}

/*
 * Delete a key/value
 */
int DelKeyValue(DBHANDLE hdb, int key) {
    int error_code;

    if ( !tchdbout((TCHDB *)hdb, &key, sizeof(int)) ) {
        error_code = tchdbecode((TCHDB *)hdb);
        printf("Delete key/value error: %s\n", tchdberrmsg(error_code));
        return -1;
    }

    return 0;
}
