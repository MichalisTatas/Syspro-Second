#include "../include/workers.h"

volatile sig_atomic_t handlerNumber;

static void handler(int sig)
{
    handlerNumber = sig;
    printf("lalla \n");
}

int workersFunction(int bufferSize)
{
    int pid = getpid();
    int readDesc, writeDesc;
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
    while (1) {
        if ((sigaction(SIGUSR1, &sa, NULL) == -1) || (sigaction(SIGQUIT, &sa, NULL) == -1) || (sigaction(SIGINT, &sa, NULL) == -1)) {
            perror("sigaction failed!");
            return -1;
        }
        
        msg = msgComposer(readDesc, bufferSize);
        printf("%s\n", msg);
        sleep(4);
    }
    // printf("ATTATAT\n");
    return 0;
}