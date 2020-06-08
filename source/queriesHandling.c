#include "../include/queriesHandling.h"

void sendQuerie(workerInfoPtr workersList, const char* querie, int writeDesc, int bufferSize)
{
    char* temp = malloc(strlen(querie) + 1);
    strcpy(temp, querie);
    workerInfoPtr iterator = workersList;

    if (iterator == NULL)                        // if querie needs to be sent to one worker
        msgDecomposer(writeDesc, temp, bufferSize);
    else {                                          // if querie needs to be sent to all workers
        while (iterator != NULL) {
            msgDecomposer(iterator->write, temp, bufferSize);
            iterator->readyForWork = false;
            iterator = iterator->next;
        }
    }
    free(temp);
}

int querieAnswer(workerInfoPtr workersList, const char* querie, int readDesc, int bufferSize)
{
    sigset_t emptyset, blockset;
    fd_set readfds;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGUSR1);
    sigaddset(&blockset, SIGINT);
    sigaddset(&blockset, SIGQUIT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);
    char* msg;
    workerInfoPtr iterator = workersList;
    int diseaseFrequency = 0;

    if (iterator != NULL) {
        while(true) {
            FD_ZERO(&readfds);
            iterator = workersList;
            int max = 0;
            while (iterator != NULL) {
                FD_SET(iterator->read, &readfds);
                if (iterator->read > max)
                    max = iterator->read;
                iterator = iterator->next;
            }

            sigemptyset(&emptyset);
            if (pselect(max + 1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
                if (errno != EINTR) {
                    perror("pselect failed!");
                    return -1;
                }
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
                        free(msg);
                        break;
                    }

                    if (!strcmp(querie, "/diseaseFrequency"))
                        diseaseFrequency =  diseaseFrequency + atoi(msg);
                    else
                        printf("%s \n", msg);
                    free(msg);
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
            if (allReady) {
                if (!strcmp(querie, "/diseaseFrequency"))
                    printf("%d\n", diseaseFrequency);
                return 0;
            }
        }
    }
    else {
        while (true) {
            if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
                perror("msgComposer failed");
                return -1;
            }

            if (!strcmp(msg, "finished!")) {
                free(msg);
                return 0;
            }

            printf("%s \n", msg);
            free(msg);
        }
    }

    return -1;
}

int queriesHandler(workerInfoPtr workersList,const char* querie, int bufferSize)
{
    wordexp_t p;
    wordexp(querie, &p, 0);

    if (!strcmp(p.we_wordv[0], "/listCountries")) {
        if (p.we_wordc != 1) {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
        listCountriesFunction(workersList);
    }
    else if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            querieAnswer(workersList, p.we_wordv[0], -1, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4], "write"), bufferSize);
            querieAnswer(NULL, p.we_wordv[0], selectWorker(workersList, p.we_wordv[4], "read"), bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
        if (p.we_wordc == 6) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[2], "write"), bufferSize);
            querieAnswer(NULL, querie, selectWorker(workersList, p.we_wordv[2], "read"), bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
        if (p.we_wordc == 2) {
            sendQuerie(workersList, querie, -1, bufferSize);
            querieAnswer(workersList, querie, -1, bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            querieAnswer(workersList, querie, -1, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4], "write"), bufferSize);
            querieAnswer(NULL, querie, selectWorker(workersList, p.we_wordv[4], "read"), bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            querieAnswer(workersList, querie, -1, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4], "write"), bufferSize);
            querieAnswer(NULL, querie, selectWorker(workersList, p.we_wordv[4], "read"), bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/exit")) {
        if (p.we_wordc != 1) {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
        sendQuerie(workersList, querie, -1, bufferSize);
        // querieAnswer(workersList, querie, -1, bufferSize);
        wordfree(&p);
        return -1;
    }
    else {
        printf("wrong querie ! \n");
        wordfree(&p);
    }

    wordfree(&p);
    return 0;
}

int selectWorker(const workerInfoPtr workersList, char* country, char* rdwr) // returns readDesc of worker which handles the country
{
    workerInfoPtr iterator = workersList;
    countryPtr countryIterator;
    while (iterator != NULL) {
        countryIterator = iterator->countriesList;
        while (countryIterator != NULL) {
            if (!strcmp(countryIterator->name, country)) {
                if (!strcmp(rdwr, "read"))
                    return iterator->read;
                else
                    return iterator->write;
            }
            countryIterator = countryIterator->next;
        }
        iterator = iterator->next;
    }
    return -1;  //???
}

void listCountriesFunction(workerInfoPtr workersList)
{
    workerInfoPtr iterator = workersList;
    countryPtr country;
    while(iterator != NULL) {
        country = iterator->countriesList;
        while (country != NULL) {
            printf("%s %d \n", country->name, iterator->pid);
            country = country->next;
        }
        iterator = iterator->next;
    }
}