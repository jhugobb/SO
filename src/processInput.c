#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

int processInput(char **result, char *buf, char *divider){
	int i;
	for(result[i=0]= strtok(buf, divider); result[i] != NULL; result[i] = strtok(NULL, divider)){
        i++;
    }
	return i;
}
