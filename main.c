/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  main.c                                 */
/*  PRINCIPAL AUTHOR      :  Pudge Group                            */
/*  SUBSYSTEM NAME        :  HDB                                    */
/*  MODULE NAME           :  main                                   */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  LINUX/UNIX                             */
/*  DATE OF FIRST RELEASE :  2012/11/29                             */
/*  DESCRIPTION           :  A key-value database system            */
/********************************************************************/
/********************************************************************/
/*              Software Architecture of Pudge                      */
/********************************************************************
                    *****************
                    *      main     *
                    *****************
                            |
                            |
                    *****************
                    *     hdbapi    *
                    *****************
                            |
                            |
                    *****************
                    * Tokyo Cabinet *
                    *****************
**********************************************************************/

/*
 * Revision log:
 *
 * Created by Pudge Group,2012/11/29
 *
 */

#include "hdbapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    DBHANDLE db;
    value_struct value;
    value_struct *rv;
    int key;
    int ret;

    db = OpenHDB("db.hdb");
    if (db == NULL) {
        return -1;
    }
    key = 4;
    value.content = strdup("hello world!");
    value.size = strlen(value.content);
    ret = PutKeyValue(db, key, &value);
    if (ret == -1) {
        printf("put key/value error: %d / %s\n", key, (char *)value.content);
    }
    rv = GetValue(db, key);
    if (rv) { 
        printf("%s - %d\n", (char *)rv->content, rv->size);
        FreeValueStruct(rv);
    }
    DelKeyValue(db, key);

    rv = GetValue(db, key);
    if (rv) { 
        printf("%s - %d\n", (char *)rv->content, rv->size);
        FreeValueStruct(rv);
    }

    CloseHDB(db);
    return 0;
}

