#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *linhas;
int lines;
int pronto;

void escrever(void* buf, int size, int add){
    int r;
    int i=0;
    char c;
    c = ((char*)buf)[i];
    while ((r=(write(1,&c,1)))>0 && i<size){
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
    while ((r=(write(1,&c,1)))>0 && i<len && new!=NULL){
        i++;
        c = new[i];
    }
    c = '\n';
    write(1,&c,1);
}

void operar(void* buf, int size, char* operation, int novo){
    int add=0;
    int i=0;
    if((strcmp(operation, "avg"))==0){
        if(pronto==lines){
            for(i=0; i < lines; i++){
                add+= linhas[i];    
            }
            add = add/lines;
            for(i=lines-1; i!=0; i--){
                linhas[i] = linhas[i-1];
            }
            linhas[0]=novo;
        }
        else{
            if (pronto==0) add = 0;
            else {
                for(i=0; i<pronto; i++){
                    add+= linhas[i];  
                }
                add = add/pronto;
            }
            linhas[i] = novo;
            pronto++;   
        }
        escrever(buf, size, add);
    }
    if(strcmp(operation, "max")==0){
        if(pronto==lines){
            add = linhas[0];
            for(i=0; i < lines; i++){
                if(linhas[i] > add) add =linhas[i];
            }
            for(i=lines-1; i!=0; i--){
                linhas[i] = linhas[i-1];
            }
            linhas[0]=novo;
        }
        else{
            if (pronto==0) add = 0;
            else {
                add = linhas[0];
                for(i=0; i<pronto; i++){
                    if(linhas[i] > add) add =linhas[i]; 
                }
            }
            linhas[i] = novo;
            pronto++;   
        }
        escrever(buf, size, add);
    }
    if(strcmp(operation, "min")==0){
        if(pronto==lines){
            add = linhas[0];
            for(i=0; i < lines; i++){
                if(linhas[i] < add) add =linhas[i];
            }
            for(i=lines-1; i!=0; i--){
                linhas[i] = linhas[i-1];
            }
            linhas[0]=novo;
        }
        else{
            if (pronto==0) add = 0;
            else {
                add = linhas[0];
                for(i=0; i<pronto; i++){
                    if(linhas[i] < add) add =linhas[i]; 
                }
            }
            linhas[i] = novo;
            pronto++;   
        }
        escrever(buf, size, add);
    }
    if(strcmp(operation, "sum")==0){
        if(pronto==lines){
            for(i=0; i < lines; i++){
                add += linhas[i];
            }
            for(i=lines-1; i!=0; i--){
                linhas[i] = linhas[i-1];
            }
            linhas[0]=novo;
        }
        else{
            if (pronto==0) add = 0;
            else {
                for(i=0; i<pronto; i++){
                    add += linhas[i]; 
                }
            }
            linhas[i] = novo;
            pronto++;   
        }
        escrever(buf, size, add);
    }
}

void  window (void* buf, char* copia, int size, char* operation, int coluna){
    int c=1;
    int produto;
    char* num;
    for(num = strtok(copia, ":"); num != NULL && c<coluna; num = strtok(NULL, ":")){
        c++;
    }
    if (num==NULL){
        perror("missing columns");
        exit(-1);
    }
    else {
        produto = atoi(num);

        operar(buf, size, operation, produto);
    }
}

ssize_t readln(int fildes, void *buf, int nbytes){
 	char c;
 	ssize_t i;
 	for (i = 0; read(fildes, &c, 1) == 1 && c != '\n' && c!= '\0' && i < nbytes;i++){
         ((char*)buf)[i] = c;
 	}
 	return i;
 }

int main (int argc , char*argv[]) {
    if (argc != 4) {
        perror("missing arguments or too much arguments");
        exit(-1);
    }
    int c = atoi(argv[1]);
    int n = atoi(argv[3]);
    lines = n;
    pronto = 0;
    int i;

    linhas = malloc (sizeof(int)*n);
    for(i= 0; i< n; i++)
        linhas[i] = 0;

    i = 0;
    char buf[1024];
    char copia[1024];

 
    while ((i=readln(0,&buf, 1024))>0){
        strcpy(copia, buf);
        window(&buf, copia, i, argv[2], c);
    }

   return 0;
}