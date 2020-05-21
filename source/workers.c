#include "../include/workers.h"

int workersFunction(int pid)
{
    int read, write;
    if ((read = createPipe("pipes/", pid, O_RDONLY,"P2C")) == -1) {  //P2C parent writes to child
        printf("Error creating pipe! \n");
        return -1;
    }
    if ((write = createPipe("pipes/", pid, O_WRONLY,"C2P")) == -1) {  //C2P child writes to parent
        printf("Error creating pipe! \n");
        return -1;
    }

    // while (true) {
    //     //try to read bro i guess
    // }
    return 0;
}