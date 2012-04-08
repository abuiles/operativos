#include "dbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
/* debug("I'm a thread. With ID %d", valor); */
/* Crear un hilo */
/* Hacer fork desde el hilo */
/* Lanzar el nuevo programa */
/* dormir el hilo por 20 segundos y luego matar al programa */


void* threadTask(void *argument){
  intptr_t tid;

  tid = (intptr_t *) argument;

  pid_t pid;
  int status, status2;

  log_info("Hello World! It's me, thread %d!\n", (int) tid);

  if ((pid = fork()) ){
    log_info("I'm a thread and I'm a father of process %d", (int) pid);

    waitpid(pid, &status, WUNTRACED | WCONTINUED);

    log_info("Soon to finish! thread %d!\n", pid);
  } else {
    pid = getpid();
    log_info("Omg I'm a new process %d", pid);

    log_info("Ready to cast.");

    /* char buffer[10]; */
    /* snprintf(buffer, sizeof(buffer), "%d", tid); */


    char *newargv[] = { "proceso", malloc(sizeof(char *)), NULL };
    char *newenviron[] = { NULL };

    snprintf(newargv[1], sizeof(newargv[1]), "%d", pid);

    status2 = execve("./proceso", newargv , newenviron);

    perror("execve");

    log_err("Something went wrong return value was %d", status2);
    log_info("Omg I'm a new process finishing %d", pid);
  }

  return (void *) 0 ;
}

int main(int argc, char *argv[])
{

  int threadsNum = 0;
  pthread_t *table;
  int i, rc, response;

  if (argc !=2){
    errno = 2;
    log_err("You have to specify the number of threads.");
    exit(1);
  }

  log_info("Well done you pass the right parameters, let's keep going.");

  threadsNum = atoi(argv[1]);

  table = (pthread_t *) malloc(sizeof(pthread_t) * threadsNum);

  for (i = 0; i < threadsNum; ++i){
    rc = pthread_create((table + i), NULL, threadTask, (void *) i);
    assert(0 == rc);
  }

  for (i = 0; i < threadsNum; ++i){
    rc = pthread_join(*(table + i), (void **) &response);
    assert(0 == rc);
  }

  exit(1);
}

