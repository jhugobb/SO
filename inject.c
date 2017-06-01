#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include "readln.h"
#include "concat.h"
#include <limits.h>

int main(int argc, char** argv){
    int i, nonamepipeout[2];
    char* id = "";
    id = concat("in", argv[1]);
    char buf[PIPE_BUF];
    pid_t p;
    pipe(nonamepipeout);
    p = fork();
    if(!p){
        close(nonamepipeout[0]);
        dup2(nonamepipeout[1], 1);
        close(nonamepipeout[1]);
        execv(argv[2], &argv[2]);
    }
    close(nonamepipeout[1]);
    int infd = open(id,	O_WRONLY);
    i = readln(nonamepipeout[0], buf, PIPE_BUF);
	write(infd, buf, i);
	fsync(infd);
    close(nonamepipeout[0]);
    close(infd);
    kill(p,SIGTERM);

    return 0;
}
