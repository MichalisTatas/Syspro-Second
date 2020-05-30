#include <wordexp.h>
#include <sys/select.h>

#include "hashTable.h"
#include "list.h"
#include "pipes.h"

int queriesAnswerer(const char*, int, HashTablePtr, HashTablePtr, countryPtr, patientPtr, int , int);
char* topkAgeRanges(int, char*, char*, datePtr, datePtr, HashTablePtr, HashTablePtr);
char* searchPatientRecord(char*, patientPtr);

char* resynthesizePatient(patientPtr);
