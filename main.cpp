#include <string.h>
#include <stdio.h>

#define LINE_LEN 1024
#define ARG_LEN 50

using namespace std;

char* args[ARG_LEN];
char line[LINE_LEN];

int getCmd(char* line, char** args){
  char* p;
  int aidx = 0;
  p = strtok(line," \0");
  if(strlen(p) < 1){return 0;}
  while(p != NULL){
    args[aidx] = p;
    aidx = ++aidx%ARG_LEN;
    p = strtok(NULL, " \0");
  }
  return 1;
}


int main(int argc, char** argv) {

	// char line[] = "hi hi hooo randal!";
	// getCmd(line,args);
	// for(int i = 0; i < ARG_LEN; i++){
	// 	printf("%s\n",args[i]);
	// }

	while(1){

		printf("myshell>");

		fgets(line, LINE_LEN-1,stdin);

		if(getCmd(line,args) == 0){ 
			continue; 
		}

		for(int i = 0; i < ARG_LEN; i++){
			if(args[i])
			printf("%s\n", args[i]);
		}

		memset(&line, 0, LINE_LEN);
		memset(&args, 0, ARG_LEN);

	}

    return 0;
}