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
        else if (pid == 0) {
            if (workersFunction() == -1) {   //maybe use exec to do this
                printf("Error in workers Function\n");
                return -1;
            }
            free(inputDirectory);
            destroyList(workersList, numWorkers, countriesNum);
            exit(0);
        }
        else
            workersList = addPidInList(workersList, pid);
    }

    if (diseaseAggregatorApp(workersList, numWorkers, countriesNum) == -1) {
        printf("Error occurred in diseaseAggregatorApp!\n");
        return -1;
    }
    
    for (int i=0; i<numWorkers; i++)
        wait(NULL);    
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
    // // in this while i distribute the countries to each child open the pipes
    while ((d = readdir(countriesDir)) != NULL) {

        if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
            continue;

        if (iterator->read == -1 || iterator->write == -1) {
            if ((iterator->write = createPipe("pipes/", iterator->pid, O_WRONLY,"P2C")) == -1) {  //P2C parent writes to child
                printf("Error opening or creating pipe! \n");
                return -1;
            }
            if ((iterator->read = createPipe("pipes/", iterator->pid, O_RDONLY,"C2P")) == -1) {  //C2P child writes to parent
                printf("Error opening or creating pipe! \n");
                return -1;
            }
        }

        addCountryInList(iterator, d->d_name);

        iterator = iterator->next;
        if (iterator == NULL)
            iterator = workersList;
    }

    if (closedir(countriesDir) == -1) {
        perror("closedir failed!");
        return -1;
    }

    //while(true){
    //     read write
    // }
    destroyList(workersList, numWorkers, countriesNum);
    return 0;
}

int countriesNumber(char* countriesDirName)
{
    //shoiuld give name of dir and strcat etc
    DIR* countriesDir;
    struct dirent* d;
    int counter=0;

    if ((countriesDir = opendir(countriesDirName)) == NULL) {
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