#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){

	FILE *r_fp,*w_fp;
	char buf[100];
	r_fp=popen("ls","r");
	w_fp=popen("wc -l","w");
	while(fgets(buf,sizeof(buf),r_fp)!=NULL)
		fputs(buf,w_fp);
	pclose(r_fp);
	pclose(w_fp);
	return 0;
}
