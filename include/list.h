#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#pragma once

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
    bool readyForWork;
    countryPtr countriesList;
    struct workerInfo* next;
} workerInfo;

typedef workerInfo* workerInfoPtr;

workerInfoPtr addPidInList(workerInfoPtr, int);
void addCountryInList(countryPtr*, char*);
void destroyCountryList(countryPtr);
void destroyList(workerInfoPtr);