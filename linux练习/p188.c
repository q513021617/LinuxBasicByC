#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#define MAX_TEXT 512

struct my_msg_st{
	long int my_msg_type;
	char anytext[MAX_TEXT];
};

int main(){

	int idx=1;
	int msqid;
	struct my_msg_st data;
	char buf[BUFSIZ];
	msqid=msgget((key_t)1000,0664|IPC_CREAT);
	if(msqid==-1){
		perror("msgget err");
		exit(-1);
	}

}
