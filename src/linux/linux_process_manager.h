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
#include "../common/types.h"

#define True 1
#define False 0

void *handleSTDOUT(void *file);
void *handleSTDERR(void *file);
int parseArgs(int argc, char *argv[], char *args[]);
int handleProcess( int argc, char *argv[]);
