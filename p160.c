#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <error.h>

unsigned int my_alarm(unsigned int sec){

	struct itimerval it,oldit;
	int ret;
	it.it_value.tv_sec=0;
	it.it_value.tv_usec=sec;
	it.it_interval.tv_sec=0;
	it.it_interval.tv_usec=0;
	ret=setitimer(ITIMER_REAL,&it,&oldit);
	if(ret==1){
		perror("setitimer");
		exit(1);
	}
	return it.it_value.tv_sec=sec;
}

int main(){

	my_alarm(1);
	while(1) printf("process will finish\n");
	return 0;

}
