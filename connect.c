#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>
#include "readln.h"
#include "concat.h"
#include "processInput.h"

#define READ 0
#define WRITE 1

pid_t nodes[10];
pid_t connects[50];
int connections[50][50];
int infds[50];
int outfds[50];


pid_t node(char** argv){
    int i, infd, outfd,  nonamepipein[2], nonamepipeout[2];
    char buf[PIPE_BUF], *namein, *nameout;
    pid_t p, readlnp, dupexecp, writep;
    namein = concat("in", argv[1]);
    nameout = concat("out", argv[1]);
    mkfifo(namein, 0666);
    mkfifo(nameout, 0666);
    p = fork();
    if(!p){
        pipe(nonamepipein);
        pipe(nonamepipeout);
        readlnp = fork();
        if(!readlnp){
            close(nonamepipein[READ]);
            infd = open(namein, O_RDONLY);
            printf("%s abriu o pipe de entrada\n", argv[1]);
            while((i = readln(infd, buf, PIPE_BUF)) > 0){
                printf("%s leu do pipe de entrada\n", argv[1]);
                write(nonamepipein[WRITE], buf, i);
                printf("%s escreveu no anonIn\n", argv[1]);
            }
        }
        else{
            dupexecp = fork();
            if(!dupexecp){
                close(nonamepipein[WRITE]);
                close(nonamepipeout[READ]);
                dup2(nonamepipein[READ], 0);//dup do std in para o pipe in
                dup2(nonamepipeout[WRITE], 1);//dup do std out para o pipe out
                execv("./const", &argv[2]);

            }
            else{
                writep = fork();
                if(!writep){
                    outfd = open(nameout, O_WRONLY);
                    printf("%s abriu o pipe de saida\n", argv[1]);
                    close(nonamepipeout[WRITE]);
                    while((i = readln(nonamepipeout[READ], buf, PIPE_BUF))>0){
                        printf("%s leu do anonOut\n", argv[1]);
                        write(outfd, buf, i);
                        printf("%s escreveu no nameOut\n", argv[1]);
                        fsync(outfd);
                    }
                }
            }
        }
    }
    else{
        return p;
    }
    _exit(0);
}

void connect(int argc, char** argv){
    if(argc < 3){
        perror("missing arguments");
        _exit(-1);
    }
    int id = atoi(argv[1]), i;
	if(nodes[id]==0){
		perror("nodo não existe");
	}
	if(connects[id]){
        kill(connects[id], SIGTERM);
    }
    for(i=2; i<argc; i++){
		if(nodes[atoi(argv[i])]==0){
			perror("nodo não existe");
		}
        connections[id][atoi(argv[i])] = 1;
    }
    pid_t p;
	printf("O MEU NODO DE DESTINO É %s\n", argv[2]);
    p = fork();
    if(!p){
        int r,j, size = argc-2, ins[size];
        char buf[PIPE_BUF], *nameout, *in, number[PIPE_BUF];
        nameout = concat("out", argv[1]);
        for(i=0, j=0; i<50; i++){
            if(connections[id][i]){
                sprintf(number, "%d", i);
                in = concat("in", number);
                infds[i] = ins[j]=open(in, O_WRONLY);
                j++;
				if(infds[i]==-1){
					perror("Error opening pipe");
					connects[id]=0;
					_exit(-1);
				}
                printf("connector abriu o pipe %s\n", in);
            }
        }
        outfds[id] = open(nameout, O_RDONLY);
		if(outfds[id]==-1){
			perror("Error opening pipe");
			connects[id]=0;
			_exit(-1);
		}
        printf("connector abriu o pipe %s\n", nameout);
        while((r = readln(outfds[id], buf, PIPE_BUF))>0){
            for(i=0; i<size; i++){
                write(ins[i], buf, r);
                printf("connector escreveu no pipe %s\n", in);
                printf("O QUE ESTAVA NO BUF: %s\n", buf);
            }
        }
        _exit(0);
    }
}

int main(int argc, char** argv){
    int r, i, j;
    char buf[PIPE_BUF], copy[PIPE_BUF];
    char* result[PIPE_BUF];
    for(i=0;i<50;i++){
        for(j=0;j<50;j++){
            connections[i][j] = 0;
        }
    }
    //node 1
    r= readln(0, buf, PIPE_BUF);
    strcpy(copy, buf);
    processInput(result, copy, " ");
    node(result);
    memset(buf, 0, r);
    //node 2
    r= readln(0, buf, PIPE_BUF);
    strcpy(copy, buf);
    processInput(result, copy, " ");
    node(result);
    memset(buf, 0, r);
    //node 3
    r= readln(0, buf, PIPE_BUF);
    strcpy(copy, buf);
    processInput(result, copy, " ");
    node(result);
    memset(buf, 0, r);
    //connect
    r = readln(0, buf, PIPE_BUF);
    strcpy(copy, buf);
    i = processInput(result, copy, " ");
    connect(i, result);
    memset(buf, 0, r);

    int infd = open("in22", O_WRONLY);
    int outfd3 = open("out7", O_RDONLY);
    r=readln(0, buf, PIPE_BUF);
    write(infd, buf, r);
    memset(buf, 0, r);
    r = readln(outfd3, buf, PIPE_BUF);
    write(1, buf, r);
    memset(buf, 0, r);

    r = readln(0, buf, PIPE_BUF);
    strcpy(copy, buf);
    i = processInput(result, copy, " ");
    connect(i, result);
    memset(buf, 0, r);

    outfd3 = open("out1", O_RDONLY);
    r = readln(0, buf, PIPE_BUF);
    write(infd, buf, r);
    memset(buf, 0, r);
    r = readln(outfd3, buf, PIPE_BUF);
    write(1, buf, r);
    memset(buf, 0, r);
    return 0;
}
