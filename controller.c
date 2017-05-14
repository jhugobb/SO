#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

#define READ 0
#define WRITE 1

pid_t nodes[10];

ssize_t readln(int fildes, void *buf, size_t nbyte){
	char c;
	ssize_t i;
	for (i = 0; read(fildes, &c, 1) == 1 && i<nbyte && c != '\n';){
        ((char*)buf)[i] = c;
        i++;
	}
	return i;
}

void processInput(char **result, char *buf, char *divider){
	int i;
	for(result[i=0]= strtok(buf, divider); result[i] != NULL; result[i] = strtok(NULL, divider)){
        i++;
    }
}

int getNodeid(char *buf){
    int i = 5, j = 0;
    char name[4];
    int id;
    while(buf[i]!=' ' && buf[i]){
        name[j] = buf[i];
        i++;
        j++;
    }
    id = atoi(name);
    if(id >=10){
        id = 9;
    }
    return id;
}

int main(){
    int i=0, id, infd, outfd,  nonamepipein[2], nonamepipeout[2];
    for(i=0;i<10;nodes[i++]=-1);
    char buf[4096], *in, *out;
	char *input[4096];
    pid_t p, readlnp, dupexecp, writep;
    while((i = readln(0, buf, 4096)) > 0){
		processInput(input, buf, " ");
        if(!strncmp(input[0], "node", 4)){
			id = atoi(input[1]);
			in = strcat("in", input[1]);
			out = strcat("out", input[1]);
			mkfifo(in, 0666);
			mkfifo(out, 0666);
			p = fork();
            if(!p){
				infd = open(in, O_RDONLY);
				outfd = open(out, O_WRONLY);
				pipe(nonamepipein);
				pipe(nonamepipeout);
				readlnp = fork();
				if(!readlnp){
					close(nonamepipein[READ]);
					while((i = readln(infd, buf, 4096)) > 0){
						write(nonamepipein[WRITE], buf, 4096);
					}
				}
				else{
					dupexecp = fork();
					if(!dupexecp){
						dup2(nonamepipein[READ], 0);//dup do std in para o pipe in
						close(nonamepipein[READ]);
						dup2(nonamepipeout[WRITE], 1);//dup do std out para o pipe out
						close(nonamepipeout[WRITE]);
						execv(strcat("./", input[2]), &input[3]);
					}
					else{
						writep = fork();
						if(!writep){
							close(nonamepipeout[WRITE]);
							while((i = readln(nonamepipeout[READ], buf, 4096))){
								write(outfd, buf, 4096);
							}
						}
					}
				}
                kill(getpid(), SIGTERM);
            }
            else{
                nodes[id] = p;
            }
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
    }
    return 0;
}
