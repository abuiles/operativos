#include "windows_control_console.h"

DWORD WINAPI handleSTDERR(LPVOID file){
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
          fprintf(stderr, "%s",temp);
          memset(temp,0,sizeof(temp));
        }
      }
      else {
        ReadFile(pipeRead,temp,1023,&bread,NULL);
        fprintf(stderr, "%s",temp);
        memset(temp,0,sizeof(temp));
      }
    }
    fflush(stderr);
  }

  return 0;
}

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

DWORD WINAPI startProcessManager(LPVOID pjob)
{

  Job *job = (Job *) pjob;

  int pid;
  int threadsNum = 2;

  DWORD status;
  DWORD  dwThreadIdArray[threadsNum];
  HANDLE hThreadArray[threadsNum];

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  HANDLE childrenSTDOUT;
  HANDLE readChildrenSTDOUT;
  HANDLE childrenSTDERR;
  HANDLE readChildrenSTDERR;

  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;



  if( !CreatePipe(&readChildrenSTDOUT, &childrenSTDOUT, &sa, 0)){
    fprintf(stdout, "Failed creating pipe");
    return 1;
  }

  if( !CreatePipe(&readChildrenSTDERR, &childrenSTDERR, &sa, 0)){
    fprintf(stdout, "Failed creating pipe");
    return 1;
  }

  char filepath[100] = " --filepath=";
  char filename[100] = " --filename=";
  char reincarnation[100] = " --reencarnacion=";
  char process_manager[400] =  "bin/procesoctrl";
  strcat(filepath, job->path);
  strcat(filename, job->filename);
  strcat(reincarnation, job->reincarnation);
  strcat(process_manager, filepath);
  strcat(process_manager, filename);
  strcat(process_manager, reincarnation);
  strcat(process_manager, " ");
  strcat(process_manager, job->id);

  GetStartupInfo(&si);
  si.dwFlags    = STARTF_USESTDHANDLES;
  si.hStdOutput = childrenSTDOUT;
  si.hStdError  = childrenSTDERR;

  pid = CreateProcess(
                      NULL,
                      process_manager,
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
    fprintf(stderr, "Proceso de control falló");
    fprintf(stderr, "Código de error es : %d\n", (int) status);
    fprintf(stderr, "Error: %s\n", strerror(errno));

    exit(1);
  }else{

    hThreadArray[0] = CreateThread(
      NULL,
      0,
      handleSTDOUT,
      (LPVOID) readChildrenSTDOUT,
      0,
      &dwThreadIdArray[0]
    );
    if (hThreadArray[0] == NULL){
      fprintf(stdout, "Failed creating thread %d", 0);
      ExitProcess(3);
    }

    hThreadArray[1] = CreateThread(
            NULL,
            0,
            handleSTDERR,
            (LPVOID) readChildrenSTDERR,
            0,
            &dwThreadIdArray[1]);

    if (hThreadArray[1] == NULL)
      {
        fprintf(stdout, "Failed creating thread %d", 0);
        ExitProcess(3);
      }

    WaitForSingleObject( pi.hProcess, INFINITE );
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
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
  int i;
  DWORD   dwThreadIdArray[jobCount];
  HANDLE  hThreadArray[jobCount];

  for (i = 0; i < jobCount; ++i){
    jobs[i].tid = i;
    hThreadArray[i] = CreateThread(
            NULL,
            0,
            startProcessManager,
            (LPVOID) &jobs[i],
            0,
            &dwThreadIdArray[i]);

    if (hThreadArray[i] == NULL){
      fprintf(stdout, "Failed creating thread %d", i);
      ExitProcess(3);
    }
  }
  WaitForMultipleObjects(jobCount, hThreadArray, TRUE, INFINITE);

  return 0;
}
