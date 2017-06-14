#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>
#include "headers/readln.h"
#include "headers/concat.h"
#include "headers/processInput.h"

#define READ 0
#define WRITE 1

pid_t nodes[50];
pid_t readlnps[50];
pid_t dupexecps[50];
pid_t writeps[50];
pid_t connects[50];
int connections[50][50];
int infds[50];
int outfds[50];
int nodeToRemove;

pid_t node(char** argv);
pid_t connect(int argc, char** argv);
pid_t disconnect(int argc, char** argv);
void inject(int argc, char** argv);

void removeNode(int sig){
	printf("Node %d is going to be removed\n", nodeToRemove);
	int i;
	char* dis[4];
	dis[0] = "disconnect";
	dis[3] = NULL;
	dis[1] = malloc(4*sizeof(char));
	dis[2] = malloc(4*sizeof(char));
	sprintf(dis[1], "%d", nodeToRemove);
	for(i=0;i<50;i++){
		if(connections[nodeToRemove][i]){
			connections[nodeToRemove][i]=0;
			sprintf(dis[2], "%d", i);
			connects[nodeToRemove] = disconnect(3, dis);
		}
	}

	kill(readlnps[nodeToRemove], SIGKILL);
	kill(dupexecps[nodeToRemove], SIGKILL);
	kill(writeps[nodeToRemove], SIGKILL);
	printf("Node removed!\n");
}

pid_t node(char** argv){
	int i, id, nonamepipein[2], nonamepipeout[2];
	char buf[PIPE_BUF], *namein, *nameout;
	pid_t p;
	id = atoi(argv[1]);
	namein = concat("pipes/in", argv[1]);
	nameout = concat("pipes/out", argv[1]);
	mkfifo(namein, 0666);
	mkfifo(nameout, 0666);
	p = fork();
	if(!p){
		pipe(nonamepipein);
		pipe(nonamepipeout);
		readlnps[id] = fork();
		if(!readlnps[id]){
			infds[id] = open(namein, O_RDONLY);
			close(nonamepipein[READ]);
			while((i = readln(infds[id], buf, PIPE_BUF)) > 0){
				write(nonamepipein[WRITE], buf, i);
			}
		}
		else{
			dupexecps[id] = fork();
			if(!dupexecps[id]){
				close(nonamepipein[WRITE]);
				close(nonamepipeout[READ]);
				dup2(nonamepipein[READ], 0);//dup do std in para o pipe in
				dup2(nonamepipeout[WRITE], 1);//dup do std out para o pipe out
				execv(argv[2], &argv[2]);
				execvp(argv[2], &argv[2]);
				perror("exec error");
				_exit(-1);
			}
			else{
				writeps[id] = fork();
				if(!writeps[id]){
					outfds[id] = open(nameout, O_WRONLY);
					close(nonamepipeout[WRITE]);
					while((i = readln(nonamepipeout[READ], buf, PIPE_BUF))>0){
						write(outfds[id], buf, i);
						fsync(outfds[id]);
					}
				}
			}
		}
		//signal(SIGTERM, removeNode);
		wait(0);
	}
	else{
		return p;
	}
	printf("ADEUS MUNDO CRUEL %d\n", id);
	_exit(0);
}

