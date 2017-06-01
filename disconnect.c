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

pid_t nodes[10];
pid_t connects[50];
int connections[50][50];

void disconnect(char** argv, int argc){
    int firstN = atoi(argv[2]);
    int secondN = atoi(argv[3]);
    int j=0, i;
    char* args="node ", linked[50];
    connections[firstN][secondN] = 0;
    for(i=0; i<50; i++){
        if(connections[firstN][i]) {
            sprintf(((char**)linked)[j], "%d", i);
            j++;
        }
    }
    if(j==0) {
        kill(connects[firstN], SIGTERM);
    }
    else{
        pid_t p = fork();
        kill(connects[firstN], SIGTERM);
        if(!p){
            for(i=0; i<j; i++){
                args = concat(args, ((char**)linked)[i]);
                args = concat(args, " ");
            }
            execvp("./connect", &args);
        }
    }

}

int main(int argc, char** argv){
  //pid_t p;
  if(argc != 4){
    perror("missing arguments");
    _exit(-1);
  }
  disconnect(argv, argc);
  return 0;
}