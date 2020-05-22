#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 

#include "pipes.h"
#include "workers.h"
#include "dataStructures.h"

int diseaseAggregatorFunction(int, int, char*);
int diseaseAggregatorApp(workerInfoPtr, int, int);
int countriesNumber(char*);