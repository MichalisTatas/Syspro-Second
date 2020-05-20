#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../include/pipes.h"

int main(int argc, char* argv[])
{
    int numWorkers, bufferSize;
    char* input_dir;

    if (argc != 7 ) {
        printf("Wrong number of arguments exactly 7 is required! \n");
        return -1;
    }

    for (int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "-w")) {
            if ((numWorkers = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or numWorkers argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-b")) {
            if ((bufferSize = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-i")) {
            if ((input_dir = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                printf("Malloc failed to allocate space!\n");
                return -1;
                //perror??
            }
            strcpy(input_dir, argv[i+1]);
        }
    }

    // DIR* countriesDir;

    // if ((countriesDir = opendir("bashScript/dir")) == NULL) {
    //     perror("opendir");
    //     return -1;
    // }

    // struct dirent* d;
    // int counter=0;
    // while ((d = readdir(countriesDir)) != NULL) {
    //     if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
    //         continue;
    //     counter++;
    // }

    pid_t pid;
    for (int i=0; i<numWorkers; i++) {\
        pid = fork();
        if (pid == -1) {
            perror("fork failed!\n");
            return -1;
        }
        if (pid == 0)
            break;
    }

    if (pid != 0) {
        printf("i am in parent process ie disease aggregator! \n");
    }
    else {
        if (createPipe("pipes/", getpid()) == -1) {
            printf("error in createPipe! \n");
            return -1;
        }
    }

    free(input_dir);
    return 0;
}