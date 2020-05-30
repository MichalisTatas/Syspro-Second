#include "../include/queriesAnswering.h"

int queriesAnswerer(const char* querie, int bufferSize, HashTablePtr countryHashTable, HashTablePtr diseaseHashTable, countryPtr countriesList, patientPtr patientListHead, int writeDesc, int readDesc)
{
    wordexp_t p;
    wordexp(querie, &p, 0);
    char* answer;

    if (!strcmp(p.we_wordv[0], "/listCountries")) {
        msgDecomposer(writeDesc, "/listCountries", bufferSize);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
        msgDecomposer(writeDesc, "/diseaseFrequency", bufferSize);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
        msgDecomposer(writeDesc, "/topk-AgeRanges", bufferSize);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
        if ( (answer = searchPatientRecord(p.we_wordv[1], patientListHead)) == NULL) 
            msgDecomposer(writeDesc, "finished!", bufferSize);      // id not here
        else {
            msgDecomposer(writeDesc, answer, bufferSize);
            free(answer);
            msgDecomposer(writeDesc, "finished!", bufferSize);
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
        msgDecomposer(writeDesc, "/numPatientAdmissions", bufferSize);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
        msgDecomposer(writeDesc, "/numPatientDischarges", bufferSize);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/exit")) {
        wordfree(&p);
        return 1;
    }

    wordfree(&p);
    return 0;
}


char* topkAgeRanges(int k, char* country, char* disease, datePtr date1, datePtr date2, HashTablePtr countryHashTable, HashTablePtr diseaseHashTable)
{
    // needs binary heap opote epa8e trikimia pros to parwn
    return NULL;
}

char* searchPatientRecord(char* recordId, patientPtr patientListHead)
{
    patientPtr current = patientListHead;
    while (current != NULL) {
        if (!strcmp(current->recordID, recordId)) {
            printf("found him! \n");
            return resynthesizePatient(current);
        }
        current = current->next;
    }
    return NULL;
}

char* resynthesizePatient(patientPtr patient)
{
    char age[sizeof(patient->age)];
    sprintf(age, "%d", patient->age);

    char entryDate[sizeof(patient->entryDate->day) + sizeof(patient->entryDate->month) + sizeof(patient->entryDate->year) + 2];
    sprintf(entryDate, "%d-%d-%d", patient->entryDate->day, patient->entryDate->month, patient->entryDate->year);

    char* exitDate;
    if (patient->exitDate != NULL) {
        exitDate = malloc(sizeof(patient->exitDate->day) + sizeof(patient->exitDate->month) + sizeof(patient->exitDate->year) + 2);
        sprintf(exitDate, "%d-%d-%d", patient->exitDate->day, patient->exitDate->month, patient->exitDate->year);
    }
    else
        exitDate = "--";       // LOL?

    char* buffer = malloc(
                   strlen(patient->recordID) + strlen(patient->patientFirstName) + 
                   strlen(patient->patientLastName) + strlen(patient->diseaseID) + 
                   strlen(age) + strlen(entryDate) + strlen(exitDate) + 7
                   );

    strcpy(buffer, patient->recordID);
    strcat(buffer, " ");
    strcat(buffer, patient->patientFirstName);
    strcat(buffer, " ");
    strcat(buffer, patient->patientLastName);
    strcat(buffer, " ");
    strcat(buffer, patient->diseaseID);
    strcat(buffer, " ");
    strcat(buffer, age);
    strcat(buffer, " ");
    strcat(buffer, entryDate);
    strcat(buffer, " ");
    strcat(buffer, exitDate);

    return buffer;
}