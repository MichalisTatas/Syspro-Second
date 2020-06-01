#include "../include/workers.h"

volatile sig_atomic_t handlerNumber;

static void handler(int sig)
{
    handlerNumber = sig;
    printf("lalla \n");
}

int workersFunction(int bufferSize, char* inputDirectory)
{
    int pid = getpid();
    int readDesc, writeDesc;
    countryPtr countryList = NULL;
    signal(SIGUSR1, handler);
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;           // prob not needed
    sigemptyset(&sa.sa_mask);           // prob not needed
    sa.sa_sigaction = (void*)handler;
    char* msg;


    if ((readDesc = createPipe("pipes/", pid, O_RDONLY,"P2C")) == -1) {  //P2C parent writes to child
        perror("createPipe failed");
        return -1;
    }
    if ((writeDesc = createPipe("pipes/", pid, O_WRONLY,"C2P")) == -1) {  //C2P child writes to parent
        perror("createPipe failed");
        return -1;
    }

    while(true) {

        if ((sigaction(SIGUSR1, &sa, NULL) == -1) || (sigaction(SIGQUIT, &sa, NULL) == -1) || (sigaction(SIGINT, &sa, NULL) == -1)) {
            perror("sigaction failed!");
            return -1;
        }

        if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
            perror("msgComposer failed");
            return -1;
        }

        if (!strcmp(msg, "finished writing countries")) {
            free(msg);
            break;
        }
        else {
            if (addCountryInList(&countryList, msg) == -1) {
                perror("addCountryInList failed");
                return -1;
            }
            free(msg);
        }
    }

    HashTablePtr diseaseHashtable,countryHashtable;
    patientPtr patientListHead = NULL;
    if ((diseaseHashtable = HTCreate(10)) == NULL) {
        perror("HTCreate failed");
        return -1;
    }
    if ((countryHashtable = HTCreate(10)) == NULL) {
        perror("HTCreate failed");
        return -1;
    }

    if (setDataStructures(&diseaseHashtable, &countryHashtable, countryList, &patientListHead, inputDirectory) == -1) {
        perror("setDataStructures");
        return -1;
    }

    // patientPtr c = patientListHead;
    // while(c!= NULL) {
    //         printf("ENTER %d %d %d \n", c->entryDate->day, c->entryDate->month, c->entryDate->year);
    //         if(c->exitDate != NULL)
    //         printf("LOLOLOLOLOLLOLOLLLLLLLLLLLLLLOLOLOLOLLOLLO%d %d %d \n", c->exitDate->day, c->exitDate->month, c->exitDate->year);
    //     c=c->next;
    // }


    //finished filling data structures ready for queries
    if (msgDecomposer(writeDesc, "finished!", bufferSize) == -1) {
        perror("msgDecomposer failed");
        return -1;
    }

    while (true){
        if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
            perror("msgComposer failed");
            return -1;
        }
        if (queriesAnswerer(msg, bufferSize, countryHashtable, diseaseHashtable, countryList, patientListHead, writeDesc, readDesc) == 1)
            break;
            
        free(msg);
    }

    free(msg);                   //its here because of break in exit for the time being
    destroyPatientList(patientListHead);
    HTDestroy(diseaseHashtable);
    HTDestroy(countryHashtable);
    destroyCountryList(countryList);
    return 0;
}

int setDataStructures(HashTablePtr* diseaseHashtable,HashTablePtr* countryHashtable, countryPtr countryList, patientPtr* patientListHead,  char* inputDirectory)
{
    // open each country file and fill the needed data structures
    DIR* countryDir;
    struct dirent* d;
    countryPtr country = countryList;
    FILE* filePtr;
    char* countryPath;
    char* datePath;
    char* line = NULL;
    size_t len = 0;
    patientPtr currentPatient = NULL;

    while (country != NULL) {

        countryPath = malloc(strlen(inputDirectory) + strlen(country->name) + 1);
        strcpy(countryPath, inputDirectory);
        strcat(countryPath, country->name);

        if ((countryDir = opendir(countryPath)) == NULL) {
            perror("opendir failed");
            return -1;
        }

        countryPtr dateList = NULL;
        while ((d = readdir(countryDir)) != NULL) {
            if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
                continue;
            if ((dateList = sortedAddDateInList(dateList, d->d_name)) == NULL) {
                perror("sortedAddDateInList failed");
                return -1;
            }
        }

        ///////////////////////////////////////////////////////////////
        // should first sort the list and check for valid entries !! //
        ///////////////////////////////////////////////////////////////

        countryPtr date = dateList;
        while (date != NULL) {
            datePath = malloc(strlen(countryPath) + strlen(date->name) + 2);
            strcpy(datePath, countryPath);
            strcat(datePath, "/");       //        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            strcat(datePath, date->name);
            if ((filePtr = fopen(datePath, "r")) == NULL)   {
                perror("fopen failed");
                return -1;
            }

            while (getline(&line, &len, filePtr) != -1) {
                line = strtok(line, "\n");
                if ((currentPatient = createPatientStruct(line, country->name, date->name)) == NULL) {
                    perror("createPatientStruct failed");
                    return -1;
                }
                if (canInsertPatient(*patientListHead, currentPatient)) {
                    if(!strcmp(currentPatient->action, "ENTER")) {
                        *patientListHead = patientListInsert(*patientListHead, currentPatient);
                        if (HTInsert(*diseaseHashtable, currentPatient->diseaseID, currentPatient) == -1) {
                        perror("HTInsert failed");
                        return -1;
                        }
                        if (HTInsert(*countryHashtable, currentPatient->country, currentPatient) == -1) {
                            perror("HTInsert failed");
                            return -1;
                        }
                    }
                    else {
                        *patientListHead = replaceExitDate(*patientListHead, currentPatient->recordID, date->name);
                        destroyPatientList(currentPatient);
                    }
                    
                }
                else
                    destroyPatientList(currentPatient);
            }

            if (fclose(filePtr) == EOF) {
                perror("fclose failed");
                return -1;
            }
            free(datePath);
            date = date->next;
        }

        if (closedir(countryDir) == -1) {
            perror("closedir failed!");
            return -1;
        }

        destroyCountryList(dateList);

        free(countryPath);
        country = country->next;
    }

    free(line);
    return 0;
}