#include "../include/pipes.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int createPipe(char* path, int pid, int flags, char* ending)
{
    char buffer[sizeof(pid_t)];
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
    strcat(pipeName, buffer);
    strcat(pipeName, ending);
    
    if (mkfifo(pipeName, flags) == -1) {
        perror("mkfifo failed!");
        return -1;
    }

    free(pipeName);
    return 0;
}

int openPipe(char* path, int pid, int flags, char* ending)
{
    int fileDesc;
    char buffer[sizeof(pid_t)];
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
    strcat(pipeName, buffer);
    strcat(pipeName, ending);
    
    if ((fileDesc = open(pipeName, flags)) == -1) {
        perror("open failed!");
        return -1;
    }

    free(pipeName);
    return fileDesc;
}