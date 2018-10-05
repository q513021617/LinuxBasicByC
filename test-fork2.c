#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	pid_t pid;
	int i;
	for(i=0;i<5;i++){
	if((pid=fork())==0)
		break;
	}
	if(pid==-1){
		perror("fork error");
		exit(1);
	}
	else if(pid >0){
		sleep(5);
		printf("parent pid=%d\n",getpid());
	}
	else if(pid==0){
		sleep(i);
		printf("I am child%d pid=%d\n",i+1,getpid());
	}
	return 0;
}
