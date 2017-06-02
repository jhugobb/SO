#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

int readln(int fildes, char *buf, int buf_size){
	int n = 0, i = 1;
	char c = 0;

	while (i && n < buf_size && c!='\n'){
		i = read(fildes, &c, 1);
		if (i){
			buf[n] = c;
			n++;
		}
		if(c=='\0') break;
	}
	buf[n-1]='\0';
	return n;
}
