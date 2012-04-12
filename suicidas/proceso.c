#include "dbg.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{


  fprintf(stdout, "Comenzando proceso hijo de  %d\n", getppid());
  int i;
  int j;

  for (i = 0; i < 5; ++i){
    fprintf(stdout, "Hi hi proceso del proceso %d\n", getppid());
    fprintf(stdout, "Hi hi my pid is %d\n", getpid());
    for( j = 0; i < 100000; ++i){
    }
    fflush(stdout);

    for( j = 0; i < 10000000; ++i){
    }
  }


  return 0;
}
