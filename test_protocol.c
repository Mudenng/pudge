#include "protocol.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1000

int main() {
    printf("*****Test protocol...*****\n");
    char buffer[BUFFER_SIZE];
    char data1[BUFFER_SIZE];
    char data2[BUFFER_SIZE];
    int len;
    int cmd_code;
    int size1;
    int size2;
    CreateMsg0(buffer, &len, CLOSE);
    AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
    if (cmd_code != CLOSE || size1 != 0 || size2 != 0) {
        printf("CreateMsg0 test failed.\n");
    }
    char *s = "ok";
    CreateMsg1(buffer, &len, OPEN, s, strlen(s));
    AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
    data1[size1] = '\0';
    if (cmd_code != OPEN || strcmp(data1, s) != 0 || size2 != 0) {
        printf("CreateMsg1 test failed.\n");
    }
    char *s1 = "ok";
    char *s2 = "hello";
    CreateMsg2(buffer, &len, PUT, s1, strlen(s1), s2, strlen(s2));
    AnalyseMsg(buffer, &cmd_code, data1, &size1, data2, &size2);
    data1[size1] = '\0';
    data2[size2] = '\0';
    if (cmd_code != PUT || strcmp(data1, s1) != 0 || strcmp(data2, s2) != 0) {
        printf("CreateMsg2 test failed.\n");
    }
    return 0;
}