pid_t connect(int argc, char** argv){
	if(argc < 3){
		perror("missing arguments");
		return connects[atoi(argv[1])];
	}
	int id = atoi(argv[1]), i;
	if(nodes[id]==0){
		perror("nodo não existe");
		return connects[atoi(argv[1])];
	}
	if(connects[id]){
		kill(connects[id], SIGTERM);
	}
	for(i=2; i<argc; i++){
		if(nodes[atoi(argv[i])]==0){
			perror("nodo não existe");
			return connects[atoi(argv[1])];
		}
		connections[id][atoi(argv[i])] = 1;
	}
	pid_t p;
	p = fork();
	if(!p){
		int r,j, size = argc-2, ins[size];
		char buf[PIPE_BUF], *nameout, *in, number[4];
		nameout = concat("pipes/out", argv[1]);
		for(i=0, j=0; i<50; i++){
			if(connections[id][i]){
				sprintf(number, "%d", i);
				in = concat("pipes/in", number);
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
			memset(buf, 0, r);
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
		return connects[atoi(argv[1])];
	}
	for(i=2; i<argc; i++){
		if(nodes[atoi(argv[i])]==0){
			perror("Nodo não existe");
			return connects[atoi(argv[1])];
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
		nameout = concat("pipes/out", argv[1]);
		for(i=0,j=0; i<50; i++){
			if(connections[id][i]){
				flag=1;
				sprintf(number, "%d", i);
				in = concat("pipes/in", number);
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
				memset(buf, 0, r);
			}
		}
		_exit(0);
	}
	else{
		return p;
	}
	_exit(0);
}

void inject(int argc, char** argv){
	int i, nonamepipeout[2];
	char* id;
	id = concat("pipes/in", argv[1]);
	char buf[PIPE_BUF];
	pid_t p;
	pipe(nonamepipeout);
	p = fork();
	if(!p){
		close(nonamepipeout[0]);
		dup2(nonamepipeout[1], 1);
		close(nonamepipeout[1]);
		execvp(argv[2], &argv[2]);
		perror("Exec failed");
		_exit(-1);
	}
	close(nonamepipeout[1]);
	int infd = open(id,	O_WRONLY);
	i = readln(nonamepipeout[0], buf, PIPE_BUF);
	write(infd, buf, i);
	fsync(infd);
	close(nonamepipeout[0]);
	kill(p,SIGTERM);
}

int controller(int fd){
	int i, size, id;
	char buf[PIPE_BUF];
	char *input[PIPE_BUF];
	while((i = readln(fd, buf, PIPE_BUF)) > 0){
		size = processInput(input, buf, " ");
		if(size){
			if(!strncmp(input[0], "node", 4)){
				id = atoi(input[1]);
				if(id<50){
					nodes[id] = node(input);
				}
				else perror("Id not in range of storage");
			}
			else if(!strncmp(input[0], "connect", 7)){
				id = atoi(input[1]);
				if(id<50){
					connects[id] = connect(size, input);
				}
				else perror("Id not in range of storage");
			}
			else if(!strncmp(input[0], "disconnect", 10)){
				id = atoi(input[1]);
				if(id<50){
					connects[id] = disconnect(size, input);
				}
				else perror("Id not in range of storage");
			}
			else if(!strncmp(input[0], "inject", 6)){
				id = atoi(input[1]);
				if(id>50) perror("Id not in range of storage");
				if(nodes[id]==0) perror("Node not found");
				else inject(size, input);
			}
			else if(!strncmp(input[0], "print", 5)){
				for(i=0;i<50;i++){
					if(nodes[i]!=0) printf("I EXIST: %d\n", nodes[i]);
				}
			}
			/*else if(!strncmp(input[0], "remove", 6)){
				id = atoi(input[1]);
				if(nodes[id]==0) perror("Node not found");
				else{
					nodeToRemove = id;
					kill(nodes[id], SIGTERM);
					nodes[id]=0;
				}
			}*/
			else perror("Command not recognized");
		}
		else{
			perror("Missing arguments in command");
		}
	}
	return 0;
}

int main(int argc, char** argv){
	int i=0, j=0, fd, result;
	for(i=0;i<50;i++){
		nodes[i]=0;
		connects[i]=0;
		readlnps[i]=0;
		dupexecps[i]=0;
		writeps[i]=0;
		outfds[i]=0;
		infds[i]=0;
		for(j=0;j<50;j++){
			connections[i][j] = 0;
		}
	}
	if(argc>1){
		for(i=1;i<argc;i++){
			fd=open(argv[i], O_RDONLY);
			result = controller(fd);
			close(fd);
		}
	}
	if(!result){
		controller(0);
	}
	return result;
}
