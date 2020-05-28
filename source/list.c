#include "../include/list.h"

workerInfoPtr addPidInList(workerInfoPtr head, int pid)
{
    workerInfoPtr temp, curr;
    temp = malloc(sizeof(workerInfo));
    temp->pid = pid;
    temp->read = -1;
    temp->write = -1;
    temp->readyForWork = false;
    temp->countriesList = NULL;
    temp->next = NULL;

    if (head == NULL)
        return temp;

    curr = head;
    while (curr->next != NULL)
        curr = curr->next;
    
    curr->next = temp;
    return head;
}

// countryPtr addDateAndSort(countryPtr head, char* date)
// {
//     datePtr toBeInserted = createDate(date);

// }

void addCountryInList(countryPtr* head, char* countryName)
{
    countryPtr cntry = malloc(sizeof(country));
    if ((cntry->name = malloc(strlen(countryName) + 1)) == NULL) {
        perror("malloc failed!");
        return;
    }
    strcpy(cntry->name, countryName);

    cntry->next = *head;
    *head = cntry;
}

void destroyCountryList(countryPtr head)
{
    if (head == NULL)
        return;
    destroyCountryList(head->next);
    free(head->name);
    free(head);
}

void destroyList(workerInfoPtr head)
{
    if (head == NULL)
        return;
    destroyList(head->next);
    destroyCountryList(head->countriesList);
    free(head);
}