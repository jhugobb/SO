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

pid_t nodes[50];
pid_t connects[50];
int connections[50][50];
int infds[50];
int outfds[50];

pid_t node(char** argv){
	int i, id, nonamepipein[2], nonamepipeout[2];
	char buf[PIPE_BUF], *namein, *nameout;
	pid_t p, readlnp, dupexecp, writep;
	id = atoi(argv[1]);
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
			infds[id] = open(namein, O_RDONLY);
			close(nonamepipein[READ]);
			while((i = readln(infds[id], buf, PIPE_BUF)) > 0){
				write(nonamepipein[WRITE], buf, i);
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
					outfds[id] = open(nameout, O_WRONLY);
					close(nonamepipeout[WRITE]);
					while((i = readln(nonamepipeout[READ], buf, PIPE_BUF))>0){
						write(outfds[id], buf, i);
						fsync(outfds[id]);
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

pid_t connect(int argc, char** argv){
	if(argc < 3){
		perror("missing arguments");
		_exit(-1);
	}
	int id = atoi(argv[1]), i;
	if(nodes[id]==0){
		perror("nodo não existe");
		return 0;
	}
	if(connects[id]){
		kill(connects[id], SIGTERM);
	}
	for(i=2; i<argc; i++){
		if(nodes[atoi(argv[i])]==0){
			perror("nodo não existe");
			return 0;
		}
		connections[id][atoi(argv[i])] = 1;
	}
	pid_t p;
	p = fork();
	if(!p){
		int r,j, size = argc-2, ins[size];
		char buf[PIPE_BUF], *nameout, *in, number[4];
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
			}
		}
		outfds[id] = open(nameout, O_RDONLY);
		if(outfds[id]==-1){
			perror("Error opening pipe");
			connects[id]=0;
			_exit(-1);
		}
		while((r = readln(outfds[id], buf, PIPE_BUF))>0){
			for(i=0; i<size; i++){
				write(ins[i], buf, r+1);
			}
		}
		_exit(0);
	}
	else{
		return p;
	}
	_exit(0);
}

pid_t disconnect(int argc, char** argv){
	if(argc < 3){
		perror("missing arguments");
		_exit(-1);
	}
	int id = atoi(argv[1]), i;
	if(nodes[id]==0){
		perror("Nodo não existe");
		_exit(-1);
	}
	for(i=2; i<argc; i++){
		if(nodes[atoi(argv[i])]==0){
			perror("Nodo não existe");
			_exit(-1);
		}
	}
	if(connects[atoi(argv[1])]){
		kill(connects[atoi(argv[1])], SIGTERM);
		connects[atoi(argv[1])] = 0;
	}
	for(i=2; i<argc; i++){
		connections[id][atoi(argv[i])] = 0;
	}
	pid_t p;
	p = fork();
	if(!p){
		int r,j, size = argc-2, ins[size], outfd, flag = 0;
		char buf[PIPE_BUF], *nameout, *in, number[4];
		nameout = concat("out", argv[1]);
		for(i=0,j=0; i<50; i++){
			if(connections[id][i]){
				flag=1;
				sprintf(number, "%d", i);
				in = concat("in", number);
				ins[j]=open(in, O_WRONLY);
				j++;
			}
		}
		if(flag){
			outfd = open(nameout, O_RDONLY);
			if(outfds[id]==-1){
				perror("Error opening pipe");
				connects[id]=0;
				_exit(-1);
			}
			while((r = readln(outfd, buf, PIPE_BUF))>0){
				for(i=0; i<size; i++){
					write(ins[i], buf, r);
				}
			}
		}
		_exit(0);
	}
	else{
		return p;
	}
	_exit(0);
}

int main(){
	int i=0, j=0, id, size;
	for(i=0;i<50;nodes[i++]=0);
	for(i=0;i<50;connects[i++]=0);
	for(i=0;i<50;infds[i++]=0);
	for(i=0;i<50;outfds[i++]=0);
	for(i=0;i<50;i++){
		for(j=0;j<50;j++){
			connections[i][j] = 0;
		}
	}
	char buf[PIPE_BUF];
	char *input[PIPE_BUF];
	while((i = readln(0, buf, PIPE_BUF)) > 0){
		size = processInput(input, buf, " ");
		if(size){
			if(!strncmp(input[0], "node", 4)){
				id = atoi(input[1]);
				if(id<50){
					nodes[id] = node(input);
				}
				else printf("Id not in range of storage\n");
			}
			else if(!strncmp(input[0], "connect", 7)){
				id = atoi(input[1]);
				if(id<50){
					connects[id] = connect(size, input);
				}
				else printf("Id not in range of storage\n");
			}
			else if(!strncmp(input[0], "disconnect", 10)){
				id = atoi(input[1]);
				if(id<50){
					connects[id] = disconnect(size, input);
				}
				else printf("Id not in range of storage\n");
			}
			else if(!strncmp(input[0], "inject", 6)){
				printf("You typed inject!\n");
			}
			else if(!strncmp(input[0], "print", 5)){
				for(i=0;i<50;i++){
					if(nodes[i]!=0) printf("I EXIST: %d\n", nodes[i]);
				}
			}
		}
	}
	return 0;
}
