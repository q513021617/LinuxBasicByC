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
		sleep(5);
		printf("First child process:pid=%d\n",getpid());
		exit(5);

	}else if(pid>0){
		int i;
		p=pid;
		for(i=0;i<3;i++){
			if((pid=fork())==0) break;
		}
		if(pid==-1){
			perror("fork error");
			exit(2);
		}else if(pid==0){
			printf("Child process:pid=%d\n",getpid());
			exit(0);
		}else if(pid>0){
			w=waitpid(p,NULL,0);
			if(w==p) printf("Catch a child Process:pid=%d\n",w);
			else printf("waitpid error\n");
		}

	}

	return 0;
}
