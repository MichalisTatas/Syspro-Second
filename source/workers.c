#include "../include/workers.h"

volatile sig_atomic_t handlerNumber;

#define bucketSize 24

static void handler(int sig)
{
    handlerNumber = sig;
    printf("lalla \n");
}

int workersFunction(int bufferSize)
{
    int pid = getpid();
    int readDesc, writeDesc;
    countryPtr countryList = NULL;

    if ((readDesc = createPipe("pipes/", pid, O_RDONLY,"P2C")) == -1) {  //P2C parent writes to child
        printf("Error opening or creating pipe! \n");
        return -1;
    }
    if ((writeDesc = createPipe("pipes/", pid, O_WRONLY,"C2P")) == -1) {  //C2P child writes to parent
        printf("Error opening or creating pipe! \n");
        return -1;
    }

    signal(SIGUSR1, handler);
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;           // prob not needed
    sigemptyset(&sa.sa_mask);           // prob not needed
    sa.sa_sigaction = (void*)handler;
    char* msg;

    while(true) {

        if ((sigaction(SIGUSR1, &sa, NULL) == -1) || (sigaction(SIGQUIT, &sa, NULL) == -1) || (sigaction(SIGINT, &sa, NULL) == -1)) {
            perror("sigaction failed!");
            return -1;
        }

        if (!strcmp((msg = msgComposer(readDesc, bufferSize)), "finished writing countries")) {
            free(msg);
            break;
        }
        else {
            addCountryInList(&countryList, msg);
            free(msg);
        }
    }

    HashTablePtr diseaseHashtable = HTCreate(10, bucketSize);
    HashTablePtr countryHashtable = HTCreate(10, bucketSize);

    if (setDataStructures(&diseaseHashtable, &countryHashtable, countryList) == -1) {
        perror("setDataStructures");
        return -1;
    }

    HTPrint(diseaseHashtable);
    HTPrint(countryHashtable);

    //finished filling data structures ready for queries
    msgDecomposer(writeDesc, "finished!", bufferSize);

    HTDestroy(diseaseHashtable);
    HTDestroy(countryHashtable);
    destroyCountryList(countryList);
    return 0;
}

int setDataStructures(HashTablePtr* diseaseHashtable,HashTablePtr* countryHashtable,countryPtr countryList)
{
    // open each country file and fill the needed data structures
    DIR* countryDir;
    struct dirent* d;
    char* path = "./bashScript/dir/";
    countryPtr country = countryList;
    FILE* filePtr;
    char* countryPath;
    char* datePath;
    char* line = NULL;
    size_t len = 0;
    patientPtr currentPatient = NULL;
    patientPtr patientListHead = NULL;

    while (country != NULL) {

        countryPath = malloc(strlen(path) + strlen(country->name) + 1);
        strcpy(countryPath, path);
        strcat(countryPath, country->name);

        if ((countryDir = opendir(countryPath)) == NULL) {
            perror("opendir");
            return -1;
        }

        countryPtr dateList = NULL;
        while ((d = readdir(countryDir)) != NULL) {
            if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
                continue;
            addCountryInList(&dateList, d->d_name);
        }

        ///////////////////////////////////////////////////////////////
        // should first sort the list and check for valid entries !! //
        ///////////////////////////////////////////////////////////////

        countryPtr date = dateList;
        while (date != NULL) {
            datePath = malloc(strlen(countryPath) + strlen(date->name) + 2);
            strcpy(datePath, countryPath);
            strcat(datePath, "/");
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
                destroyPatientList(currentPatient);
                // patientListHead = patientListInsert(patientListHead, currentPatient);
                HTInsert(*diseaseHashtable, currentPatient->diseaseID, currentPatient);
                // HTInsert(*countryHashtable, currentPatient->country, currentPatient);
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

    // destroyPatientList(patientListHead);
    free(line);
    return 0;
}