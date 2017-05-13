#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

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
    int i=0, id;
    for(i=0;i<10;nodes[i++]=-1);
    char buf[4096];
    pid_t p;
    while((i = readln(0, buf, 4096)) > 0){
        if(!strncmp(buf, "node", 4)){
            p = fork();
            if(!p){
                kill(getpid(), SIGTERM);
            }
            else{
				id = getNodeid(buf);
                nodes[id] = p;
            }
        }
        else if(!strncmp(buf, "connect", 7)){
            printf("You typed connect!\n");
        }
        else if(!strncmp(buf, "disconnect", 10)){
            printf("You typed disconnect!\n");
        }
        else if(!strncmp(buf, "inject", 6)){
            printf("You typed inject!\n");
        }
        else if(!strncmp(buf, "print", 5)){
            for(i=0;i<10;i++){
                if(nodes[i]!=-1) printf("I EXIST: %d\n", nodes[i]);
            }
        }
    }
    return 0;
}
