#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <sys/wait.h>
#include "readln.h"
#include "concat.h"
#include "processInput.h"

void writeOP(char* buf, char* execResult, int sizeBuf){
    char* final;
    char c;
    buf[sizeBuf-1] = ':';
    final = concat(buf, execResult);
    memset(execResult, 0, strlen(execResult));
    write(1, final, sizeBuf+strlen(execResult)+1);
    c = '\n';
    write(1, &c, 1);
}

void spawn(char* buf, char* copy, char** argv, int sizeBuf){
    int status;
    char execResult[4];
    pid_t p;
    p = fork();
    if(!p){
        execvp(argv[1], &argv[1]);
        _exit(-1);
    }
    else{
        wait(&status);
        sprintf(execResult, "%d", WEXITSTATUS(status));
        writeOP(buf, execResult, sizeBuf);
    }
}

void searchFor(char** args, int argc, char** input){
    int i=1, column;
    char *c;
    while(i<argc){
        c = args[i];
        if(*c == '$') {
            c++;
            column = *c - '0';
            args[i] = input[column-1];
        }
        i++;
    }
}

int main(int argc, char** argv){
    if (argc < 3) {
        perror("missing arguments");
        exit(-1);
    }
    int r;
    char buf[PIPE_BUF], copy[PIPE_BUF], *input[PIPE_BUF];
    while((r=readln(0, buf,1024))>0){
        strncpy(copy, buf, r);
        processInput(input, copy, ":");
        searchFor(argv, argc, input);
        spawn(buf, copy, argv, r);
        memset(buf, 0, r);
        memset(copy, 0, r);
    }
    return 0;
}
