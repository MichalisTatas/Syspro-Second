#include "../include/diseaseAggregator.h"

int diseaseAggregatorFunction(int bufferSize, int numWorkers, char* inputDirectory)
{
    pid_t pid;
    workerInfoPtr workersList = NULL;

    int countriesNum = countriesNumber(inputDirectory);
    if (numWorkers > countriesNum)                       // if workers more than countries some will do nothing
        numWorkers = countriesNum;

    for (int i=0; i<numWorkers; i++) {
        
        pid = fork();

        if (pid == -1) {
            perror("fork failed!\n");
            return -1;
        }
        else if (pid == 0)
            break;
        else
            workersList = addPidInList(workersList, pid);
    }

    if (pid != 0) {
        if (diseaseAggregatorApp(workersList, numWorkers, countriesNum) == -1) {
            printf("Error occurred in diseaseAggregatorApp!\n");
            return -1;
        }
    }
    else {
        if (workersFunction(getpid()) == -1) {   //maybe use exec to do this
            printf("Error in workers Function\n");
            return -1;
        }
    }

    destroyList(workersList);
    return 0;
}

int diseaseAggregatorApp(workerInfoPtr workersList, int numWorkers, int countriesNum)
{
    //open all the pipes and store countries in list and sent countries to workers
    DIR* countriesDir;
    struct dirent* d;

    if ((countriesDir = opendir("bashScript/dir")) == NULL) {
        perror("opendir");
        return -1;
    }

    
    workerInfoPtr iterator = workersList;
    int number = 0;
    if (countriesNum % numWorkers == 0)
        number = countriesNum / numWorkers;
    else 
        number = countriesNum / numWorkers +1 ;

    // in this while i distribute the countries to each child open the pipes
    while ((d = readdir(countriesDir)) != NULL) {

        if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
            continue;

        if (iterator->read == -1 || iterator->write == -1) {
            
        }

        if (iterator->countriesArray != NULL) {
            for (int i=0; i<number; i++) {
                if (iterator->countriesArray[i] == NULL) {
                    iterator->countriesArray[i] = malloc(strlen(d->d_name) + 1);
                    strcpy(iterator->countriesArray[i], d->d_name);
                    break;
                }
            }
        }
        else
        {
            initializeArray(iterator, number);
            iterator->countriesArray[0] = malloc(strlen(d->d_name) + 1);
            strcpy(iterator->countriesArray[0], d->d_name);
        }

        iterator = iterator->next;
        if (iterator == NULL)
            iterator = workersList;
    }

    workerInfoPtr iterator1 = workersList;
    while(iterator1!=NULL){
        for(int i=0; i<number; i++) {
            if(iterator1->countriesArray[i] != NULL)
                printf("%s ->", iterator1->countriesArray[i]);
        }
        printf("\n\n\n");
        iterator1=iterator1->next;
    }

    if (closedir(countriesDir) == -1) {
        perror("closedir failed!");
        return -1;
    }

    //while(true){
    //     read write
    // }
    return 0;
}

void initializeArray(workerInfoPtr node, int number)
{
    node->countriesArray = malloc(number*sizeof(char*));
    for (int i=0; i<number; i++)
        node->countriesArray[i] = NULL;
}

int countriesNumber(char* countriesDirName)
{
    //shoiuld give name of dir and strcat etc
    DIR* countriesDir;
    struct dirent* d;
    int counter=0;

    if ((countriesDir = opendir("bashScript/dir")) == NULL) {
        perror("opendir");
        return -1;
    }

    while ((d = readdir(countriesDir)) != NULL) {
        if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
            continue;
        counter++;
    }

    if (closedir(countriesDir) == -1) {
        perror("closedir failed!");
        return -1;
    }

    return counter;
}