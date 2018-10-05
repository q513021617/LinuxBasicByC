#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){
	pid_t pid; 
	int i;
	for(i=0;i<5;i++)
	pid=fork();


	if(pid==-1){
		perror("fork error");
		exit(1);
	}else if(pid>0)
	{
		printf("parent process,pid=%d,ppid=%d\n",getpid(),getppid());
	
	}else if(pid==0)
	{
		printf("child process,pid=%d,ppid=%d\n",getpid(),getppid());
	
	}
	printf("......finish.....\n");
	return 0;
}
