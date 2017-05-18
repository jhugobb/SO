#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "readln.h"
#include "processInput.h"

int operando;

void escrever(void* buf, int size){
    int r;
    int i=0;
    char c;
    c = ((char*)buf)[i];
    while ((r=(write(1,&c,1)))>0 && i<size){
        i++;
        c = ((char*)buf)[i];
    }
}

void operate(void* buf, int size, char* operation, int new){
    if((strcmp(operation, "<"))==0){
        if(new < operando){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "<="))==0){
        if(new <= operando){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, ">"))==0){
        if(new > operando){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, ">="))==0){
        if(new >= operando){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "!="))==0){
        if(new != operando){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "="))==0){
        if(new == operando){
            escrever(buf, size);
        }
    }
}

void  window (void* buf, char* copy, int size, char* operation, int column){
    int c=1;
    int product;
    char *num;
    for(num = strtok(copy, ":"); num != NULL && c<column; num = strtok(NULL, ":")){
        c++;
    }
    if (num==NULL){
        perror("Missing columns!");
        exit(-1);
    }
    else {
        product = atoi(num);
        operate(buf, size, operation, product);
    }
}

int main (int argc , char*argv[]) {
    if (argc != 4) {
        perror("Missing arguments or too much arguments!");
        exit(-1);
    }
    int c = atoi(argv[1]);
    int n = atoi(argv[3]);
    int i=0;
    char buf[PIPE_BUF];
    char copy[PIPE_BUF];
    operando = n;

    while ((i=readln(0, buf, PIPE_BUF))>0){
        strcpy(copy, buf);
        window(&buf, copy, i, argv[2], c);
        memset(buf, 0, i);
    }
   return 0;
}
