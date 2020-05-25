#include "../include/patient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

datePtr createDate(char* line)
{
    datePtr dt = malloc(sizeof(date));
    line = strtok(line, "-");
    dt->day = atoi(line);
    line = strtok(NULL, "-");
    dt->month = atoi(line);
    line = strtok(NULL, " ");
    dt->year = atoi(line);
    return dt;
}

void replaceExitDate(patientPtr head, char* recordID, char* dt)
{
    datePtr date;
    if (strcmp(dt, "-"))
        date = createDate(dt);
    else 
        date = NULL;
    patientPtr temp = head;
    while (temp->next != NULL){
        if (!strcmp(temp->recordID, recordID)) {
            if (temp->exitDate == NULL) {
                temp->exitDate = malloc(sizeof(date));
                temp->exitDate = date;
            }
            else {
                free(temp->exitDate);
                temp->exitDate = date;
            }
            return;
        }
        temp = temp->next;
    }
}

int compareDates(datePtr date1, datePtr date2)
{
    if (date1->year > date2->year)
        return 1;
    else if (date1->year < date2->year)
        return -1;
    else {
        if (date1->month > date2->month)
            return 1;
        else if (date1->month < date2->month)
            return -1;
        else {
            if (date1->day > date2->day)
                return 1;
            else if (date1->day < date2->day)
                return -1;
        }
    }
    return 0;
}

patientPtr patientListInsert(patientPtr head, patientPtr current)
{
    if (head == NULL)
        return current;

    patientPtr temp = head;

    if (compareDates(head->entryDate, current->entryDate) == 1) {
        current->next = head;
        head = current;
    }
    else {
        while (temp->next != NULL) {
            if (compareDates(temp->next->entryDate, current->entryDate) == 1) {
                current->next = temp->next;
                temp->next = current;
                return head;
            }
            temp = temp->next;
        }
        if (temp->next == NULL)
            temp->next = current;

    }

    return head;
}

patientPtr createPatientStruct(char* line)
{

    wordexp_t p;
    wordexp(line, &p, 0);
    patientPtr current = malloc(sizeof(patient));

    current->recordID = malloc(strlen(p.we_wordv[0]) + 1);
    strcpy(current->recordID, p.we_wordv[0]);

    current->patientFirstName = malloc(strlen(p.we_wordv[1]) + 1);
    strcpy(current->patientFirstName, p.we_wordv[1]);
    
    current->patientLastName = malloc(strlen(p.we_wordv[2]) + 1);
    strcpy(current->patientLastName, p.we_wordv[2]);
    
    current->diseaseID = malloc(strlen(p.we_wordv[3]) + 1);
    strcpy(current->diseaseID, p.we_wordv[3]);
    
    current->country = malloc(strlen(p.we_wordv[4]) + 1);
    strcpy(current->country, p.we_wordv[4]);

    current->entryDate = createDate(p.we_wordv[5]);
    current->exitDate = createDate(p.we_wordv[6]);
    current->next = NULL;
    wordfree(&p);
    return current;
}

void destroyPatientList(patientPtr patient)
{
    if (patient == NULL)
        return;
    destroyPatientList(patient->next);
    free(patient->recordID);
    free(patient->country);
    free(patient->diseaseID);
    free(patient->entryDate);
    free(patient->exitDate);
    free(patient->patientFirstName);
    free(patient->patientLastName);
    free(patient);

}