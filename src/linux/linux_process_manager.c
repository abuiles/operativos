#include "linux_process_manager.h"

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

int parseArgs(int argc, char *argv[], char *args[]){
  static struct option long_options[] =
  {
    { "filepath",      required_argument, 0, 'f'},
    { "filename",      required_argument, 0, 'n'},
    { "reencarnacion", required_argument, 0, 'r'},
    { "idMemoria",     required_argument, 0, 'm'},
    { "idSemaforoMemoria", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };
  int option_index = 0, i = 0, c;

  for (i = 0; i < 5; ++i){
    c = getopt_long (argc, argv, ":f:n:r:m:s",
                     long_options, &option_index);

    if (c == -1)
      break;

    switch (c)
      {
        case 'f':
          args[0] = optarg;
          break;
        case 'n':
          args[1] = optarg;
          break;
        case 'r':
          args[2] = optarg;
          break;
        case 'm':
          args[4] = optarg;
          break;
        case 's':
          args[5] = optarg;
          break;
      }
  }

  args[3] = argv[6];

  return 0;
}

int handleProcess( int argc, char *argv[])
{
  if (argc != 7){
    fprintf(stderr, "Argumentos incorrectos, utilice de la siguiente manera:\n");
    fprintf(stderr, "procesoctrl --filepath=<path del ejecutable> --filename=<nombre del ejecutable> --reencarnacion=<número de reencarnaciones> --idMemoria=IdentificadorMemoriaCompartida --idSemaforoMemoria=IdentificadorSemaforo <número del proceso de control>\n");

    return 1;
  }

  char *args[6]; /* {filepath, filaname, reencarnacion, id, memoria, semáforo} */


  parseArgs(argc, argv, args);

  int pid, status, repeat, rc, processNumber;
  int run = 1;
  int done = False;

  int idSeg;
  int idSem;
  int key, sKey;
  MemoriaCompartida *memoriaCompartida;
  struct sembuf semaforos[1];

  repeat = atoi(args[2]);
  processNumber = atoi(args[3]);

  key = atoi(args[4]);
  sKey = atoi(args[5]);

  int outfd[2];
  int errfd[2];
  pthread_t *table;
  table = (pthread_t *) malloc(sizeof(pthread_t) * 2);


  if (pipe(outfd) == -1) {
    perror("pipe");
    return (void *) -1;
  }

  if (pipe(errfd) == -1) {
    perror("pipe");
    return (void *) -1;
  }

  if ((idSeg = shmget(key, PAGE_SIZE, 0)) < 0) {
    fprintf(stderr, "Fallo al crear el segmento de memoria debido a: %d %s\n",
	    errno, strerror(errno));
    exit(1);
  }

  if ((memoriaCompartida = (MemoriaCompartida *) shmat(idSeg, 0, 0)) == (void *) 0) {
    fprintf(stderr, "No pudo ser asignado el segmento de memoria: %d %s\n",
	    errno, strerror(errno));
    exit(1);
  }

  if ((idSem = semget(sKey, 1, 0)) < 0) {
    fprintf(stderr, "No se pudo crear un semaforo: %d %s\n",
	    errno, strerror(errno));
    exit(1);
  }

  while( !done ){
    pid = fork();

    if (pid == 0){
      close(outfd[0]);

      dup2(outfd[1], 1);
      close(outfd[1]);

      close(errfd[0]);

      dup2(errfd[1], 2);
      close(errfd[1]);

      char program[80] = "";

      strcat (program, args[0]);
      strcat (program, "/");
      strcat (program, args[1]);

      char *newargv[] = {args[1], NULL };
      char *newenviron[] = { NULL };

      status = execve(program, newargv , newenviron);

      fprintf(stderr, "Proceso suicida %s no pudo ser ejecutado.\n", args[3]);
      fprintf(stderr, "Código de error es : %d\n", status);
      fprintf(stderr, "Error: %s\n", strerror(errno));

      exit(1);
    }else{

      if(!rc){
        rc = pthread_create((table + 0), NULL, handleSTDOUT, (void *) &outfd[0]);
        rc = pthread_create((table + 1), NULL, handleSTDERR, (void *) &errfd[0]);
      }

      waitpid(pid, &status, WUNTRACED | WCONTINUED);

      if (repeat == 0){
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: Infinitas\n", args[1], status, args[3]);
      } else {
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: %d\n", args[3], status, args[3], repeat - run);
      };
      fflush(stdout);


      semaforos[0].sem_num = 0;
      semaforos[0].sem_op = -1;
      semaforos[0].sem_flg = 0;

      printf("I'm going to the semaphore\n");
      fflush(stdout);

      if (semop(idSem, semaforos, 1) < 0) {
        fprintf(stderr, "No fue posible senalar el semaforo: %d %s\n",
                errno, strerror(errno));
        exit(1);
      }


      memoriaCompartida->muertes[processNumber].seq =  memoriaCompartida->valSeq;
      memoriaCompartida->muertes[processNumber].nDecesos =  run;
      printf("\nProcess es %d\n", processNumber);
      printf("Secuencia es %d\n", memoriaCompartida->muertes[processNumber].seq);
      printf("Decesos es %d\n", memoriaCompartida->muertes[processNumber].nDecesos);
      fflush(stdout);

      memoriaCompartida->valSeq = memoriaCompartida->valSeq + 1;

      semaforos[0].sem_num = 0;
      semaforos[0].sem_op = 1;
      semaforos[0].sem_flg = 0;

      if (semop(idSem, semaforos, 1) < 0) {
        fprintf(stderr, "No fue posible senalar el semaforo: %d %s\n",
                errno, strerror(errno)); exit(1);
      }


      run++;
      if (repeat > 0){
        if (run  > repeat){
          done = True;
        }
      }
    }
  }
  return 0;
}
