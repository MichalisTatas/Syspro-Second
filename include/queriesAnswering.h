#include <wordexp.h>
#include <sys/select.h>

#include "hashTable.h"
#include "list.h"
#include "pipes.h"
#include "patient.h"

int queriesAnswerer(const char*, int, HashTablePtr, HashTablePtr, countryPtr, patientPtr, int , int);
char* topkAgeRanges(int, char*, char*, datePtr, datePtr, HashTablePtr, HashTablePtr);
char* searchPatientRecord(char*, patientPtr);
char* numPatientAdmissions(HashTablePtr, countryPtr, char*, datePtr, datePtr);

char* resynthesizePatient(patientPtr);
int searchInTree(char*, char*, datePtr, datePtr, treeNodePtr);
