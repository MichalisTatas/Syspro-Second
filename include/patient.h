#include <sys/types.h>
#include <unistd.h>
#pragma once

typedef struct date
{
    int day;
    int month;
    int year;
} date;
typedef date* datePtr;

typedef struct patient{
    char* recordID;
    char* patientFirstName;
    char* patientLastName;
    char* diseaseID;
    char* country;
    datePtr entryDate;
    datePtr exitDate;
    char* action;
    int age;
    struct patient* next;
}patient;
typedef patient* patientPtr;

datePtr createDate(const char*);
void replaceExitDate(patientPtr, char*, char*);
patientPtr patientListInsert(patientPtr, patientPtr);
int compareDates(datePtr, datePtr);
patientPtr createPatientStruct(const char*, char*, char*);
void destroyPatientList(patientPtr);