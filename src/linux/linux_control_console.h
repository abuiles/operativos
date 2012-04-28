#include "../common/types.h"
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

void *handleSTDOUT(void *file);
void *handleSTDERR(void *file);
void *startProcessManager(void *pjob);
int readConfigFile(char *configFile, Job jobs[]);
int startJobs(int jobCount, Job jobs[]);
