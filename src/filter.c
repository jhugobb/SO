#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "headers/readln.h"
#include "headers/concat.h"
#include "headers/processInput.h"


void escrever(void* buf, int size){
    strcat(buf, "\n");
    write(1, buf, size);
}

void operate(void* buf, int size, char* operation, int new1, int new2){
    if((strcmp(operation, "<"))==0){
        if(new1 < new2){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "<="))==0){
        if(new1 <= new2){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, ">"))==0){
        if(new1 > new2){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, ">="))==0){
        if(new1 >= new2){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "!="))==0){
        if(new1 != new2){
            escrever(buf, size);
        }
    }
    if((strcmp(operation, "="))==0){
        if(new1 == new2){
            escrever(buf, size);
        }
    }
}

void  filter (void* buf, char* copy, int size, char* operation, int column, int column2){
    int c=1;
    int product1 , product2;
    char *num1, *num2, copy2[PIPE_BUF];
    strcpy(copy2, copy);

    for(num1 = strtok(copy, ":"); num1 != NULL && c<column; num1 = strtok(NULL, ":")){
        c++;
    }
    if (num1==NULL){
        return;
    }
    c=1;
    product1 = atoi(num1);
    for(num2 = strtok(copy2, ":"); num2 != NULL && c<column2; num2 = strtok(NULL, ":")){
        c++;
    }
    if (num2==NULL){
        return;
    }
    else {
        product2 = atoi(num2);
        operate(buf, size, operation, product1, product2);

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

    while ((i=readln(0, buf, PIPE_BUF))>0){
        strcpy(copy, buf);
        filter(&buf, copy, i, argv[2], c, n);
        memset(buf, 0, i);
    }
   return 0;
}
