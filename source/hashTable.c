#include "../include/hashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashTablePtr HTCreate(int size, int bucketSize)
{
    HashTablePtr ht = malloc(sizeof(HashTable));
    ht->table = malloc(size*sizeof(HTNode));
    ht->size = size;
    // check if it fits perfectly
    int checker;
    if ((checker = bucketSize % 8) == 0)
        ht->ptrNum = bucketSize/8 - 1; //see how many pointers i can store minus the one to the next HTNode
    else 
        ht->ptrNum = (bucketSize - checker)/8 - 1;

    for (int i=0; i<size; i++)
        ht->table[i] = NULL;

    return ht;
}

int hashFunction(char* str)    //djb2 hash function ;;
{
    return (*str)/10;
}

int hasBucketFreeSpace(HTNodePtr hashNode, int numOfEntries)
{

    for (int i=0; i<numOfEntries; i++) {
        if (hashNode->array[i] == NULL)
            return i;
    }

    return -1;
}

int findTree(HTNodePtr htNode, char* key, int ptrNum)            //finds if the tree exists in the bucket
{
    for (int i=0; i<ptrNum; i++) {
        if (htNode->array[i] != NULL) {
            if (!strcmp(htNode->array[i]->key, key))
                return i;
        }
    }
    return -1;
}

HTNodePtr bucketInit(HTNodePtr node, int ptrNum)
{
        node = malloc(sizeof(HTNode));
        node->array = malloc(ptrNum * sizeof(ItemNode));
        node->next = NULL;
        for (int i=0; i<ptrNum; i++)
            node->array[i] = NULL;
        return node;
}

void HTInsert(HashTablePtr hashTable, char* key, patientPtr patient)
{
    int bucketNum = hashFunction(key);
    HTNodePtr temp = NULL;
    int position = 0;
    int help;

    // if bucket is empty
    if (hashTable->table[bucketNum] == NULL) {
        hashTable->table[bucketNum] = bucketInit(hashTable->table[bucketNum], hashTable->ptrNum);        //check if needs return type
        hashTable->table[bucketNum]->array[0] = malloc(sizeof(ItemNode));
        hashTable->table[bucketNum]->array[0]->key = malloc(strlen(key) + 1);
        strcpy(hashTable->table[bucketNum]->array[0]->key, key);
        hashTable->table[bucketNum]->array[0]->tree = NULL;
        hashTable->table[bucketNum]->array[0]->tree = AVLInsert(hashTable->table[bucketNum]->array[0]->tree, patient);
    }
    // else {
    // // if node with such key exists insert
    //     temp = hashTable->table[bucketNum];
    //     while (temp->next != NULL) {
    //         if ((position = findTree(temp, key, hashTable->ptrNum)) != -1) {
    //             temp->array[position]->tree = AVLInsert(temp->array[position]->tree, patient);
    //             return;
    //         }
    //         temp = temp->next;
    //     }
    // // last bucket
    //     // check if key exists there
    //     if ((position = findTree(temp, key, hashTable->ptrNum)) != -1) {
    //         temp->array[position]->tree = AVLInsert(temp->array[position]->tree, patient);
    //         return;
    //     }
    //     // if not check if it has free space
    //     if ((help = hasBucketFreeSpace(temp, hashTable->ptrNum)) != -1) {
    //         temp->array[help] = malloc(sizeof(ItemNode));
    //         temp->array[help]->key = malloc(strlen(key) + 1);
    //         strcpy(temp->array[help]->key, key);
    //         temp->array[help]->tree = NULL;
    //         temp->array[help]->tree = AVLInsert(temp->array[help]->tree, patient);
    //     }
    //     // if no free space create new hash node
    //     else {
    //         temp->next = bucketInit(temp->next, hashTable->ptrNum);
    //         temp = temp->next;
    //         temp->array[0] = malloc(sizeof(ItemNode));
    //         temp->array[0]->key = malloc(strlen(key) + 1);
    //         strcpy(temp->array[0]->key, key);
    //         temp->array[0]->tree = NULL;
    //         temp->array[0]->tree = AVLInsert(temp->array[0]->tree, patient);
    //     }
    // }

}

void HTPrint(HashTablePtr hashTable)
{
    HTNodePtr temp;
    for (int i=0; i<hashTable->size; i++) {
        if (hashTable->table[i] != NULL) {
            temp = hashTable->table[i];
            while (temp != NULL) {
                for (int j=0; j<hashTable->ptrNum; j++) {
                    if (temp->array[j] != NULL)
                        printf("%s --> ", temp->array[j]->key);
                        // preorder(temp->array[j]->tree);
                        // printf("=========================================================\n");
                }
                temp = temp->next;
            }
        }
        printf("\n");
    }
}

void recursiveDestroy(HTNodePtr node, int ptrNum)
{
    if (node == NULL)
        return;
    recursiveDestroy(node->next, ptrNum);
    for (int i=0; i<ptrNum; i++) {
        if (node->array[i] != NULL) {
            AVLDestroy(node->array[i]->tree);
            free(node->array[i]->key);
            free(node->array[i]);
        }
    }
    free(node->array);
    free(node);
}

void HTDestroy(HashTablePtr hashTable)
{
    for (int i=0; i<hashTable->size; i++)
        recursiveDestroy(hashTable->table[i], hashTable->ptrNum);
    free(hashTable->table);
    free(hashTable);
}