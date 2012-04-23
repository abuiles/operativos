#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define True 1
#define False 0

void *handleSTDOUT(void *file);
void *handleSTDERR(void *file);
int parseArgs(int argc, char *argv[], char *args[]);
int handleProcess( int argc, char *argv[]);
