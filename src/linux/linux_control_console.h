#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "../common/types.h"

void *handleSTDOUT(void *file);
void *startProcessManager(void *pjob);
int readConfigFile(char *configFile, Job jobs[]);
int startJobs(int jobCount, Job jobs[]);
