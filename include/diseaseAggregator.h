#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/select.h>

#include "pipes.h"
#include "workers.h"
#include "list.h"

int diseaseAggregatorFunction(int, int, char*);
int diseaseAggregatorApp(workerInfoPtr, int, int, int);
int countriesNumber(char*);