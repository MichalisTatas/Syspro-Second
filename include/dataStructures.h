#include <stdio.h>
#include <stdlib.h>

typedef struct workerInfo
{
    int pid;
    int read;
    int write;
    char** countriesArray;
    struct workerInfo* next;
} workerInfo;

typedef workerInfo* workerInfoPtr;

workerInfoPtr addPidInList(workerInfoPtr, int);
void destroyList(workerInfoPtr);