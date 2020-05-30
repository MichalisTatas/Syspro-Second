#include "../include/queriesAnswering.h"

int queriesAnswerer(const char* querie, int bufferSize, HashTablePtr countryHashTable, HashTablePtr diseaseHashTable, countryPtr countriesList, patientPtr patientListHead, int writeDesc, int readDesc)
{
    wordexp_t p;
    wordexp(querie, &p, 0);
    char* answer;
    datePtr date1, date2;

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
        date1 = createDate(p.we_wordv[2]);
        date2 = createDate(p.we_wordv[3]);
        countryPtr countryGiven=NULL;
        if (p.we_wordc == 4) {
            countryGiven = countriesList;
            while(countryGiven != NULL) {
                 char* msg = numPatientAdmissions(diseaseHashTable, countryGiven, p.we_wordv[1], date1, date2);
                msgDecomposer(writeDesc, msg, bufferSize);
                free(msg);
                countryGiven = countryGiven->next;
            }
        }
        else if (p.we_wordc == 5) {
            addCountryInList(&countryGiven, p.we_wordv[4]);
            char* msg = numPatientAdmissions(diseaseHashTable, countryGiven, p.we_wordv[1], date1, date2);
            msgDecomposer(writeDesc, msg, bufferSize);
            free(msg);
            destroyCountryList(countryGiven);
        }
        else {
            printf("number of arguments is not right! \n");
            free(date1);
            free(date2);
            wordfree(&p);
            return -1;
        }
        free(date1);
        free(date2);
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
            return resynthesizePatient(current);
        }
        current = current->next;
    }
    return NULL;
}

char* numPatientAdmissions(HashTablePtr diseaseHashTable, countryPtr countries, char* disease, datePtr date1, datePtr date2) // if country argument given countries contains only that country
{                                                                                             // otherwise every country the worker is handling
    countryPtr current  = countries;
    int bucketNum = hashFunction(disease);
    int numPerCountry;

    numPerCountry = numPatientAdmissionsHelper(current->name, disease, date1, date2, diseaseHashTable->table[bucketNum]->tree);
    char buffer[4];
    sprintf(buffer, "%d",numPerCountry);
    char* msg = malloc(strlen(current->name) + strlen(buffer) + 2);
    strcpy(msg, current->name);
    strcat(msg, " ");
    strcat(msg, buffer);
    return msg;
    
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

int numPatientAdmissionsHelper(char* country, char* disease, datePtr date1, datePtr date2, treeNodePtr tree)
{
    if (tree == NULL)
        return 0;

    if (compareDates(tree->patient->entryDate, date1) == -1) {
        numPatientAdmissionsHelper(country, disease, date1, date2, tree->right);
    }
    else if (compareDates(tree->patient->entryDate, date2) == 1) {
        numPatientAdmissionsHelper(country, disease, date1, date2, tree->left);
    }

    int count = 0;
    patientPtr list;

    if (!strcmp(tree->patient->country, country)) {
        if ((compareDates(tree->patient->entryDate, date1) >= 0) && (compareDates(tree->patient->entryDate, date2) <=0)) {
            list = tree->patient;  //searches the list of patients because some may not be in the tree bcz they have the same date
            while (list != NULL && compareDates(list->entryDate, tree->patient->entryDate)==0){
                count ++;
                list = list->next;
            }
        }
    }

    count += (numPatientAdmissionsHelper(country, disease, date1, date2, tree->left) + numPatientAdmissionsHelper(country, disease, date1, date2, tree->right));

    return count;
}