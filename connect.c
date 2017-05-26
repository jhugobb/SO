#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include "readln.h"
#include "concat.h"
#include "processInput.h"

#define READ 0
#define WRITE 1

pid_t connects[50];
int connections[50][50];

void connect(char** argv, int argc){
  if(argc < 3){
    perror("missing arguments");
    _exit(-1);
  }
  int i, j, id, read, inID, outID, tam = argc-2;
  char buf[PIPE_BUF], *nameout, *ids[tam];
  pid_t p;
  for(i=0, j=2; i<tam && j<argc; i++, j++){
    ids[i] = concat("in", argv[j]);
  }
  nameout = concat("out", argv[1]);
  outID = open(nameout, O_RDONLY);
  if(outID == -1) {
    perror("This node doesn't exist");
    _exit(-1);
  }
  p = fork();
  id = atoi(argv[1]);
  for(i=2; i<argc; i++){
    connections[id][atoi(argv[i])] = 1;
  }
  if(connects[id] != 0) {
    kill(connects[id], SIGTERM);
    connects[id] = p;
  }
  if(!p){
    while((read=readln(outID, buf, PIPE_BUF))>0){
      i=0;
      while(i<tam){
        inID = open(ids[i], O_WRONLY);
        write(inID, buf, read);
        i++;
      }
      memset(buf, 0, PIPE_BUF);
    }
  }
  else{
    //return p;
  }
  _exit(0);
}

int main(int argc, char** argv){
  //pid_t p;
  connect(argv, argc);
  return 0;
}
