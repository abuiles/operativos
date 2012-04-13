#include "linux_control_console.h"

void *handleSTDOUT(void *file){
  FILE *stream;
  char temp[1024];
  int c;
  int *fid = (int *) file;
  stream = fdopen (*(fid), "r");
  int n;

  while(fgets(temp, 1024, stream) != EOF){
    fprintf(stdout, "%s", temp);
    fflush(stdout);
  }
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
