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
            if (workersFunction(bufferSize) == -1) {   //maybe use exec to do this
                perror("workersFunction failed");
                return -1;
            }
            free(inputDirectory);
            destroyList(workersList);
            exit(0);
        }
        else
            if ((workersList = addPidInList(workersList, pid)) == NULL) {
                perror("addPidInList failed");
                return -1;
            }
    }

    if (diseaseAggregatorApp(workersList, numWorkers, bufferSize) == -1) {
        perror("diseaseAggregatorApp failed");
        return -1;
    }
    
    for (int i=0; i<numWorkers; i++)
        wait(NULL);    
    return 0;
}

int diseaseAggregatorApp(workerInfoPtr workersList, int numWorkers, int bufferSize)
{
    //open all the pipes and store countries in list and sent countries to workers
    DIR* countriesDir;
    struct dirent* d;

    if ((countriesDir = opendir("./bashScript/dir")) == NULL) {
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
                perror("createPipe failed");
                return -1;
            }
            if ((iterator->read = createPipe("pipes/", iterator->pid, O_RDONLY,"C2P")) == -1) {  //C2P child writes to parent
                perror("createPipe failed");
                return -1;
            }
        }

                     //check it out sto countries list exw mia parapanw grammi gia to script pou den tou areseio edw profanwes
        if (msgDecomposer(iterator->write, d->d_name, bufferSize) == -1) {
            perror("msgDecomposer failed");
            return -1;
        }

        if (addCountryInList(&iterator->countriesList, d->d_name) == -1) {
            perror("addCountryInList failed");
            return -1;
        }

        iterator = iterator->next;
        if (iterator == NULL)
            iterator = workersList;
    }

    iterator = workersList;
    for (int i=0; i<numWorkers; i++) {
        if (msgDecomposer(iterator->write, "finished writing countries", bufferSize) == -1) {
            perror("msgDecomposer failed");
            return -1;
        }
        iterator = iterator->next;
    }

    if (closedir(countriesDir) == -1) {
        perror("closedir failed!");
        return -1;
    }

    fd_set readfds;
    char* msg;

    // makes sure every worker finishes reading and storing data
    // before taking queries from user

    while(true) {
        // parent read process from workers
        FD_ZERO(&readfds);
        iterator = workersList;
        int max = 0;
        while (iterator != NULL) {
            FD_SET(iterator->read, &readfds);
            if (iterator->read > max)
                max = iterator->read;
            iterator = iterator->next;
        }

        if (pselect(max + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
            perror("pselect failed!");
            return -1;
        }

        iterator = workersList;
        while (iterator != NULL) {
            if (FD_ISSET(iterator->read, &readfds)) {

                if ((msg = msgComposer(iterator->read, bufferSize)) == NULL) {
                    perror("msgComposer failed");
                    return -1;
                }
                
                if (!strcmp(msg, "finished!")) {
                    iterator->readyForWork = true;
                    printf("epa8e trikimia %s \n", msg);
                    free(msg);
                }
            }
            iterator = iterator->next;
        }

        iterator = workersList;
        bool allReady=true;
        while (iterator != NULL) {
            if (iterator->readyForWork == false)
                allReady = false;
            iterator = iterator->next;
        }
        if (allReady)
            break;
    }

    // start taking queries as input 

    destroyList(workersList);
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