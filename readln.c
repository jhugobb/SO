#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

ssize_t readln(int fildes, char *buf, size_t nbyte){
	char c;
	ssize_t i = 0, r;
	do{
		r = read(fildes, &c, 1);
		if(r==1){
			buf[i] = c;
			i++;
		}
	} while(r && i<nbyte && c != '\n');
	return i;
}
