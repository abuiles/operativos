#include "dbg.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{


  debug ("Comenzando proceso hijo de  %d", getppid());
  int i;

  for (i = 0; i < 5; ++i){
    debug ("Hi hi proceso del proceso %d", getppid());
    sleep(1);
  }

  exit(1);
}
