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
 * Pudge 0.1, can set/get key-value. Created by Pudge Group, 2012/11/29
 * Pudge 0.2, add command line interfate. Created by Pudge Group, 2012/12/07
 *
 */

#include "hdbapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define HELP_INFO()                                                 \
        printf("-------------------------------------------\n");    \
        printf("          You can do these:\n");                    \
        printf("-------------------------------------------\n");    \
        printf(" '?'             - Get help\n");                    \
        printf(" 'open filename' - Open a data file\n");            \
        printf(" 'put key value' - Store a key-value\n");           \
        printf(" 'get key'       - Get value by key\n");            \
        printf(" 'delete key'    - Delete a key-value\n");          \
        printf(" 'close'         - Close current data file\n");     \
        printf(" 'exit'          - Exit\n");                        \
        printf("-------------------------------------------\n");

#define STRING_LEN 1000

DBHANDLE db = NULL;
char DBName[STRING_LEN] = "\0";

int CommandMatching(char *command, char *pattern) {
    regex_t reg;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regcomp(&reg, pattern, REG_EXTENDED | REG_ICASE | REG_NOSUB);
    if ( regexec(&reg, command, nmatch, pmatch, 0) == 0 )
    {
        regfree(&reg);
        return 0;
    }
    regfree(&reg);
    return -1;
}

void ExecCommand(char *command) {
    if ( CommandMatching(command, "\\?") == 0 ) {
        HELP_INFO();
    } 
    else if ( CommandMatching(command, "open( +|\\t).+\\.hdb$") == 0 ) {
        if (strlen(DBName) != 0) {
            printf("Close current data file '%s' first.\n", DBName);
        }
        else {
            sscanf(command, "open %s", DBName);
            db = OpenHDB(DBName);
            if (db == NULL) {
                DBName[0] = '\0';
            }
        }
    }
    else if ( db == NULL && CommandMatching(command, "put|get|delete|close") == 0 ) {
        printf("Open a data file first!\n");
    }
    else if ( CommandMatching(command, "put( +|\\t)[0-9]+( +|\\t).+") == 0 ) {
        int key;
        value_struct value;
        char temp[STRING_LEN] = "\0";
        sscanf(command, "put %d %s", &key, temp);
        value.content = temp;
        value.size = strlen((char *)value.content);
        if (PutKeyValue(db, key, &value) == 0) {
            printf("Success\n");
        }
    }
    else if ( CommandMatching(command, "get( +|\\t)[0-9]+$") == 0 ) {
        int key;
        value_struct *value;
        sscanf(command, "get %d", &key);
        value = GetValue(db, key);
        if (value) {
            printf("%d --> %s\n", key, (char *)value->content);
            FreeValueStruct(value);
        }
    }
    else if ( CommandMatching(command, "delete( +|\\t)[0-9]+$") == 0 ) {
        int key = -1;
        sscanf(command, "delete %d", &key);
        if (DelKeyValue(db, key) == 0) {
            printf("Success\n");
        }
    }
    else if ( CommandMatching(command, "close") == 0 ) {
        if (db) {
            CloseHDB(db);
            db = NULL;
        }
        DBName[0] = '\0';
    }
    else if ( CommandMatching(command, "exit") == 0 ) {
        if (db) {
            CloseHDB(db);
        }
        exit(0);
    }
    else {
        printf("Wrong Command, try again.\n");
    }
}

int main() {
    char cmdbuf[STRING_LEN];
    HELP_INFO();
    while(1) {
        printf("Pudge@%s > ", DBName);
        fgets(cmdbuf, STRING_LEN, stdin);
        cmdbuf[strlen(cmdbuf) - 1] = '\0';
        ExecCommand(cmdbuf);
    }
    return 0;
}

