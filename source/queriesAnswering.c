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
        date1 = createDate(p.we_wordv[2]);
        date2 = createDate(p.we_wordv[3]);
        countryPtr countryGiven=NULL;
        if (p.we_wordc == 4) {
            countryGiven = countriesList;
            while(countryGiven != NULL) {
                char* msg = diseaseFrequency(diseaseHashTable, p.we_wordv[1], date1, date2, countryGiven);
                msgDecomposer(writeDesc, msg, bufferSize);
                free(msg);
                countryGiven = countryGiven->next;
            }
        }
        else if (p.we_wordc == 5) {                      //cahnges the if not problem i guess
            addCountryInList(&countryGiven, p.we_wordv[4]);
            char* msg = diseaseFrequency(diseaseHashTable, p.we_wordv[1], date1, date2, countryGiven);
            msgDecomposer(writeDesc, msg, bufferSize);
            free(msg);
            destroyCountryList(countryGiven);
        }
        free(date1);
        free(date2);
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
        else if (p.we_wordc == 5) {                      //cahnges the if not problem i guess
            addCountryInList(&countryGiven, p.we_wordv[4]);
            char* msg = numPatientAdmissions(diseaseHashTable, countryGiven, p.we_wordv[1], date1, date2);
            msgDecomposer(writeDesc, msg, bufferSize);
            free(msg);
            destroyCountryList(countryGiven);
        }
        free(date1);
        free(date2);
        msgDecomposer(writeDesc, "finished!", bufferSize);
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
        date1 = createDate(p.we_wordv[2]);
        date2 = createDate(p.we_wordv[3]);
        countryPtr countryGiven=NULL;
        if (p.we_wordc == 4) {
            countryGiven = countriesList;
            while(countryGiven != NULL) {
                 char* msg = numPatientDischarges(diseaseHashTable, countryGiven, p.we_wordv[1], date1, date2);
                msgDecomposer(writeDesc, msg, bufferSize);
                free(msg);
                countryGiven = countryGiven->next;
            }
        }
        else if (p.we_wordc == 5) {
            addCountryInList(&countryGiven, p.we_wordv[4]);
            char* msg = numPatientDischarges(diseaseHashTable, countryGiven, p.we_wordv[1], date1, date2);
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
    else if (!strcmp(p.we_wordv[0], "/exit")) {
        wordfree(&p);
        return 1;
    }

    wordfree(&p);
    return 0;
}


char* diseaseFrequency(HashTablePtr diseaseHashTable, char* disease, datePtr date1, datePtr date2, countryPtr country)
{
    int bucketNum = hashFunction(disease);
    int size = diseaseFrequencyHelper(diseaseHashTable->table[bucketNum]->tree, date1, date2, country->name);
    char* buffer = malloc(12);
    snprintf(buffer, 12, "%d\n", size);
    return buffer;
}

int diseaseFrequencyHelper(treeNodePtr tree, datePtr date1, datePtr date2, char* country)
{
    if (tree == NULL)
        return 0;

    if (compareDates(tree->patient->entryDate, date1) == -1) {
        diseaseFrequencyHelper(tree->right, date1, date2, country);
    }
    else if (compareDates(tree->patient->entryDate, date2) == 1) {
        diseaseFrequencyHelper(tree->left, date1, date2,  country);
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

    count += (diseaseFrequencyHelper(tree->left, date1, date2, country) + diseaseFrequencyHelper(tree->right, date1, date2, country));

    return count;
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

char* numPatientAdmissions(HashTablePtr diseaseHashTable, countryPtr countries, char* disease, datePtr date1, datePtr date2) // if country argument given countries contains only that country
{                                                                                             // otherwise every country the worker is handling
    countryPtr current  = countries;
    int bucketNum = hashFunction(disease);
    int numPerCountry;

    numPerCountry = numPatientAdmissionsHelper(current->name, date1, date2, diseaseHashTable->table[bucketNum]->tree);
    char buffer[4];
    sprintf(buffer, "%d",numPerCountry);
    char* msg = malloc(strlen(current->name) + strlen(buffer) + 2);
    strcpy(msg, current->name);
    strcat(msg, " ");
    strcat(msg, buffer);
    return msg;
}

int numPatientAdmissionsHelper(char* country, datePtr date1, datePtr date2, treeNodePtr tree)
{
    if (tree == NULL)
        return 0;

    if (compareDates(tree->patient->entryDate, date1) == -1) {
        numPatientAdmissionsHelper(country, date1, date2, tree->right);
    }
    else if (compareDates(tree->patient->entryDate, date2) == 1) {
        numPatientAdmissionsHelper(country, date1, date2, tree->left);
    }

    int count = 0;
    patientPtr list;

    if (!strcmp(tree->patient->country, country)) {              // does it need || NULL
        if ((compareDates(tree->patient->entryDate, date1) >= 0) && (compareDates(tree->patient->entryDate, date2) <=0)) {
            list = tree->patient;  //searches the list of patients because some may not be in the tree bcz they have the same date
            while (list != NULL && compareDates(list->entryDate, tree->patient->entryDate)==0){
                count ++;
                list = list->next;
            }
        }
    }

    count += (numPatientAdmissionsHelper(country, date1, date2, tree->left) + numPatientAdmissionsHelper(country, date1, date2, tree->right));

    return count;
}

char* numPatientDischarges(HashTablePtr diseaseHashTable, countryPtr countries, char* disease, datePtr date1, datePtr date2) // if country argument given countries contains only that country
{                                                                                             // otherwise every country the worker is handling
    countryPtr current  = countries;
    int bucketNum = hashFunction(disease);
    int numPerCountry;

    numPerCountry = numPatientDischargesHelper(current->name, disease, date1, date2, diseaseHashTable->table[bucketNum]->tree);
    char buffer[4];
    sprintf(buffer, "%d",numPerCountry);
    char* msg = malloc(strlen(current->name) + strlen(buffer) + 2);
    strcpy(msg, current->name);
    strcat(msg, " ");
    strcat(msg, buffer);
    return msg;
}

int numPatientDischargesHelper(char* country, char* disease, datePtr date1, datePtr date2, treeNodePtr tree)
{
    if (tree == NULL)
        return 0;

    if (compareDates(tree->patient->entryDate, date1) == -1) {
        numPatientDischargesHelper(country, disease, date1, date2, tree->right);
    }
    else if (compareDates(tree->patient->entryDate, date2) == 1) {
        numPatientDischargesHelper(country, disease, date1, date2, tree->left);
    }

    int count = 0;
    patientPtr list;

    if (!strcmp(tree->patient->country, country)) {
        if (tree->patient->exitDate != NULL) {           // MIGHT CHANGE LATER
            if ((compareDates(tree->patient->exitDate, date1) >= 0) && (compareDates(tree->patient->exitDate, date2) <=0)) {
                list = tree->patient;  //searches the list of patients because some may not be in the tree bcz they have the same date
                while (list != NULL && compareDates(list->entryDate, tree->patient->entryDate)==0){
                    count ++;
                    list = list->next;
                }
            }
        }
    }

    count += (numPatientDischargesHelper(country, disease, date1, date2, tree->left) + numPatientDischargesHelper(country, disease, date1, date2, tree->right));

    return count;
}