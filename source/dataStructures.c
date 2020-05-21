#include "../include/dataStructures.h"

workerInfoPtr addPidInList(workerInfoPtr head, int pid)
{
    workerInfoPtr temp, curr;
    temp = malloc(sizeof(workerInfo));
    temp->pid = pid;
    temp->read = -1;
    temp->write = -1;
    temp->countriesArray = NULL;
    temp->next = NULL;

    if (head == NULL)
        return temp;

    curr = head;
    while (curr->next != NULL)
        curr = curr->next;
    
    curr->next = temp;
    return head;
}

void destroyList(workerInfoPtr head)
{
    if (head == NULL)
        return;
    destroyList(head->next);
    // free(head->countriesArray);
    free(head);
}