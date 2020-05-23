#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#pragma once

int createPipe(char*, int, int, char*);
void msgDecomposer(int, char*, int);
char* msgComposer(int, char*, int);