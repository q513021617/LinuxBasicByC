#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h>

void printset(sigset_t * ped){
	int i;
	for(i=1;i<32;i++){
		if((sigismember(ped,i)==1)){
			putchar('1');
		}else{
			putchar('0');
		}
	}
	printf("\n");
}

int main(){

	sigset_t set,oldset,ped;
	sigemptyset(&set);
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_BLOCK,&set,&oldset);
	while(1){
		sigpending(&ped);
		printset(&ped);
		sleep(1);
	}
	return 0;
}
