#ifndef __types_
#define __types_

typedef struct{
  char id[100];
  char path[100];
  char filename[100];
  char reincarnation[100];
  int tid;
} Job;


typedef struct {
  long int seq;
  int nDecesos;
} InfoMuerte;

typedef struct {
  int n;
  int valSeq;
  InfoMuerte muertes[100];
}  MemoriaCompartida;

#endif
