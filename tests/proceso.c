#include "dbg.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

  int id = atoi(argv[1]);
  debug ("Comenzando proceso %d", id);
  int i;

  for (i = 0; i < 5; ++i){
    debug ("Hi hi proceso del thread %d", id);
    sleep(1);
  }

  exit(1);
}
