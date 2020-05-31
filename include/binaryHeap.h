#pragma once

typedef struct BHNode
{
    char* key;
    int volume;
    struct BHNode* left;
    struct BHNode* right;
} BHNode;
typedef BHNode* BHNodePtr;

typedef struct BinaryHeap
{
    BHNodePtr root;
} BinaryHeap;
typedef BinaryHeap* BinaryHeapPtr;

void lvl(BinaryHeapPtr );
BinaryHeapPtr BHInit();
BHNodePtr BHNodeCreate(char*, int);
void BHTreeInsert(BinaryHeapPtr, BHNodePtr);
void BHInsert(BinaryHeapPtr, char*, int);
void BHDestroy(BinaryHeapPtr);
BHNodePtr BHRemove(BinaryHeapPtr);

