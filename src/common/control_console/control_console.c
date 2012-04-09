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

void *handleSTDOUT(void *file){
  FILE *stream;
  int c;
  int *fid = (int *) file;
  stream = fdopen (*(fid), "r");

  while((c = fgetc (stream)) != EOF)
    putchar (c);
  fclose (stream);
  return (void *) NULL;
}

void *startProcessManager(void *pjob)
{

  Job *job = (Job *) pjob;
  int pid, status, status2, rc;
  int threadsNum = 1;

  pthread_t *table;
  int outfd[2];

  if (pipe(outfd) == -1) {
    perror("pipe");
    return (void *) -1;
  }

  pid = fork();

  if (pid == 0){
    close(outfd[0]);

    dup2(outfd[1], 1);
    close(outfd[1]);

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
    close(outfd[1]);

    table = (pthread_t *) malloc(sizeof(pthread_t) * threadsNum);

    rc = pthread_create((table + 0), NULL, handleSTDOUT, (void *) &outfd[0]);
    assert(0 == rc);

    waitpid(pid, &status2, WUNTRACED | WCONTINUED);
  }

  return 0;
}

int main(int argc, char *argv[]){
  Job jobs[] = { { "proceso", "/Users/adolfobuiles/universidad/operativos/yagod/suicidas/", "proceso", "3", -1 }, { "ProcesoSuicida", "suicidas/", "ProcesoSuicida", "10", -1 }, { "proceso2", "/Users/adolfobuiles/universidad/operativos/yagod/suicidas/", "proceso", "5", -1 } };

  int jobCount =  sizeof(jobs) / sizeof(Job);
  int i, rc, status;
  pthread_t *threadsTable;
  threadsTable = (pthread_t *) malloc(sizeof(pthread_t) * jobCount);

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

