#include "pipes.h"
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <signal.h>

#include "list.h"


int workersFunction(int);