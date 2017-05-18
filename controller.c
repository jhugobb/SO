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

pid_t nodes[10];
pid_t connectors[10];

pid_t node(char** argv){
	int i, infd, outfd,  nonamepipein[2], nonamepipeout[2];
	for(i=0;i<10;nodes[i++]=-1);
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
			infd = open(namein, O_RDONLY);
			close(nonamepipein[READ]);
			while((i = readln(infd, buf, PIPE_BUF)) > 0){
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
				execv(concat("./", argv[2]), &argv[3]);
			}
			else{
				writep = fork();
				if(!writep){
					outfd = open(nameout, O_WRONLY);
					close(nonamepipeout[WRITE]);
					while((i = readln(nonamepipeout[READ], buf, PIPE_BUF))>0){
						write(outfd, buf, i);
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

int main(){
	int i=0, id;
	for(i=0;i<10;nodes[i++]=-1);
	char buf[PIPE_BUF];
	char *input[PIPE_BUF];
	while((i = readln(0, buf, 4096)) > 0){
		processInput(input, buf, " ");
		if(!strncmp(input[0], "node", 4)){
			id = atoi(input[1]);
			if(id<10){
				nodes[id] = node(input);
			}
			else printf("Id not in range of storage\n");
		}
		else if(!strncmp(input[0], "connect", 7)){
			printf("You typed connect!\n");
		}
		else if(!strncmp(input[0], "disconnect", 10)){
			printf("You typed disconnect!\n");
		}
		else if(!strncmp(input[0], "inject", 6)){
			printf("You typed inject!\n");
		}
		else if(!strncmp(input[0], "print", 5)){
			for(i=0;i<10;i++){
				if(nodes[i]!=-1) printf("I EXIST: %d\n", nodes[i]);
			}
		}
		/*else if(!strncmp(input[0], "delete", 6)){
			if(input[1] != NULL){
			kill()
			}
		}*/
	}
	return 0;
}
