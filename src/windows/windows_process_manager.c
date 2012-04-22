#include "windows_process_manager.h"

DWORD WINAPI handleSTDOUT(LPVOID file){
  unsigned long bread;
  unsigned long avail;
  char temp[1024];
  HANDLE pipeRead = (HANDLE) file;

  memset(temp,0,sizeof(temp));
  while(1){
    PeekNamedPipe(pipeRead, temp, 1023, &bread, &avail,NULL);
    if (bread != 0){
      memset(temp,0,sizeof(temp));
      if (avail > 1023){
        while (bread >= 1023){
          ReadFile(pipeRead,temp,1023,&bread,NULL);
          printf("%s",temp);
          memset(temp,0,sizeof(temp));
        }
      }
      else {
        ReadFile(pipeRead,temp,1023,&bread,NULL);
        printf("%s",temp);
        memset(temp,0,sizeof(temp));
      }
    }
    fflush(stdout);
  }

  return 0;
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
          args[0] = optarg;
          break;
        case 'n':
          args[1] = optarg;
          break;
        case 'r':
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

  int pid, repeat;
  int run = 1;
  int done = False;
  DWORD status;

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  repeat = atoi(args[2]);

  DWORD   dwThreadIdArray[1];
  HANDLE  hThreadArray[1];

  HANDLE childrenSTDOUT;
  HANDLE readChildrenSTDOUT;
  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;

  if( !CreatePipe(&readChildrenSTDOUT, &childrenSTDOUT, &sa, 0)){
    fprintf(stdout, "Failed creating pipe");
    return 1;
  }

  char program[80] = "";
  strcat (program, args[0]);
  strcat (program, "/");
  strcat (program, args[1]);

  while( !done ){
    GetStartupInfo(&si);

    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = childrenSTDOUT;
    si.hStdError = childrenSTDOUT;

    pid = CreateProcess(
                        NULL,
                        program,
                        NULL,
                        NULL,
                        TRUE,
                        0,
                        NULL,
                        NULL,
                        &si,
                        &pi
                        );

    if (pid == 0){
      fprintf(stderr, "Proceso suicida %s no pudo ser ejecutado.\n", args[3]);
      fprintf(stderr, "Código de error es : %d\n", (int) status);
      fprintf(stderr, "Error: %s\n", strerror(errno));

      exit(1);
    }else{
      if(hThreadArray[0] == NULL){
        hThreadArray[0] = CreateThread(
            NULL,
            0,
            handleSTDOUT,
            (LPVOID) readChildrenSTDOUT,
            0,
            &dwThreadIdArray[0]);

        if (hThreadArray[0] == NULL)
          {
            fprintf(stdout, "Failed creating thread %d", 0);
            ExitProcess(3);
          }
      }

      WaitForSingleObject( pi.hProcess, INFINITE );
      GetExitCodeProcess(pi.hProcess, &status);
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );

      if (repeat == 0){
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: Infinitas\n", args[1], (int) status, args[3]);
      } else {
        fprintf(stdout, "Proceso suicida `%s` termino por causa %d -- Proceso Control %s, vidas restantes: %d\n", args[3], (int) status, args[3], repeat - run);
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
  CloseHandle(hThreadArray[0]);
  return 0;
}


