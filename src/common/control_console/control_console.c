#include "control_console.h"

int main(int argc, char *argv[]){
  if (argc != 2){
    fprintf(stderr, "Argumentos incorrectos, utilice de la siguiente manera:\n");
    fprintf(stderr, "consolactrl path/archivo/configuracion\n");

    return 1;
  }


  Job jobs[512];
  int jobCount = 0;

  jobCount = readConfigFile(argv[1], jobs);


  int status;

  status = startJobs( jobCount, jobs);

  return status;
}

