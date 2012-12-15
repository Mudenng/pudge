#include "hdbapi.h"
#include <stdio.h>

int main() {
    printf("*****Test hdbapi...*****\n");
    DBHANDLE db = NULL;
    db = OpenHDB("pudge.hdb");
    value_struct value;
    value.content = "hello";
    value.size = 5;
    int key = 100;
    if (PutKeyValue(db, key, &value) != 0) {
        printf("put error.\n");
    }
    value_struct *rv;
    rv = GetValue(db, key);
    if (strcmp(rv->content, "hello") != 0) {
        printf("get error.\n");
    } 
    if (DelKeyValue(db, key) != 0) {
        printf("delete error.\n");
    }

    return 0;
}
