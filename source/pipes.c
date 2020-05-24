#include "../include/pipes.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int createPipe(char* path, int pid, int flags, char* ending)
{
    char buffer[12];
    if (sprintf(buffer, "%d", pid) <= 0) {
        perror("sprintf failed!");
        return -1;
    }

    char* pipeName;
    if ((pipeName = malloc(strlen(buffer) + strlen(path) + 4)) == NULL) {
        perror("malloc failed!");
        return -1;
    }

    strcpy(pipeName, path);
    strcat(pipeName, ending);
    strcat(pipeName, buffer);
    
    printf("%s \n\n",pipeName);

    if (mkfifo(pipeName, 0666) == -1) {
        if (errno != EEXIST) {    
            perror("mkfifo failed!");
            return -1;
        }
    }

    int fileDesc;
    if ((fileDesc = open(pipeName, flags)) == -1) {
        perror("open failed!");
        return -1;
    }

    free(pipeName);
    return fileDesc;
}

void msgDecomposer(int fileDescriptor, char* msg, int bufferSize)
{
    int repetitions;
    int msgSize = strlen(msg) + 1;
    char numBuff[12] = {0};
    sprintf(numBuff, "%d", msgSize);
 
    if (12 % bufferSize == 0)
        repetitions = 12 / bufferSize;
    else
        repetitions = 12 / bufferSize + 1;

    for (int i=0; i<repetitions; i++) {
        write(fileDescriptor,  numBuff + i*repetitions, bufferSize);
    }


    if ((msgSize % bufferSize) == 0)
        repetitions = msgSize / bufferSize;
    else 
        repetitions = msgSize / bufferSize + 1;

    for (int i=0; i<repetitions; i++) {
        write(fileDescriptor, msg + i*bufferSize , bufferSize);
    }
}

char* msgComposer(int fileDescriptor, int bufferSize)
{
    int repetitions;
    char msgSizeArray[12] = {0};
    char* buffer[bufferSize];
    char* msg;

    if (12 % bufferSize == 0)
        repetitions = 12 / bufferSize;
    else
        repetitions = 12 / bufferSize + 1;

    for (int i=0; i<repetitions; i++) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msgSizeArray + i*bufferSize, buffer, bufferSize); //bufferSize?
    }

    int msgSize = strtol(msgSizeArray, NULL, 10);

    printf("msgSize %d\n", msgSize);

    if ((msg = malloc(msgSize)) == NULL) {
        perror("malloc failed!");
        return NULL;
    }

    if ((msgSize % bufferSize) == 0)
        repetitions = msgSize / bufferSize;
    else 
        repetitions = msgSize / bufferSize + 1;

    for (int i=0; i<repetitions; i++) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msg + i*bufferSize, buffer, bufferSize);
    }

    return msg;
}






