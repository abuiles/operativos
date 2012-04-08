#include "process_manager.h"
#include "../../dbg.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


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


int main(int argc, char *argv[])
{

  if (argc  != 5){
    fprintf(stdout, "Argumentos incorrectos, utilice de la siguiente manera:\n");
    fprintf(stdout, "procesoctrl --filepath=<path del ejecutable> --filename=<nombre del ejecutable> --reencarnacion=<número de reencarnaciones> <número del proceso de control>\n");

    return 1;
  }

  char *args[4]; /* {filepath, filaname, reencarnacion, id} */

  parseArgs(argc, argv, args);

  printf ("filapath=%s\n", args[0]);
  printf ("filaname=%s\n", args[1]);
  printf ("reencarnaciones=%s\n", args[2]);
  printf ("procesoId=%s\n", args[3]);

  return 0;
}
