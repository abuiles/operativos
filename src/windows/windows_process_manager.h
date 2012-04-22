#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <windows.h>

#define True 1
#define False 0

int parseArgs(int argc, char *argv[], char *args[]);
int handleProcess( int argc, char *argv[]);
