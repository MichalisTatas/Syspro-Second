#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#pragma once

#include "patient.h"

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
int addCountryInList(countryPtr*, char*);
countryPtr sortedAddDateInList(countryPtr, const char*);
countryPtr addDateAndSort(countryPtr, char*);
bool existIn(patientPtr, patientPtr);
void destroyCountryList(countryPtr);
void destroyList(workerInfoPtr);