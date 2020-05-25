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
    int msgSize = strlen(msg)+1;
    char numBuff[12] = {0};
    sprintf(numBuff, "%d", msgSize);
    
    for (int i=0; i<12/bufferSize; i++) {
        write(fileDescriptor,  numBuff + i*bufferSize, bufferSize);
    }

    if (12 % bufferSize)
        write(fileDescriptor, numBuff + (12 - 12 % bufferSize), bufferSize);

    for (int i=0; i<msgSize/bufferSize; i++)
        write(fileDescriptor, msg + i*bufferSize , bufferSize);

    if (msgSize % bufferSize)
        write(fileDescriptor, msg + ( msgSize - msgSize % bufferSize) , bufferSize);

}

char* msgComposer(int fileDescriptor, int bufferSize)
{
    char msgSizeArray[12] = {0};
    char* buffer[bufferSize];
    char* msg;

    for (int i=0; i<12/bufferSize; i++) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msgSizeArray + i*bufferSize, buffer, bufferSize); //bufferSize?
    }

    if ((12 % bufferSize)) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msgSizeArray + bufferSize * (12 / bufferSize), buffer, 12 - bufferSize * (12 / bufferSize));
    }

    int msgSize = strtol(msgSizeArray, NULL, 10);

    // printf("msgSize %d\n", msgSize);

    if ((msg = malloc(msgSize)) == NULL) {      // +1 ???
        perror("malloc failed!");
        return NULL;
    }

    for (int i=0; i<msgSize/bufferSize; i++) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msg + i*bufferSize, buffer, bufferSize);
    }

    if (msgSize % bufferSize) {
        read(fileDescriptor, buffer, bufferSize);
        memcpy(msg + bufferSize*(msgSize / bufferSize), buffer, msgSize - bufferSize*(msgSize / bufferSize));
    }

    return msg;
}






