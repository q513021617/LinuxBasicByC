#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	pid_t pid1,pid2;
	if((pid1==fork())==0){
		sleep(3);
		printf("child process_1\n");
		exit(0);
		printf("child process_1\n");
	}else{
		if((pid2==fork())==0){
			sleep(1);
			printf("child process-2\n");
		}else{
			wait(NULL);
			wait(NULL);
			printf("info1 from parent process\n");
			printf("info2 from parent process\n");
			return 0;
		}

	}
	return 0;
}
