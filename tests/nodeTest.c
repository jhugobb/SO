#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <limits.h>

#define READ 0
#define WRITE 1

int main(int argc, char** argv){
    int i, infd, outfd,  nonamepipein[2], nonamepipeout[2];
    char buf[PIPE_BUF], result[PIPE_BUF], c;
    pid_t p, readlnp, dupexecp, writep;
    mkfifo("fifoin", 0666);
    mkfifo("fifoout", 0666);
    p = fork();
    if(!p){
        pipe(nonamepipein);
        pipe(nonamepipeout);
        readlnp = fork();
        if(!readlnp){
            close(nonamepipein[READ]);
            infd = open("fifoin", O_RDONLY);
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
                execv(argv[2], &argv[2]);

            }
            else{
                writep = fork();
                if(!writep){
                    close(nonamepipeout[WRITE]);
                    outfd = open("fifoout", O_WRONLY);
                    while((i = readln(nonamepipeout[READ], buf, PIPE_BUF))>0){
                        write(outfd, buf, i);
                        fsync(outfd);
                    }
                }
            }
        }
    }
    else{
        i = readln(0, buf, PIPE_BUF);
        infd = open("fifoin", O_WRONLY);
        write(infd, buf, i);
        fsync(infd);
        outfd = open("fifoout", O_RDONLY);
        i = read(outfd, result, PIPE_BUF);
        write(1, result, i);
    }
    return 0;
}
