#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct country {
    char* name;
    struct country* next;
} country;
typedef country* countryPtr;

typedef struct workerInfo
{
    int pid;
    int read;
    int write;
    countryPtr countriesList;
    struct workerInfo* next;
} workerInfo;

typedef workerInfo* workerInfoPtr;

workerInfoPtr addPidInList(workerInfoPtr, int);
void addCountryInList(workerInfoPtr, char*);
void destroyList(workerInfoPtr, int, int);