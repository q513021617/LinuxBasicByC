#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
	pid_t pid,p,w;
	pid = fork();
	if(pid==-1){
		perror("fork error");
		exit(1);
	}else if(pid==0){
		sleep(3);
		printf("First child process:pid=%d\n",getpid());
		exit(0);

	}else if(pid>0){
		do{
			w=waitpid(pid,NULL,WNOHANG);
			if(w==0){
				printf("No Child exited\n");
				sleep(1);
			}
		}while(w==0);
		if(w==pid){
			printf("Catch a Child process:pid=%d\n",w);
		}else printf("waitpid error\n");

	}

	return 0;
}
