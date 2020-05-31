#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/binaryHeap.h"
#include "../include/queue.h"

BinaryHeapPtr BHInit()
{
    BinaryHeapPtr BH = malloc(sizeof(BinaryHeap));
    BH->root = NULL;
    return BH;
}

BHNodePtr BHNodeCreate(char* key, int volume)
{
    BHNodePtr node = malloc(sizeof(BHNode));
    if (node == NULL) {
        perror("malloc failed !");
        return NULL;
    }

    if ((node->key = malloc(strlen(key) + 1)) == NULL) {
        perror("malloc failed !");
        return NULL;
    }

    strcpy(node->key, key);
    node->volume = volume;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

void BHTreeInsert(BinaryHeapPtr BH, BHNodePtr node)
{
    if (BH->root == NULL) {
        BH->root = node;
        return;
    }

    QueuePtr Q = malloc(sizeof(Queue));
    if (Q == NULL) {
        perror("malloc failed !");
        return;
    }
    QInit(Q);
    BHNodePtr root = BH->root;

    while (root->left != NULL && root->right != NULL) {
        QInsert(Q, root->left);
        QInsert(Q, root->right);
        root = QRemove(Q);
    }

    if (root->left == NULL)
        root->left = node;
    else 
        root->right = node;
    
    QDestroy(Q);
}

void swapInfo(BHNodePtr node1, BHNodePtr node2)
{
    int node1volume = node1->volume;
    char* node1key = malloc(strlen(node1->key) + 1);
    strcpy(node1key, node1->key);

    node1->key = realloc(node1->key, strlen(node2->key) + 1);
    strcpy(node1->key, node2->key);
    node1->volume = node2->volume;

    node2->key = realloc(node2->key, strlen(node1key) + 1);
    strcpy(node2->key, node1key);
    node2->volume = node1volume;

    free(node1key);
}

void heapify(BHNodePtr root)
{
    if (root->left != NULL) {
        heapify(root->left);
        if (root->left->volume > root->volume)
            swapInfo(root->left, root);
    }

    if (root->right != NULL) {
        heapify(root->right);
        if (root->right->volume > root->volume)
            swapInfo(root->right, root);
    }
}

void help(BHNodePtr node)
{
    if (node->left != NULL && node->right != NULL) {
        if (node->left->volume > node->right->volume) {
            if (node->volume < node->left->volume)
                swapInfo(node, node->left);
        }
        else if (node->volume < node->left->volume)
            swapInfo(node, node->right);
    }
    if (node->left != NULL && node->right == NULL) {
        if (node->volume < node->left->volume)
            swapInfo(node, node->left);
    }
    if (node->left == NULL && node->right != NULL) {
        if (node->volume < node->right->volume)
            swapInfo(node, node->right);
    }
}

void hy(BHNodePtr node)
{
    if (node == NULL)
        return;
    hy(node->left);
    hy(node->right);
    help(node);
}


void BHInsert(BinaryHeapPtr BH, char* key, int volume)
{
    BHNodePtr node = malloc(sizeof(BHNode));
    if (node == NULL) {
        perror("malloc failed !");
        return;
    }
    if ((node->key = malloc(strlen(key) + 1)) == NULL) {
        perror("malloc failed !");
        return;
    }
    
    strcpy(node->key, key);
    node->volume = volume;
    node->left = NULL;
    node->right = NULL;

    BHTreeInsert(BH, node);     //insert into complete binary tree
    heapify(BH->root);          //swaps the nodes until max heap is created
}

void lvl(BinaryHeapPtr BH)
{
    QueuePtr Q = malloc(sizeof(Queue));
    if (Q == NULL) {
        perror("malloc failed !");
        return ;
    }
    QInit(Q);
    BHNodePtr root = BH->root;

    QInsert(Q, BH->root);
    while (!QEmpty(Q)) {
        root = QRemove(Q);
        printf("%s %d  \n", root->key, root->volume);
        if (root->left != NULL)
            QInsert(Q, root->left);
        if (root->right != NULL)
            QInsert(Q, root->right);
    }
    QDestroy(Q);
}

BHNodePtr BHRemove(BinaryHeapPtr BH)
{

    if (BH->root == NULL)
        return NULL;

    char* key = malloc(strlen(BH->root->key) + 1);
    strcpy(key, BH->root->key);

    QueuePtr Q = malloc(sizeof(Queue));
    if (Q == NULL) {
        perror("malloc failed !");
        return NULL;
    }
    QInit(Q);
    BHNodePtr root = BH->root;

    QInsert(Q, BH->root);
    while (!QEmpty(Q)) {
        root = QRemove(Q);
        if (root->left != NULL)
            QInsert(Q, root->left);
        if (root->right != NULL)
            QInsert(Q, root->right);
    }

    swapInfo(BH->root, root);

    QInsert(Q, BH->root);
    while (!QEmpty(Q)) {
        root = QRemove(Q);
        if (root->left != NULL)
            QInsert(Q, root->left);
        if (root->right != NULL)
            QInsert(Q, root->right);
        if (!strcmp(root->left->key,key) || !strcmp(root->right->key,key))
            break;
    }

    BHNodePtr temp = malloc(sizeof(BHNode));
    if (!strcmp(root->left->key, key)) {
        temp->key = malloc(strlen(root->left->key) + 1);
        strcpy(temp->key, root->left->key);
        temp->volume = root->left->volume;
        free(root->left->key);
        free(root->left);
        root->left = NULL;
    }
    else if (!strcmp(root->right->key, key)) {
        temp->key = malloc(strlen(root->right->key) + 1);
        strcpy(temp->key, root->right->key);
        temp->volume = root->right->volume;
        free(root->right->key);
        free(root->right);
        root->right = NULL;
    }


    hy(BH->root);
    free(key);
    QDestroy(Q);
    return temp;
}

void destroy(BHNodePtr node)
{
    if (node != NULL)
    {
        destroy(node->left);
        destroy(node->right);
        free(node->key);
        free(node);
    }
}

void BHDestroy(BinaryHeapPtr BH)
{
    destroy(BH->root);
    free(BH);
}