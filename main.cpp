#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define LINE_LEN 1024
#define ARG_LEN 50

using namespace std;

char* args[ARG_LEN];
char line[LINE_LEN];
int forkstatus = 0;

int getCmd(char* line, char** args){
  char* p;
  int aidx = 0;
  p = strtok(line," ");
  if(strlen(p) < 1){return 0;}
  while(p != NULL){
    args[aidx] = p;
    aidx = ++aidx%ARG_LEN;
    p = strtok(NULL, " ");
  }
  args[aidx] = NULL;
  return 1;
}

int execCmd(char** args, bool bkgd){

	forkstatus = fork();


	if(forkstatus < 0){
		return -1;
	}else if(forkstatus == 0){
		//child context
		execvp(args[0], args);
		printf("execError");
		// exit(-1);
		//if control returned, return error:
		return -1;
	}else{
		// if (bkgd {return 0})
		//parent context
		// wait until child process is finished
		int status = 
		wait(&status);
		if(status == 0){
			return 0;
		}else{
			return -1;
		}
	}


}


int main(int argc, char** argv) {


	while(1){

		printf("myshell>");

		// get line, remove newline char
		fgets(line, LINE_LEN-1,stdin);
		char* nl = strchr(line,'\n');
		*nl = '\0';

		if(getCmd(line,args) == 0){ 
			continue; 
		}

		// for(int i = 0; i < ARG_LEN; i++){
		// 	if(args[i])
		// 	printf("%s\n", args[i]);
		// }
		
		execCmd(args, 0);

		memset(&line, 0, LINE_LEN);
		memset(&args, 0, ARG_LEN);

	}

    return 0;
}