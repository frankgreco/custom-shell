#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

void execute(char* cmd, char** arr);

void parse_file(char** arr, char* cmd);

int main(int argc, const char* argv[]){

	printf("\nWELCOME TO MY SHELL! My PID is: %d\n\n", getpid());

	char input[1024];
	char* cmd;
	char** arr = malloc(sizeof(char*) * 1024);
	char* tmp;
	int flag = 1;
        do {
        	printf("Enter a command for PID %d: ", getpid());

		//read in input
        	fgets(input, 1024, stdin);
		//lets not waste memory
		tmp = strcpy((char*)malloc(sizeof(char) * (strlen(input) + 1)), input);

		//if user enters 'exit' terminate program
		if(strcmp(tmp, "exit\n") == 0){
			flag = 0;
			exit(0);
		}else{
			//new line characters is at the end of tmp, lets get rid of it
			char *newline = strchr( tmp, '\n' );
			if ( newline ) *newline = 0;

			//parse every argument into an array of strings
			cmd = strtok(tmp, " ");

			if(strcmp(cmd, "multi") == 0){ //parse commands from a file
				parse_file(arr, cmd);
			}else{ //parse commands from command line
				if(strcmp(cmd, "./main") == 0) printf("\nWELCOME TO MY SHELL! My PID is: %d\n\n", getpid());
				execute(cmd, arr);
			}
		}

	} while ( flag );

	//if program finished successfully
	return 0;
}

void parse_file(char** arr, char* cmd){
	char* file_name;
	char* line = NULL;
        size_t len = 0;
        ssize_t read;
	char* output_command;
	char* output_file_name;
        file_name = strtok(NULL, " ");

	/*if((output_command = strtok(NULL, " ")) != NULL){
		if(((output_file_name = strtok(NULL, " ")) != NULL) && ((strcmp("output", output_command)) == 0)){
			//redirect
		}else printf("command not recognized\n");
	}*/


        FILE* file = fopen(file_name, "r");
	if(file == NULL){
		printf("%s\n", strerror(errno));
	}else{
	        while ((read = getline(&line, &len, file)) != -1) {
        		char *newline = strchr( line, '\n' );
                	if ( newline ) *newline = 0;
                	cmd = strtok(line, " ");
                	if(strcmp(cmd, "multi") == 0){ //nested multi command
                		//recursion
				parse_file(arr, cmd);
                	}else{
                        	execute(cmd, arr);
                	}
		}
        	fclose(file);
	}
}


void execute(char* cmd, char** arr){

	pid_t pid = fork();

	if(pid == 0){ //child

		int i = 0;
        	arr[i++] = strcpy((char*)malloc(sizeof(char) * (strlen(cmd)+1)), cmd);
        	while(cmd != NULL){
                	cmd = strtok(NULL, " ");
                	if(cmd != NULL) arr[i++] = strcpy((char*)malloc(sizeof(char) * (strlen(cmd)+1)), cmd);
       	 	}

        	//to execute the command, we'll use execvp so we can pass in an n number of arguments
        	execvp(arr[0], arr);

		exit(errno);
	}else if(pid > 0){ //parent
		int status;
    		wait(&status);
    		if(WIFEXITED(status))
         		if(WEXITSTATUS(status) != 0) printf("command not recognized!\n");
	}

}
