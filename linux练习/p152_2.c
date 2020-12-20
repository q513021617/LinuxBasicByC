#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	pid_t pid1;
	if((pid1=fork())==0){
		printf("this is child process\n");
		printf(".........start............\n");
			

		execl("cat","");


		printf("..........end.............\n");

	}
	else{
		printf("this is parent process\n");
	}


}
