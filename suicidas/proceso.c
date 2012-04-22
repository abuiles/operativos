#include <stdio.h>
#include <windows.h>
/* #include <unistd.h> */

int main(int argc, char *argv[])
{


  fprintf(stdout, "Comenzando proceso hijo de pending getppid()\n");
  fflush(stdout);
  int i;
  int j;
  for (i = 0; i < 5; ++i){
    fprintf(stdout, "Hi hi proceso del proceso pending getppid\n  ");
    fprintf(stdout, "Hi hi my pid is pending getppid\n");
    fflush(stdout);
    for( j = 0; i < 100000; ++i){
    }
    fflush(stdout);

    for( j = 0; i < 10000000; ++i){
    }
    Sleep(5000);
  }


  return 0;
}
