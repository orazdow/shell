/* Ollie Razdow CS575 Project 1 Shell */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define LINE_LEN 1024
#define ARG_LEN 50
#define HIST_LEN 100

char* args[ARG_LEN];
char line[LINE_LEN];
char memline[LINE_LEN];
int memactive = 0;

struct History{
	char lines[HIST_LEN][LINE_LEN];
	int max_idx;
	int write_idx;
	int read_idx;
};

int getCmd(char* line, char** args);
void startPipe(char* line, char* vbar);
int cd(char** args);
void getCwd(char* *dir);
void add_hist(char* line, struct History *history);
char* read_hist(struct  History *history);
void execCmd(char** args, int bkgd);
void execPipe(char** childargs, char** parentargs);

int main(int argc, char** argv) {

	struct History history;
	history.max_idx = HIST_LEN;
	history.write_idx = 0;
	history.read_idx = 0;

	char* dir = NULL;
	getCwd(&dir);

	while(1){

		int bkgd = 0;
		int pipeactive = 0;

		if(dir)
			printf("myshell%s> ", dir);
		else 
			printf("myshell> ");

		if(memactive)
			printf("%s", memline);

		// get line, remove newline char
		fgets(line, LINE_LEN, stdin);
		char* nl = strchr(line,'\n');
		*nl = '\0';

		// detect bkgd
		char* ampsnd = strchr(line, '&');
		if(ampsnd != NULL){
			*ampsnd = ' ';
			bkgd = 1;
		}

		// detect pipe
		char* vbar = strchr(line, '|');
		if(vbar != NULL)
			pipeactive = 1;

		// get line from history
		char* up = strchr(line,(char)65);
		if(up != NULL){
			memcpy(memline, read_hist(&history), LINE_LEN);
			memactive = 1;
			continue;
		}

		// add line to history
		if(strlen(line) > 0)
			add_hist(line, &history);

		// execute pipe
		if(pipeactive){ 
 			startPipe(line, vbar);
			continue;
		}

		// if enter pressed w/o input:
		if(getCmd(line,args) == 0){ 
			// if history active:
			if(memactive){
				memactive = 0;
				add_hist(memline, &history);
				 vbar = NULL;
				 vbar = strchr(memline, '|');
				 // if pipe command:
				 if(vbar != NULL){
				 	// exec from history
				 	startPipe(memline, vbar);
				 	continue;
				 }else{
				 	// get args from historty if not empty
					if(getCmd(memline,args) == 0)
						continue;	

					if(cd(args)){
						getCwd(&dir);
						continue;
					}
					execCmd(args, bkgd);			 	
				}
			}			
			continue; 			
		}

		// change directory
		if(cd(args)){
			getCwd(&dir);
			continue;
		}

		// execute command
		execCmd(args, bkgd);

	}

    return 0;
}

// parse arguments
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

// change directory
int cd(char** args){
	if(strcmp(args[0], "cd") == 0){
		int cdrtn = chdir(args[1]);
		if(cdrtn != 0)
			printf("cd error: %d\n", cdrtn);
		return 1;
	}	
	return 0;
}

// get current dir
void getCwd(char* *dir){
	static char cwd[LINE_LEN];
	getcwd(cwd, LINE_LEN);
	*dir = strrchr(cwd, '/');
}

// parse args for pipe
void startPipe(char* line, char* vbar){

	*vbar = '\0';
	vbar++;	

	// malloc b/c of pointer problems..
	char** args1 = (char**)malloc(LINE_LEN*ARG_LEN);
	char** args2 = (char**)malloc(LINE_LEN*ARG_LEN);

	getCmd(line, args1);
	getCmd(vbar, args2);
	// fork context
	int f = fork();

	if(f < 0){
		fprintf(stderr, "fork error");
	}else if(f == 0){
		execPipe(args1, args2);
	}else{
		wait(NULL);
	}

	free(args2); 
	free(args1);

}

// add line to history
void add_hist(char* line, struct History *history){	
	memcpy(history->lines[history->write_idx], line, strlen(line));
	history->read_idx = history->write_idx;
	history->write_idx = ++history->write_idx % history->max_idx;
}

// read from history
char* read_hist(struct  History *history){
	char* rtn = history->lines[history->read_idx];
	history->read_idx--;
	if(history->read_idx < 0)
		history->read_idx = history->write_idx-1;
	return rtn;
}

// execute command
void execCmd(char** args, int bkgd){

	int fstatus = fork();

	if(fstatus < 0){ 
		printf("fork error\n");
	}else if(fstatus == 0){ 
		// child context

		execvp(args[0], args);
		//if control returned, return error:
		fprintf(stderr, "execution error\n");
	}else{
		// parent context

		// wait until child process is finished
		int status = 0;
		if(bkgd){
			// run in background
			while(waitpid(-1, 0, WNOHANG) > 0){}
			return;
		}else{
			wait(&status);
			return;
		}

	}
}

// execute pipe
void execPipe(char** childargs, char** parentargs){

	int fd[2]; // 0: stdin 1: stdout
	pipe(fd);

	int freturn = fork();

	if(freturn < 0){
		fprintf(stderr, "fork error\n");
	}else if(freturn == 0){
		// child context

		// dup stdout
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		// exec process
		execvp(childargs[0], childargs);

	}else{
		// parent context
		wait(NULL);
		// dup stdin
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		// exec process
		execvp(parentargs[0], parentargs);

	}

}