#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int fd[2];
	int ret=pipe(fd);
	if(ret==-1){
		perror("pip err");
	}
	int i;
	pid_t pid,wpid;
	for(i=0;i<2;i++){
	
	if((pid=fork())==0)
	{
		break;
	}	
	}
	if(2==i){
		close(fd[0]);
		close(fd[1]);
		wpid=wait(NULL);
		printf("wait child 1 success,pid=%d\n",wpid);
		pid=wait(NULL);
		printf("wait child 2 success,pid=%d\n",pid);
	}else if(i==0){
		close(fd[0]);
		dup2(fd[1],STDOUT_FILENO);
		execlp("ls","ls",NULL);
	}else if(i==1){
		close(fd[1]);
		dup2(fd[0],STDOUT_FILENO);
		execlp("wc","wc","-l",NULL);
	}


	return 0;
}
