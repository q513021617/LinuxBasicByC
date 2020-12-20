#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

	alarm(1);
	while(1){
		printf("process will finish\n");
	}
	return 0;
}
