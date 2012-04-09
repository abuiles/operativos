#include "control_console.h"
#include "../../dbg.h"
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



typedef struct{
  char id[100];
  char path[100];
  char filename[100];
  char reincarnation[100];
  int tid;
} Job;


void *startProcessManager(void *pjob)
{
  Job *job = (Job *) pjob;
  int pid, status;
  int status2;

  pid = fork();

  if (pid == 0){
    log_info("Hey I will start a new process manager, thead %s", job->id);
    char *newenviron[] = { NULL };
    char filepath[100] = "--filepath=";
    char filename[100] = "--filename=";
    char reincarnation[100] = "--reencarnacion=";

    char *newargv[] = { "procesoctrl",
                        filepath,
                        filename,
                        reincarnation,
                        job->id };

    strcat(filepath, job->path);
    strcat(filename, job->filename);
    strcat(reincarnation, job->reincarnation);

    status = execve("bin/procesoctrl", newargv , newenviron);

    fprintf(stderr, "Proceso de control falló");
    fprintf(stderr, "Código de error es : %d\n", status);
    fprintf(stderr, "Error: %s\n", strerror(errno));

    exit(1);
  }else{
    waitpid(pid, &status2, WUNTRACED | WCONTINUED);
    fprintf(stdout, "I'm thread %d, will finish after managing %s\n", job->tid, job->id);
  }

  return 0;
}

int main(int argc, char *argv[]){
  Job jobs[] = { { "proceso", "/Users/adolfobuiles/universidad/operativos/yagod/suicidas/", "proceso", "10", -1 }, { "ProcesoSuicida", "suicidas/", "ProcesoSuicida", "1000", -1 }, { "proceso2", "/Users/adolfobuiles/universidad/operativos/yagod/suicidas/", "proceso", "3", -1 } };

  int jobCount =  sizeof(jobs) / sizeof(Job);
  int i, rc, status;
  pthread_t *threadsTable;
  threadsTable = (pthread_t *) malloc(sizeof(pthread_t) * jobCount);

  log_info("Starting threads");

  for (i = 0; i < jobCount; ++i){
    jobs[i].tid = i;
    rc = pthread_create((threadsTable + i), NULL, startProcessManager, (void *) &jobs[i]);
    assert(0 == rc);
  }

  for (i = 0; i < jobCount; ++i){
    rc = pthread_join(*(threadsTable + i), (void **) &status);
    assert(0 == rc);
  }

  return 0;
}

