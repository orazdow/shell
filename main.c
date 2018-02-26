#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE_LEN 1024
#define ARG_LEN 50


char* args[ARG_LEN];
char line[LINE_LEN];
int forkstatus = 0;

int getCmd(char* line, char** args){
  char* p;
  int aidx = 0;
  if(strlen(line) < 1){return 0;}
  p = strtok(line," ");
  while(p != NULL){
    args[aidx] = p;
    aidx = ++aidx%ARG_LEN;
    p = strtok(NULL, " ");
  }
  args[aidx] = NULL;
  return 1;
}

int execCmd(char** args, int bkgd){

	forkstatus = fork();

	if(forkstatus < 0){ printf("forkError\n");
		return -1;
	}else if(forkstatus == 0){ 
		//child context
		execvp(args[0], args);
		printf("Execution Error");
		//if control returned, return error:
		return -1;
	}else{
		//parent context
		// wait until child process is finished
		int status = 0;
		if(bkgd){
			while(waitpid(-1, 0, WNOHANG) > 0){}
			return 0;
		}else{
			wait(&status);
		}
		if(status == 0){
			return 0;
		}else{
			return -1;
		}
	}


}


int main(int argc, char** argv) {


	while(1){

		int bkgd = 0;
		printf("myshell>");

		// get line, remove newline char
		fgets(line, LINE_LEN-1,stdin);
		char* nl = strchr(line,'\n');
		*nl = '\0';

		char* ampsnd = strchr(line, '&');
		if(ampsnd != NULL){
			*ampsnd = ' ';
			bkgd = 1;
		}

		char* up = strchr(line,(char)27);
		if(up != NULL){
			printf("UP!\n");
			continue;
		}

		if(getCmd(line,args) == 0){ 
			continue; 
		}

		// for(int i = 0; i < ARG_LEN; i++){
		// 	if(args[i])
		// 	printf("%s\n", args[i]);
		// }

		execCmd(args, bkgd);

		memset(&line, 0, LINE_LEN);
		memset(&args, 0, ARG_LEN);

	}

    return 0;
}

/*
wget http://www.orazdow.com/stuff/otter.jpg
wget http://www.orazdow.com/stuff/ha.mp3
*/