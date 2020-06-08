#include "../include/diseaseAggregator.h"

volatile sig_atomic_t parentSignalHandlerNumber;


void parentSignalHandler(int sig) {  //change global variable and do things brother
    parentSignalHandlerNumber = sig;
}

int forkAssignFunctionality(int bufferSize, int numWorkers, char* inputDirectory)
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
            if (workersFunction(bufferSize, inputDirectory) == -1) {   //maybe use exec to do this
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

    if (diseaseAggregator(workersList, numWorkers, bufferSize, inputDirectory) == -1) {
        perror("diseaseAggregator failed");
        return -1;
    }
    
    for (int i=0; i<numWorkers; i++)
        wait(NULL);

    return 0;
}



int diseaseAggregator(workerInfoPtr workersList, int numWorkers, int bufferSize, char* inputDirectory)
{
    fd_set readfds;
    sigset_t emptyset, blockset;
    signal(SIGINT, parentSignalHandler);
    signal(SIGQUIT, parentSignalHandler);
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGINT);
    sigaddset(&blockset, SIGQUIT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);

    char* querie = NULL;
    size_t len = 0;
    signal(SIGCHLD, parentSignalHandler);
    if (distributeCountries(&workersList, numWorkers, bufferSize, inputDirectory) == -1) {
        perror("distributeCountries failed");
        return -1;
    }

    if (areWorkersReady(&workersList, bufferSize)) {
        while (true) {
                FD_ZERO(&readfds);
                FD_SET(0, &readfds);

                sigemptyset(&emptyset);
                if (pselect(1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
                    if (errno == EINTR) {
                        if (parentSignalHandlerNumber == SIGCHLD) {
                            workersList = handleChildDeath(workersList, bufferSize, inputDirectory);
                        }
                        else if (parentSignalHandlerNumber == SIGINT ) {
                            parentSIGINThandler(workersList);
                            break;
                        }
                        else if (parentSignalHandlerNumber == SIGQUIT)  {
                            parentSIGQUIThandler(workersList);
                            break;
                        }
                    }
                    else if (errno != EINTR) {
                        perror("pselect failed!");
                        return -1;
                    }
                }

                if (FD_ISSET(0, &readfds))  {                    // if input from user occured
                    if (getline(&querie, &len, stdin) == -1) {
                        perror("getline failed");
                        return -1;
                    }
                    querie = strtok(querie, "\n");
                    if (queriesHandler(workersList, querie, bufferSize) == -1) {
                        break;
                    }
                }
        }
    }

    workerInfoPtr temp = workersList;
    while (temp != NULL) {
        if (closePipe("pipes/", temp->pid, "P2C") == -1) {
            perror("closePipe failed");
            return -1;
        }
        if (closePipe("pipes/", temp->pid, "C2P") == -1) {
            perror("closePipe failed");
            return -1;
        }
        temp = temp->next;
    }
    free(querie);
    destroyList(workersList);
    return 0;
}


workerInfoPtr handleChildDeath(workerInfoPtr workersList, int bufferSize, char* inputDirectory)
{
    sigset_t emptyset;
    pid_t pid;
    pid = wait(NULL);
    workerInfoPtr temp = workersList;
    while(temp->pid != pid)
        temp = temp->next;
    closePipe("pipes/", temp->pid, "C2P");
    closePipe("pipes/", temp->pid, "P2C");
    
    pid = fork();

    if (pid == -1) {
        perror("fork failed!\n");
        return NULL;
    }
    else if (pid == 0) {
        if (workersFunction(bufferSize, inputDirectory) == -1) {
            perror("workersFunction failed");
            return NULL;
        }
        free(inputDirectory);
        destroyList(workersList);
        exit(0);
    }
    else
        temp->pid = pid;

    if ((temp->write = createPipe("pipes/", temp->pid, O_WRONLY,"P2C")) == -1) {  //P2C parent writes to child
        perror("createPipe failed");
        return NULL;
    }
    if ((temp->read = createPipe("pipes/", temp->pid, O_RDONLY,"C2P")) == -1) {  //C2P child writes to parent
        perror("createPipe failed");
        return NULL;
    }

    countryPtr country = temp->countriesList;
    while (country != NULL) {
        if (msgDecomposer(temp->write, country->name, bufferSize) == -1) {
            perror("msgDecomposer failed");
            return NULL;
        }
        country = country->next;
    }

    if (msgDecomposer(temp->write, "finished writing countries", bufferSize) == -1) {
            perror("msgDecomposer failed");
            return NULL;
    }


    fd_set readfds;
    char* msg;
    FILE* filePtr = fopen("newStatistics.txt", "w");
    while(true) {
        FD_ZERO(&readfds);
        FD_SET(temp->read, &readfds);

        sigemptyset(&emptyset);
        if (pselect(temp->read + 1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
            perror("pselect failed!");
            return NULL;
        }

        if (FD_ISSET(temp->read, &readfds)) {

            if ((msg = msgComposer(temp->read, bufferSize)) == NULL) {
                perror("msgComposer failed");
                return NULL;
            }
            
            if (!strcmp(msg, "finished!")) {
                temp->readyForWork = true;
                free(msg);
                break;
            }
            else  {
                fputs(msg, filePtr);
            free(msg);
            }

        }
    }
    fclose(filePtr);

    return workersList;
}

void parentSIGINThandler(workerInfoPtr workersList)
{
    workerInfoPtr temp = workersList;
    while (temp!=NULL) {
        kill(temp->pid, SIGINT);
        temp = temp->next;
    }
}

void parentSIGQUIThandler(workerInfoPtr workersList)
{
    workerInfoPtr temp = workersList;
    while (temp!=NULL) {
        kill(temp->pid, SIGQUIT);
        temp = temp->next;
    }
}

int distributeCountries(workerInfoPtr* workersList, int numWorkers, int bufferSize, char* inputDirectory)
{
    DIR* countriesDir;
    struct dirent* d;

    if ((countriesDir = opendir(inputDirectory)) == NULL) {
        perror("opendir");
        return -1;
    }

    workerInfoPtr iterator = *workersList;
    // in this while i distribute the countries to each child open the pipes
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

        // send countries to workers
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
            iterator = *workersList;
    }

    iterator = *workersList;
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

    return 0;
}


bool areWorkersReady(workerInfoPtr* workersList, int bufferSize)
{
    fd_set readfds;
    char* msg;
    workerInfoPtr iterator;
    FILE* filePtr = fopen("statistics.txt", "w");
    while(true) {
        // parent read process from workers
        FD_ZERO(&readfds);
        iterator = *workersList;
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

        iterator = *workersList;
        while (iterator != NULL) {
            if (FD_ISSET(iterator->read, &readfds)) {

                if ((msg = msgComposer(iterator->read, bufferSize)) == NULL) {
                    perror("msgComposer failed");
                    return -1;
                }
                
                if (!strcmp(msg, "finished!")) {
                    iterator->readyForWork = true;
                    free(msg);
                }
                else  {
                    fputs(msg, filePtr);
                free(msg);
                }
            }
            iterator = iterator->next;
        }

        iterator = *workersList;
        bool allReady=true;
        while (iterator != NULL) {
            if (iterator->readyForWork == false)
                allReady = false;
            iterator = iterator->next;
        }
        if (allReady) {
            fclose(filePtr);
            return true;
        }
    }
    return false;
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