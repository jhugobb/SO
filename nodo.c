#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

#define READ 0
#define WRITE 1

int nodes[10];

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

int main(int argc, char** argv){
    int i, infd, outfd,  nonamepipein[2], nonamepipeout[2];
    for(i=0;i<10;nodes[i++]=-1);
    char buf[512], result[512], c;
    pid_t p, readlnp, dupexecp, writep;
    mkfifo("fifoin", 0666);
    mkfifo("fifoout", 0666);
    p = fork();
    if(!p){
        pipe(nonamepipein);
        pipe(nonamepipeout);
        readlnp = fork();
        if(!readlnp){
			infd = open("fifoin", O_RDONLY);
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
                execv("./const", &argv[2]);
            }
            else{
                writep = fork();
                if(!writep){
					outfd = open("fifoout", O_WRONLY);
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
        i = readln(0, buf, 512);
		infd = open("fifoin", O_WRONLY);
		write(infd, buf, i);
		fsync(infd);
		outfd = open("fifoout", O_RDONLY);
        i = readln(outfd, result, 512);
		write(1, result, i);
	   }
    return 0;
}
