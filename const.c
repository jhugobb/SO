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

int main(int argc, char **argv){
    if(argc != 2 ){
        printf("please give 1 arg\n");
        _exit(-1);
    }
    else{
        int i=0;
        char buf[4096];
		printf("HEREBIATCH\n");
		while((i = readln(0, buf,4096))>0){
			buf[i-1] = ':';
			strcat(buf, argv[1]);
			i+=strlen(argv[1]);
			strcat(buf, "\n");
			write(1, buf, i+1);
			memset(buf, 0, 4096);
		}
        return 0;
    }
}
