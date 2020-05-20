#include "../include/pipes.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int createPipe(char* path, int pid)
{
    char buffer[sizeof(pid_t)];
    if (sprintf(buffer, "%d", pid) <= 0) {
        perror("sprintf failed!");
        return -1;
    }

    // pipe where child writes to parent
    char* pipeNameC2P;
    if ((pipeNameC2P = malloc(strlen(buffer) + strlen(path) + 4)) == NULL) {
        perror("malloc failed!");
        return -1;
    }

    strcpy(pipeNameC2P, path);
    strcat(pipeNameC2P, buffer);
    strcat(pipeNameC2P, "C2P");
    
    if (mkfifo(pipeNameC2P, 0666) == -1) {
        if (errno != EEXIST) {
        perror("mkfifo failed!");
        return -1;
        }
    }

    // pipe where parent writes to child
    char* pipeNameP2C;
    if ((pipeNameP2C = malloc(strlen(buffer) + strlen(path) + 4)) == NULL) {
        perror("malloc failed!");
        return -1;
    }

    strcpy(pipeNameP2C, path);
    strcat(pipeNameP2C, buffer);
    strcat(pipeNameP2C, "P2C");

    if (mkfifo(pipeNameP2C, 0666) == -1) {
        if (errno != EEXIST) {
        perror("mkfifo failed!");
        return -1;
        }
    }


    // should open both pipes form childs perspective
    return 0;
}