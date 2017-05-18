#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "readln.h"

int *vlines;
int lines;
int ready;

void writeOP(void* buf, int size, int add){
    int r;
    int i=0;
    char c;
    c = ((char*)buf)[i];
    while ((r=(write(1,&c,1)))>0 && i<size-2){
        i++;
        c = ((char*)buf)[i];
    }
    char new[10];
    sprintf(new, "%d", add);
    int len = strlen(new);
    i=0;
    c = ':';
    write(1,&c,1);
    c = new[i];
    while ((r=(write(1,&c,1)))>0 && i<len-1 && new!=NULL){
        i++;
        c = new[i];
    }
    c = '\n';
    write(1, &c, 1);
}

void operate(void* buf, int size, char* operation, int new){
    int add=0;
    int i=0;
    if((strcmp(operation, "avg"))==0){
        if(ready==lines){
            for(i=0; i < lines; i++){
                add+= vlines[i];
            }
            add = add/lines;
            for(i=lines-1; i!=0; i--){
                vlines[i] = vlines[i-1];
            }
            vlines[0]=new;
        }
        else{
            if (ready==0) add = 0;
            else {
                for(i=0; i<ready; i++){
                    add+= vlines[i];
                }
                add = add/ready;
            }
            vlines[i] = new;
            ready++;
        }
        writeOP(buf, size, add);
    }
    if(strcmp(operation, "max")==0){
        if(ready==lines){
            add = vlines[0];
            for(i=0; i < lines; i++){
                if(vlines[i] > add) add =vlines[i];
            }
            for(i=lines-1; i!=0; i--){
                vlines[i] = vlines[i-1];
            }
            vlines[0]=new;
        }
        else{
            if (ready==0) add = 0;
            else {
                add = vlines[0];
                for(i=0; i<ready; i++){
                    if(vlines[i] > add) add =vlines[i];
                }
            }
            vlines[i] = new;
            ready++;
        }
        writeOP(buf, size, add);
    }
    if(strcmp(operation, "min")==0){
        if(ready==lines){
            add = vlines[0];
            for(i=0; i < lines; i++){
                if(vlines[i] < add) add =vlines[i];
            }
            for(i=lines-1; i!=0; i--){
                vlines[i] = vlines[i-1];
            }
            vlines[0]=new;
        }
        else{
            if (ready==0) add = 0;
            else {
                add = vlines[0];
                for(i=0; i<ready; i++){
                    if(vlines[i] < add) add =vlines[i];
                }
            }
            vlines[i] = new;
            ready++;
        }
        writeOP(buf, size, add);
    }
    if(strcmp(operation, "sum")==0){
        if(ready==lines){
            for(i=0; i < lines; i++){
                add += vlines[i];
            }
            for(i=lines-1; i!=0; i--){
                vlines[i] = vlines[i-1];
            }
            vlines[0]=new;
        }
        else{
            if (ready==0) add = 0;
            else {
                for(i=0; i<ready; i++){
                    add += vlines[i];
                }
            }
            vlines[i] = new;
            ready++;
        }
        writeOP(buf, size, add);
    }
}

void window(void* buf, char* copy, int size, char* operation, int coluna){
    int c=1;
    int product;
    char* num;
    for(num = strtok(copy, ":"); num != NULL && c<coluna; num = strtok(NULL, ":")){
        c++;
    }
    if (num==NULL){
        perror("missing columns");
        exit(-1);
    }
    else {
        product = atoi(num);

        operate(buf, size, operation, product);
    }
}

int main (int argc , char*argv[]) {
    if (argc != 4) {
        perror("missing arguments or too much arguments");
        exit(-1);
    }
    int c = atoi(argv[1]);
    int n = atoi(argv[3]);
    lines = n;
    ready = 0;
    int i;
    vlines = malloc (sizeof(int)*n);
    for(i= 0; i< n; i++)
        vlines[i] = 0;
    i = 0;
    char buf[PIPE_BUF];
    char copy[PIPE_BUF];

    while ((i=readln(0, buf, PIPE_BUF))>0){
        strcpy(copy, buf);
        window(&buf, copy, i, argv[2], c);
    }

   return 0;
}
