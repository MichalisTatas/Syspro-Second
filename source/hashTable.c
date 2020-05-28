#include "../include/hashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





// REHASH








HashTablePtr HTCreate(int size)
{
    HashTablePtr ht = malloc(sizeof(HashTable));
    ht->table = malloc(size*sizeof(HTNode));
    ht->size = size;
    for (int i=0; i<size; i++)
        ht->table[i] = NULL;

    return ht;
}

int hashFunction(char* str)
{
    return (*str)/10;
}

HTNodePtr bucketInit(HTNodePtr node)
{
    node = malloc(sizeof(HTNode));
    node->next = NULL;
    return node;
}

void HTInsert(HashTablePtr hashTable, char* key, patientPtr patient)
{
    int bucketNum = hashFunction(key);
    HTNodePtr temp = NULL;

    // if bucket is empty
    if (hashTable->table[bucketNum] == NULL) {
        hashTable->table[bucketNum] = bucketInit(hashTable->table[bucketNum]);
        hashTable->table[bucketNum]->key = malloc(strlen(key) + 1);
        strcpy(hashTable->table[bucketNum]->key, key);
        hashTable->table[bucketNum]->tree = NULL;
        hashTable->table[bucketNum]->tree = AVLInsert(hashTable->table[bucketNum]->tree, patient);
    }
    else {
    // if node with such key exists insert
        temp = hashTable->table[bucketNum];
        while (temp->next != NULL) {
            if (!strcmp(temp->key, key)) {
                temp->tree = AVLInsert(temp->tree, patient);
                return;
            }
            temp = temp->next;
        }
    // if last node has the same key
        if (!strcmp(temp->key, key)) {
            temp->tree = AVLInsert(temp->tree, patient);
            return;
        }
        else {
            temp->next = bucketInit(temp->next);
            temp = temp->next;
            temp->key = malloc(strlen(key) + 1);
            strcpy(temp->key, key);
            temp->tree = NULL;
            temp->tree = AVLInsert(temp->tree, patient);
        }
    }

}

void HTPrint(HashTablePtr hashTable)
{
    HTNodePtr temp;
    for (int i=0; i<hashTable->size; i++) {
        if (hashTable->table[i] != NULL) {
            temp = hashTable->table[i];
            while (temp != NULL) {
                printf("%s --> ", temp->key);
                        // preorder(temp->array[j]->tree);
                        // printf("=========================================================\n");
                temp = temp->next;
            }
        }
        printf("\n");
    }
}

void recursiveDestroy(HTNodePtr node)
{
    if (node == NULL)
        return;
    recursiveDestroy(node->next);
    AVLDestroy(node->tree);
    free(node->key);
    free(node);
}

void HTDestroy(HashTablePtr hashTable)
{
    for (int i=0; i<hashTable->size; i++)
        recursiveDestroy(hashTable->table[i]);
    free(hashTable->table);
    free(hashTable);
}