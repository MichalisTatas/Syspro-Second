#include "../include/workers.h"

int workersFunction(int pid)
{

    // 0666 or?>??
    int readDesc, writeDesc;
    if (createPipe("pipes/", pid, 0666,"P2C") == -1) {  //P2C parent writes to child
        printf("Error creating pipe! \n");
        return -1;
    }
    if (createPipe("pipes/", pid, 0666,"C2P") == -1) {  //C2P child writes to parent
        printf("Error creating pipe! \n");
        return -1;
    }
    if ((readDesc = openPipe("pipes/", pid, O_RDONLY,"P2C")) == -1) {  //P2C parent writes to child
        printf("Error creating pipe! \n");
        return -1;
    }
    if ((writeDesc = openPipe("pipes/", pid, O_WRONLY,"C2P")) == -1) {  //C2P child writes to parent
        printf("Error creating pipe! \n");
        return -1;
    }

    // while (true) {
    //     //try to read bro i guess
    // }
    return 0;
}