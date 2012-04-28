#include "linux_control_console.h"

void *handleSTDOUT(void *file){
  FILE *stream;
  char temp[1024];
  int *fid = (int *) file;
  stream = fdopen (*(fid), "r");

  while(fgets(temp, 1024, stream) != EOF){
    fprintf(stdout, "%s", temp);
    fflush(stdout);
  }
  fclose (stream);
  return (void *) NULL;
}


void *handleSTDERR(void *file){
  FILE *stream;
  char temp[1024];
  int *fid = (int *) file;
  stream = fdopen (*(fid), "r");

  while(fgets(temp, 1024, stream) != EOF){
    fprintf(stderr, "%s", temp);
    fflush(stderr);
  }
  fclose (stream);
  return (void *) NULL;
}

void *startProcessManager(void *pjob)
{

  Job *job = (Job *) pjob;
  int pid, status, status2, rc;
  int threadsNum = 2;
  char tid [33];


  pthread_t *table;
  int outfd[2];
  int errfd[2];

  if (pipe(outfd) == -1) {
    perror("pipe");
    return (void *) -1;
  }

  if (pipe(errfd) == -1) {
    perror("pipe");
    return (void *) -1;
  }

  pid = fork();

  if (pid == 0){
    close(outfd[0]);
    dup2(outfd[1], 1);
    close(outfd[1]);

    close(errfd[0]);
    dup2(errfd[1], 2);
    close(errfd[1]);

    sprintf (tid, "%d", job->tid);

    char *newenviron[] = { NULL };
    char filepath[100] = "--filepath=";
    char filename[100] = "--filename=";
    char reincarnation[100] = "--reencarnacion=";

    char *newargv[] = { "procesoctrl",
                        filepath,
                        filename,
                        reincarnation,
                        "--idMemoria=123456",
                        "--idSemaforoMemoria=123456",
                        tid };

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

    rc = pthread_create((table + 1), NULL, handleSTDERR, (void *) &errfd[0]);
    assert(0 == rc);

    waitpid(pid, &status2, WUNTRACED | WCONTINUED);
  }

  return 0;
}

int readConfigFile(char *configFile, Job jobs[]){
  FILE *file;
  char c;

  file = fopen(configFile, "r");


  char entry[200];
  char *lines[512];

  int i,j;
  j = 0;
  int jobsCount = 0;



  c = fgetc(file);

  /* Get array of lines */
  while (c != EOF ){
    i = 0;
    while ( c != '\n' && c != EOF){
      entry[i] = c;
      c = fgetc(file);
      i ++;
    }
    entry[i] = '\0';

    lines[j] = malloc(sizeof(entry));
    strcpy(lines[j], entry);
    jobsCount ++;

    if (c != EOF)
      c = fgetc(file);

    j++;
  }


  int p =0;

  while(lines[p]){
    /* Parse lines to Jobs */
    i = 0;
    char *line = lines[p];
    char temp[512];

    j = 0;

    /* Skip until process id */
    while( line[j] != ' ')
      j++;

    j++;


    /* Get process id */
    i = 0;
    while( line[j] != ' '){
      temp[i] = line[j];
      i++;
      j++;
    }
    temp[i] = '\0';
    strcpy(jobs[p].id, temp);

    j = j + 3;

    i = 0;

    /* Get pathname  */
    while( line[j] != ' '){
      temp[i] = line[j];
      i++;
      j++;
    }
    temp[i] = '\0';
    strcpy(jobs[p].path, temp);

    j = j + 4;

    i = 0;

    /* Get process name  */
    while( line[j] != ' '){
      temp[i] = line[j];
      i++;
      j++;
    }
    temp[i] = '\0';
    strcpy(jobs[p].filename, temp);

    j = j + 1;

    i = 0;

    /* Get reincarnations */
    while( line[j] != ' '){
      temp[i] = line[j];
      i++;
      j++;
    }
    temp[i] = '\0';
    strcpy(jobs[p].reincarnation, temp);

    p++;
  }
  fclose(file);

  return jobsCount;
}


int startJobs(int jobCount, Job jobs[]){
  int i, rc, status;
  pthread_t *threadsTable;
  int idSeg;
  int idSem;
  int key = 123456;

  MemoriaCompartida *memoriaCompartida;
  struct sembuf semaforos[1];
  union semun semunion;

  threadsTable = (pthread_t *) malloc(sizeof(pthread_t) * jobCount);


    if ((idSeg = shmget(key, PAGE_SIZE, IPC_CREAT | IPC_EXCL | 0660 )) < 0) {
    fprintf(stderr, "Fallo al crear el segmento de memoria debido a: %d %s\n",
            errno, strerror(errno));
    exit(1);
  }

  if ((memoriaCompartida = (MemoriaCompartida *) shmat(idSeg, 0, 0)) == (void *) 0) {
    fprintf(stderr, "No pudo ser asignado el segmento de memoria: %d %s\n",
            errno, strerror(errno));
    exit(1);
  }

  if ((idSem = semget(key, 1, IPC_CREAT | IPC_EXCL | 0660)) < 0) {
    fprintf(stderr, "No se pudo crear un semaforo: %d %s\n",
            errno, strerror(errno));
    exit(1);
  }

  semunion.val = 0;

  if (semctl(idSem, 0, SETVAL, semunion) < 0) {
    fprintf(stderr, "No se pudo establecer el valor del semaforo: %d %s\n",
            errno, strerror(errno));
    exit(1);
  }


  memoriaCompartida->n = jobCount;
  memoriaCompartida->valSeq = 0;

  semaforos[0].sem_num = 0;
  semaforos[0].sem_op = 1;
  semaforos[0].sem_flg = 0;

  if (semop(idSem, semaforos, 1) < 0) {
    fprintf(stderr, "No fue posible senalar el semaforo: %d %s\n",
            errno, strerror(errno)); exit(1);
  }

  for (i = 0; i < jobCount; ++i){
    jobs[i].tid = i;
    rc = pthread_create((threadsTable + i), NULL, startProcessManager, (void *) &jobs[i]);
    assert(0 == rc);
  }

  for (i = 0; i < jobCount; ++i){
    rc = pthread_join(*(threadsTable + i), (void **) &status);
    assert(0 == rc);
  }

  if (semctl(idSem, 0, IPC_RMID, semunion) == -1) {
    fprintf(stderr, "No fue posible liberar el semaforo: %d %s\n",
            errno, strerror(errno)); exit(1);
  }

  if (shmctl(idSeg, IPC_RMID, NULL) == -1){
    fprintf(stderr, "No fue posible liberar la memoria compartida: %d %s\n",
            errno, strerror(errno)); exit(1);
  }

  return 0;
}
