#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

	pid_t pid,w;
	pid=fork();
	if(pid==-1){
		perror("fork error");
		exit(1);
	}
	else if(pid==0){
		sleep(3);
		printf("child process:pid=%d\n",getpid());
	}
	else if(pid>0){
		w=wait(NULL);
		printf("Catched a child process,pid=%d\n",w);
	}
	return 0;
}
