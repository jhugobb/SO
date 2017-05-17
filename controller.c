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

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);//+1 to copy the null-terminator
    return result;
}

ssize_t readln(int fildes, char *buf, size_t nbyte){
	char c;
	ssize_t i = 0, r;
	do{
		r = read(fildes, &c, 1);
		if(r==1){
			buf[i] = c;
			i++;
		}
	} while(r && c != '\n');
	return i;
}

void processInput(char **result, char *buf, char *divider){
	int i;
	for(result[i=0]= strtok(buf, divider); result[i] != NULL; result[i] = strtok(NULL, divider)){
        i++;
    }
}

pid_t node(char** argv){
	int i, infd, outfd,  nonamepipein[2], nonamepipeout[2];
	for(i=0;i<10;nodes[i++]=-1);
	char buf[512], *namein, *nameout;
	pid_t p, readlnp, dupexecp, writep;
	namein = strcat("in", argv[1]);
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
			while((i = readln(infd, buf, 512)) > 0){
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
					while((i = readln(nonamepipeout[READ], buf, 512))>0){
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
    char buf[4096];
	char *input[4096];
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
		}
    }
    return 0;
}
