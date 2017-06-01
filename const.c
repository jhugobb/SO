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
#include "readln.h"

int main(int argc, char **argv){
    if(argc != 2 ){
        printf("please give 1 arg\n");
        _exit(-1);
    }
    else{
        int i=0;
        char buf[PIPE_BUF];
		while((i = readln(0, buf, PIPE_BUF))>0){
			buf[i-1] = ':';
			strcat(buf, argv[1]);
            strcat(buf,"\n");
			i+=strlen(argv[1]);
			write(1, buf, i+1);
			memset(buf, 0, i);
		}
        return 0;
    }
}
