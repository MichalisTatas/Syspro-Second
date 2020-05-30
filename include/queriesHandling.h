#include <wordexp.h>
#include <sys/select.h>


#include "list.h"
#include "pipes.h"

int querieAnswer(workerInfoPtr, const char* querie, int, int);
void sendQuerie(workerInfoPtr, const char*, int, int);
int queriesHandler(workerInfoPtr,const char*, int);
int selectWorker(const workerInfoPtr, char*, char*);
void listCountriesFunction(workerInfoPtr);