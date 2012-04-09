#include "dbg.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{


  fprintf(stdout, "Comenzando proceso hijo de  %d\n", getppid());
  int i;

  for (i = 0; i < 5; ++i){
    fprintf(stdout, "Hi hi proceso del proceso %d\n", getppid());
    fprintf(stdout, "Hi hi my pid is %d\n", getpid());
    sleep(1);
  }

  return 0;
}
