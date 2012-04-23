#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <windows.h>
#include "../common/types.h"

DWORD WINAPI handleSTDOUT(LPVOID file);
DWORD WINAPI handleSTDERR(LPVOID file);
DWORD WINAPI startProcessManager(LPVOID pjob);
int readConfigFile(char *configFile, Job jobs[]);
int startJobs(int jobCount, Job jobs[]);
