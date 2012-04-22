#include "windows_process_manager.h"

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

int parseArgs(int argc, char *argv[], char *args[]){
  static struct option long_options[] =
    {
    {"filepath",  required_argument, 0, 'f'},
    {"filename",  required_argument, 0, 'n'},
    {"reencarnacion",    required_argument, 0, 'r'},
    {0, 0, 0, 0}
  };
  int option_index = 0, i = 0, c;

  for (i = 0; i < 3; ++i){
    c = getopt_long (argc, argv, ":f:n:r",
                     long_options, &option_index);

    if (c == -1)
      break;

    switch (c)
      {
        case 'f':
          fprintf(stdout, "arg filepath %s\n", optarg);
          args[0] = optarg;
          break;
        case 'n':
          fprintf(stdout, "arg name %s\n", optarg);
          args[1] = optarg;
          break;
        case 'r':
          fprintf(stdout, "arg reencarnacion %s\n", optarg);
          args[2] = optarg;
          break;
      }
  }

  args[3] = argv[4];
  return 0;
}

int handleProcess( int argc, char *argv[])
{
  if (argc != 5){
    fprintf(stderr, "Argumentos incorrectos, utilice de la siguiente manera:\n");
    fprintf(stderr, "procesoctrl --filepath=<path del ejecutable> --filename=<nombre del ejecutable> --reencarnacion=<número de reencarnaciones> <número del proceso de control>\n");

    return 1;
  }

  char *args[4]; /* {filepath, filaname, reencarnacion, id} */

  parseArgs(argc, argv, args);

  int pid, repeat, rc;
  int run = 1;
  int done = False;
  DWORD status;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;


  fprintf(stdout, "filepath: %s, filaname %s, reencarnacion %s, id %s\n",
          args[0], args[1], args[2], args[3]);

  repeat = atoi(args[2]);

  /* int outfd[2]; */
  /* pthread_t *table; */
  /* table = (pthread_t *) malloc(sizeof(pthread_t) * 1); */


  /* if (pipe(outfd) == -1) { */
  /*   perror("pipe"); */
  /*   return (void *) -1; */
  /* } */

  while( !done ){
    GetStartupInfo(&si);
    /* pid = fork(); */

    pid = CreateProcess(
                        NULL,
                        "NOTEPAD proceso.c",
                        NULL,
                        NULL,
                        FALSE,
                        0,
                        NULL,
                        NULL,
                        &si,
                        &pi
                        );

    if (pid == 0){
      /* close(outfd[0]); */

      /* dup2(outfd[1], 1); */
      /* close(outfd[1]); */

      /* char program[80] = ""; */

      /* strcat (program, args[0]); */
      /* strcat (program, "/"); */
      /* strcat (program, args[1]); */

      /* char *newargv[] = {args[1], NULL }; */
      /* char *newenviron[] = { NULL }; */

      /* status = execve(program, newargv , newenviron); */

      fprintf(stderr, "Proceso suicida %s no pudo ser ejecutado.\n", args[3]);
      fprintf(stderr, "Código de error es : %d\n", status);
      fprintf(stderr, "Error: %s\n", strerror(errno));

      exit(1);
    }else{

      /* if(!rc) */
      /*   rc = pthread_create((table + 0), NULL, handleSTDOUT, (void *) &outfd[0]); */

      WaitForSingleObject( pi.hProcess, INFINITE );
      GetExitCodeProcess(pi.hProcess, &status);
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );

      if (repeat == 0){
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: Infinitas\n", args[1], status, args[3]);
      } else {
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: %d\n", args[3], status, args[3], repeat - run);
      };
      fflush(stdout);

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


